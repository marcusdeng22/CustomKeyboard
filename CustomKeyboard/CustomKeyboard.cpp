#include "stdafx.h"

#include "CustomKeyboard.h"

// Global Variables:
HINSTANCE hInst;
HANDLE hSingleInstanceMutex;
HHOOK hKeyboardHook;
HWINEVENTHOOK activeWindowHook;
HWINEVENTHOOK minActiveWindowHook;

std::map<DWORD, LogiLed::KeyName> gKeyAssignments({
	{VK_F16, LogiLed::KeyName::G_1},
	{VK_F17, LogiLed::KeyName::G_2},
	{VK_F18, LogiLed::KeyName::G_3},
	{VK_F19, LogiLed::KeyName::G_4},
	{VK_F20, LogiLed::KeyName::G_5},
	{VK_F21, LogiLed::KeyName::G_6},
	{VK_F22, LogiLed::KeyName::G_7},
	{VK_F23, LogiLed::KeyName::G_8},
	{VK_F24, LogiLed::KeyName::G_9},
	});

std::map<std::string, std::shared_ptr<Profile>> profiles;

std::shared_ptr<Profile> curProfile;

std::wstring settingsPath;
nlohmann::json settings;

std::mutex updating;

std::thread painter;

bool resuming = false;

HPOWERNOTIFY standbyHandle;
static ULONG isConnectedStandby(PVOID Context, ULONG type, PVOID Setting);
ULONG isConnectedStandby(PVOID Context, ULONG type, PVOID Setting) {
	OutputDebugString(L"\n\nconnected standby request!!!!\n");

	if (type == PBT_APMSUSPEND) {
		//we are suspending, so set an event to reregister application
		OutputDebugString(L"about to suspend\n");
		stopPainter();
	}
	else if (type == PBT_APMRESUMEAUTOMATIC) {
		OutputDebugString(L"resuming from standby\n");
		resuming = true;
		stopPainter();
		curProfile = nullptr;	//need to notify setActiveProfile that we need to restart
		LogiLedShutdown();
		Sleep(10000);
		int attempts = 0;
		while (attempts < 5) {
			char buf[256];
			sprintf_s(buf, "attempting to reinit; attempt %d\n", attempts);
			OutputDebugStringA(buf);
			if (initLogi()) {
				setActiveProfile();
				break;
			}
			LogiLedShutdown();
			Sleep(5000);
			attempts++;
		}
		if (attempts >= 5) {
			MessageBox(NULL, L"CUSTOM KEYBOARD FAILED TO RESUME; PLEASE RESTART", L"ERROR", MB_OK);
		}
		resuming = false;
	}
	return 0;
}

static _DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS testCallback = {
	isConnectedStandby,
	nullptr
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Perform application initialization:
	if (!initInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	//TODO: watch it: https://stackoverflow.com/questions/931093/how-do-i-make-my-program-watch-for-file-modification-in-c
	HMODULE mod = GetModuleHandleW(NULL);
	WCHAR path[MAX_PATH] = L"";
	GetModuleFileNameW(mod, path, MAX_PATH);
	PathCchRemoveFileSpec(path, MAX_PATH);
	

	HANDLE settingsHandle = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	std::thread fileWatcher(watchFile, settingsHandle);


	wcscat_s(path, L"\\settings.json");
	OutputDebugString(L"Path to settings: ");
	OutputDebugStringW(path);
	OutputDebugString(L"\n");
	settingsPath = std::wstring(path);

	parseSettings();

	//std::thread t(&ProfileColor::sendColor, std::ref(curProfile->colorProfile));	//http://www.cplusplus.com/forum/beginner/189296/
	//painter = std::thread(&ProfileColor::sendColor, std::ref(curProfile->colorProfile));	//http://www.cplusplus.com/forum/beginner/189296/

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MICMUTETOGGLE));

	MSG msg;
	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	//cleanup here
	UnhookWindowsHookEx(hKeyboardHook);
	UnhookWinEvent(activeWindowHook);
	UnhookWinEvent(minActiveWindowHook);
	PowerUnregisterSuspendResumeNotification(standbyHandle);
	CloseHandle(settingsHandle);
	ReleaseMutex(hSingleInstanceMutex);
	OutputDebugString(L"released mutex");

	return (int)msg.wParam;
}

void stopPainter() {
	//stop the current painter
	if (curProfile != nullptr) {
		curProfile->colorProfile.stop();
	}
	if (painter.joinable()) {
		painter.join();
		//painter.~thread();
	}
}

void startPainter() {
	if (curProfile != nullptr) {
		std::thread newPainter(&ProfileColor::sendColor, std::ref(curProfile->colorProfile));
		std::swap(painter, newPainter);	//frees up old thread to be destroyed
		if (newPainter.joinable()) {
			newPainter.join();
		}
		newPainter.~thread();
	}
}

void watchFile(HANDLE h) {
	FILE_NOTIFY_INFORMATION strNotify[512];
	DWORD bytesRet = 0;
	while (true) {
		try {
			if (ReadDirectoryChangesW(h, (LPVOID)&strNotify, sizeof(strNotify), FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE, &bytesRet, NULL, NULL) == 0) {
				OutputDebugString(L"0 condition met on update file\n");
			}
			else {
				OutputDebugStringW(L"1 condition met on update file; updating settings\n");
				parseSettings();
			}
		}
		catch (...) {
			OutputDebugString(L"watch had an exception\n");
			MessageBox(NULL, L"CUSTOM KEYBOARD INSTANCE FAILED TO READ FILE", L"ERROR", MB_OK);
		}
	}
}

void parseSettings() {
	//lock so no other modifications can be made
	updating.lock();
	using json = nlohmann::json;
	std::ifstream ifs(settingsPath.c_str());
	settings = json::parse(ifs);

	//stop the current painter
	stopPainter();

	profiles.clear();
	for (const auto& profileJSON : settings.items()) {
		profiles.insert({ profileJSON.key(), std::make_shared<Profile>() });	//create new profile
		std::shared_ptr<Profile> curProfile = profiles[profileJSON.key()];
		for (const auto& layerJSON : profileJSON.value()["layers"].items()) {	//parse layer data
			//open the dict for reading
			for (const auto& layerData : layerJSON.value().items()) {
				//we now have the layer type
				//need to check for the key value pairs; if they don't exist, set default values
				std::string layerName = layerData.key();
				if (layerName == "Cycle") {
					int d = layerData.value().value<int>("/delta"_json_pointer, 5);
					if (d <= 0) {
						d = 5;
					}
					//add a cycle layer
					curProfile->colorProfile.addLayer<Cycle>(d);
				}
				else if (layerName == "Echo") {
					double d = layerData.value().value<double>("/delta"_json_pointer, 0.01);
					if (d <= 0) {
						d = 0.01;
					}
					std::vector<int> start = layerData.value().value<std::vector<int>>("/start"_json_pointer, { 255, 255, 255 });
					std::vector<int> end = layerData.value().value<std::vector<int>>("/end"_json_pointer, {});
					//add an echo layer
					curProfile->colorProfile.addLayer<Echo>(d, Color(start), Color(end));
				}
				else if (layerName == "Fill") {
					std::string cond = layerData.value().value<std::string>("/condition"_json_pointer, "None");
					OutputDebugString(L"Condition: ");
					OutputDebugStringA(cond.c_str());
					OutputDebugString(L"\n");
					//array of color arrays
					std::vector<Color> colorList;
					for (const auto& colorData : layerData.value()["color"]) {
						if (colorData.is_array()) {
							std::vector<int> temp = colorData;
							colorList.push_back(Color(temp));
						}
					}
					std::vector<std::string> k = layerData.value().value<std::vector<std::string>>("/keys"_json_pointer, {});
					//add a fill layer
					if (k.size() == 0) {
						//curProfile->colorProfile.addLayer<Fill>(convertCondition(cond), Color(c));
						curProfile->colorProfile.addLayer<Fill>(convertCondition(cond), colorList);
					}
					else {
						//curProfile->colorProfile.addLayer<Fill>(convertList(k), convertCondition(cond), Color(c));
						curProfile->colorProfile.addLayer<Fill>(convertList(k), convertCondition(cond), colorList);
					}
				}
			}
		}
		for (const auto& macroJSON : profileJSON.value()["macros"].items()) {	//parse macro data
			std::string keyName = macroJSON.key();
			for (const auto& keyJSON : macroJSON.value().items()) {
				std::string t = keyJSON.key();
				//open dict for reading
				std::string type = keyJSON.value().value<std::string>("/type"_json_pointer, "NONE");
				if (type == "keySeq") {
					std::vector<std::string> k = keyJSON.value().value<std::vector<std::string>>("/seq"_json_pointer, {});
					std::vector<int> innerDelayData = keyJSON.value().value<std::vector<int>>("/innerDelay", {});
					int defaultDelay = keyJSON.value().value<int>("/delay"_json_pointer, 50);
					std::string kT = keyJSON.value().value<std::string>("/keyType"_json_pointer, "virtual");
					//parse keys
					bool keyType = (kT == "virtual");
					std::vector<KeyState> keySeq = parseKeyInput(k, keyType);
					std::vector<int> delaySeq = parseKeyDelay(keySeq, innerDelayData, defaultDelay);
					curProfile->addMacro(convertKey(keyName), keySeq, delaySeq, defaultDelay);
				}
				else if (type == "launchApp") {
					std::string path = keyJSON.value().value<std::string>("/path"_json_pointer, "");
					std::string args = keyJSON.value().value<std::string>("/args"_json_pointer, "");
					curProfile->addMacro(convertKey(keyName), path, args);
				}
				else if (type == "power" || type == "audioCycle" || type == "micToggle") {
					curProfile->addMacro(convertKey(keyName), toMacroType.at(type));
				}
				else if (type == "delay") {
					int d = keyJSON.value().value<int>("/delay"_json_pointer, 50);
					curProfile->addMacro(convertKey(keyName), d);
				}
			}
		}
	}
	//verify there is a default profile; if none set an empty profile
	if (profiles.find("default") == profiles.end()) {
		profiles.insert({ "default", std::make_shared<Profile>() });
	}
	setActiveProfile(false);

	updating.unlock();
}

LRESULT CALLBACK globalKeyboardHook(int code, WPARAM wParam, LPARAM lParam) {
	//this function is synchronous
	updating.lock();
	if (resuming) {
		updating.unlock();
		return CallNextHookEx(0, code, wParam, lParam);
	}
	//Sleep(500);	//key up may occur before key down function finishes
	KBDLLHOOKSTRUCT* event = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

	bool consumed = false;
	LogiLed::KeyName eventKey = LogiLed::KeyName::NONE;


	if (event->flags & LLKHF_INJECTED) {
		OutputDebugString(L"injected key!\n");
		event->flags = event->flags ^ LLKHF_INJECTED;
		//event->flags = event->flags ^ LLKHF_LOWER_IL_INJECTED;
	}


	if (event->vkCode == VK_RETURN && event->flags & LLKHF_EXTENDED) {
		eventKey = LogiLed::KeyName::NUM_ENTER;
	}
	else if (gKeyAssignments.find(event->vkCode) != gKeyAssignments.end()) {	//check the g keys first
		eventKey = gKeyAssignments.at(event->vkCode);
		//trigger the macro on key up
		if (wParam == WM_KEYUP || wParam == WM_SYSKEYDOWN) {
			std::thread t(&Profile::execMacro, std::ref(*curProfile), (DWORD)eventKey);
			t.detach();	//these threads should finish; hard to break? TODO: verify this
		}
		consumed = true;	//consume and don't pass the event on
	}
	else {
		auto ek = LogiLed::scanCodeToKeyName.find(event->vkCode);
		if (ek != LogiLed::scanCodeToKeyName.end()) {
			eventKey = ek->second;
		}
	}
	if (eventKey == LogiLed::KeyName::NONE) {
		updating.unlock();
		return CallNextHookEx(0, code, wParam, lParam);
	}

	//echo effect registering here
	//TODO: capture ctrl_alt_delete and ctrl_shift_escape and force first two key ups
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
		curProfile->colorProfile.notifyEcho(eventKey);
	}

	if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
		curProfile->colorProfile.notifyEchoStart(eventKey);
	}

	/*if (consumed) {
		updating.unlock();
		return S_FALSE;
	}*/
	updating.unlock();
	return CallNextHookEx(0, code, wParam, lParam);
}

void setActiveProfile(bool needToAcquire) {
	CHAR path[MAX_PATH] = { 0 };
	DWORD dwProcId = 0;
	GetWindowThreadProcessId(GetForegroundWindow(), &dwProcId);
	//char buf[256];
	//sprintf_s(buf, "process id %d\n", dwProcId);
	//OutputDebugStringA(buf);
	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcId);
	if (hProc) {
		//sprintf_s(buf, "handle is %p\n", hProc);
		//OutputDebugStringA(buf);

		GetModuleFileNameExA(hProc, NULL, path, MAX_PATH);
		//OutputDebugString(L"Window changed to: ");
		//OutputDebugStringA(path);
		//OutputDebugString(L"\n");
		CloseHandle(hProc);

		//check if new window is in list of profiles; if so, stop the current thread and switch to the new profile
		//std::wstring pa = buffer;
		auto i = profiles.find(path);
		if (i != profiles.end()) {
			if (needToAcquire) {
				updating.lock();
			}
			//stop the current painter
			stopPainter();
			curProfile = i->second;
			startPainter();
			if (needToAcquire) {
				updating.unlock();
			}
		}
		else {
			//if we are still on the default profile, don't do anything
			if (curProfile != profiles["default"]) {
				if (needToAcquire) {
					updating.lock();
				}
				//stop the current painter
				stopPainter();
				curProfile = profiles["default"];
				startPainter();
				if (needToAcquire) {
					updating.unlock();
				}
			}
		}
	}
}

void activeWindowChanged(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
	setActiveProfile();
}

BOOL initLogi(bool msgBox) {
	bool LedInit = LogiLedInitWithName("custom_keyboard");
	if (!LedInit) {
		OutputDebugString(L"failed to initialize\n");
		if (msgBox) {
			MessageBox(NULL, L"CUSTOM KEYBOARD FAILED TO INITIALIZE LED SDK", L"ERROR", MB_OK);
		}
		return FALSE;
	}
	OutputDebugString(L"initialized SDK\n");
	LogiLedSetTargetDevice(LOGI_DEVICETYPE_PERKEY_RGB | LOGI_DEVICETYPE_PERKEY_RGB_ORD);
	return TRUE;
}

BOOL initInstance(HINSTANCE hInstance, int nCmdShow)	//code enters from here
{
	hInst = hInstance;

	hSingleInstanceMutex = CreateMutex(nullptr, FALSE, L"Global\\custom_keyboard");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBox(NULL, L"CUSTOM KEYBOARD INSTANCE ALREADY RUNNING", L"ERROR", MB_OK);
		return FALSE;
	}

	if (!initLogi(true)) {
		return FALSE;
	}
	//register hooks
	hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, globalKeyboardHook, nullptr, 0); // hook for keyboard events
	activeWindowHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL, activeWindowChanged, 0, 0, WINEVENT_OUTOFCONTEXT);
	minActiveWindowHook = SetWinEventHook(EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND, NULL, activeWindowChanged, 0, 0, WINEVENT_OUTOFCONTEXT);
	//TODO: eventually add an audio endpoint event hook:https://docs.microsoft.com/en-us/windows/win32/api/mmdeviceapi/nf-mmdeviceapi-immdeviceenumerator-registerendpointnotificationcallback?redirectedfrom=MSDN
	//TODO: create a global endpoint device to listen ^ to reduce the overhead on switching/detecting changes in devices

	//register a callback on standby resume
	PowerRegisterSuspendResumeNotification(DEVICE_NOTIFY_CALLBACK, &testCallback, &standbyHandle);
	return TRUE;
}
