#include "stdafx.h"

#include "CustomKeyboard.h"


// Configuration

namespace {
	const auto SWITCH_KEY = VK_F23;
	const auto TOGGLE_KEY = VK_F24;
};

// Global Variables:
HINSTANCE hInst;
HANDLE hSingleInstanceMutex;
HHOOK hKeyboardHook;
HWINEVENTHOOK activeWindowHook;
IAudioEndpointVolume* micVolume;	//TODO: use a smart pointer

std::map<DWORD, LogiLed::KeyName> defaultMacroAssignments({
	{VK_F23, LogiLed::KeyName::G_6},
	{VK_F24, LogiLed::KeyName::G_7},
	{VK_ESCAPE, LogiLed::KeyName::G_1},
	{VK_LCONTROL, LogiLed::KeyName::G_5}
});

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

//Profile* curProfile;
std::shared_ptr<Profile> curProfile;

std::wstring settingsPath;
nlohmann::json settings;

std::vector<std::thread> threadManager;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Perform application initialization:
	if (!initInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	//TODO: read a file here to create the profiles
	//TODO: watch it: https://stackoverflow.com/questions/931093/how-do-i-make-my-program-watch-for-file-modification-in-c
	HMODULE mod = GetModuleHandleW(NULL);
	WCHAR path[MAX_PATH] = L"";
	GetModuleFileNameW(mod, path, MAX_PATH);
	PathCchRemoveFileSpec(path, MAX_PATH);
	wcscat_s(path, L"\\settings.json");
	OutputDebugString(L"Path to settings: ");
	OutputDebugStringW(path);
	OutputDebugString(L"\n");
	settingsPath = std::wstring(path);
	parseSettings();

	/*Profile defaultProfile;
	defaultProfile.colorProfile.AddLayer<Cycle>();
	defaultProfile.colorProfile.AddLayer<Echo>();
	defaultProfile.colorProfile.AddLayer<Fill>(LogiLed::KeyName::G_7, Condition::MicMuted);

	curProfile = &defaultProfile;*/

	std::thread t(&ProfileColor::sendColor, std::ref(curProfile->colorProfile));	//http://www.cplusplus.com/forum/beginner/189296/

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

	CoUninitialize();
	ReleaseMutex(hSingleInstanceMutex);
	OutputDebugString(L"released mutex");

	return (int)msg.wParam;
}

void parseSettings() {
	using json = nlohmann::json;
	std::ifstream ifs(settingsPath.c_str());
	settings = json::parse(ifs);

	profiles.clear();
	OutputDebugString(L"begin of JSON:\n");
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
					//add a cycle layer
					curProfile->colorProfile.addLayer<Cycle>(d);
				}
				else if (layerName == "Echo") {
					double d = layerData.value().value<double>("/delta"_json_pointer, 0.01);
					std::vector<int> start = layerData.value().value<std::vector<int>>("/start"_json_pointer, { 255, 255, 255 });
					std::vector<int> end = layerData.value().value<std::vector<int>>("/end"_json_pointer, {});
					//add an echo layer
					curProfile->colorProfile.addLayer<Echo>(d, Color(start), Color(end));
				}
				else if (layerName == "Fill") {
					std::string cond = layerData.value().value<std::string>("/condition"_json_pointer, "None");
					std::vector<int> c = layerData.value().value<std::vector<int>>("/color"_json_pointer, { 255, 255, 255 });
					std::vector<std::string> k = layerData.value().value<std::vector<std::string>>("/keys"_json_pointer, {});
					//add a fill layer
					if (k.size() == 0) {
						curProfile->colorProfile.addLayer<Fill>(convertCondition(cond), Color(c));
					}
					else {
						curProfile->colorProfile.addLayer<Fill>(convertList(k), convertCondition(cond), Color(c));
					}
				}
			}
		}
		for (const auto& macroJSON : profileJSON.value()["macros"].items()) {	//parse macro data
			std::string keyName = macroJSON.key();
			//curProfile->addMacro();
			OutputDebugStringA(keyName.c_str());
			OutputDebugString(L"\n");
			for (const auto& keyJSON : macroJSON.value().items()) {
				std::string t = keyJSON.key();
				OutputDebugStringW(L"\t");
				OutputDebugStringA(t.c_str());
				OutputDebugString(L"\n");
				//open dict for reading
				std::string type = keyJSON.value().value<std::string>("/type"_json_pointer, "NONE");
				OutputDebugStringA(type.c_str());
				OutputDebugString(L"\n");
				if (type == "keySeq") {
					std::vector<std::string> k = keyJSON.value().value<std::vector<std::string>>("/seq"_json_pointer, {});
					std::vector<int> innerDelayData = keyJSON.value().value<std::vector<int>>("/innerDelay", {});
					int defaultDelay = keyJSON.value().value<int>("/delay"_json_pointer, 50);
					//parse keys
					std::vector<KeyState> keySeq = parseKeyInput(k);
					std::vector<int> delaySeq = parseKeyDelay(keySeq, innerDelayData, defaultDelay);
					curProfile->addMacro(convertKey(keyName), keySeq, delaySeq, defaultDelay);
				}
				else if (type == "launchApp") {
					std::string p = keyJSON.value().value<std::string>("/path"_json_pointer, "");
					std::string a = keyJSON.value().value<std::string>("/args"_json_pointer, "");
					LPCSTR path = p.c_str();
					LPSTR args = &(a[0]);
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
	OutputDebugString(L"-----------------------------\n");

	//verify there is a default profile; if none deinitialize and wait until another update

	//profiles.insert({ "default", std::make_shared<Profile>() });
	//profiles["default"]->colorProfile.addLayer<Cycle>();
	//profiles["default"]->colorProfile.addLayer<Echo>();
	//profiles["default"]->colorProfile.addLayer<Fill>(LogiLed::KeyName::G_7, Condition::MicMuted);

	curProfile = profiles["default"];
	//curProfile = profiles["C:\\Program Files (x86)\\Steam\\steamapps\\common\\Warframe\\Warframe.x64.exe"];
}

HRESULT setDefaultAudioPlaybackDevice(LPCWSTR devID) {
	IPolicyConfigVista* pPolicyConfig;

	HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient), NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID*)&pPolicyConfig);
	if (SUCCEEDED(hr))
	{
		hr = pPolicyConfig->SetDefaultEndpoint(devID, ERole::eConsole);
		pPolicyConfig->Release();
	}
	return hr;
}

void toggleMute() {
	//HRESULT hr = CoInitialize(NULL);
	//if (SUCCEEDED(hr)) {
		IMMDeviceEnumerator* de;
		HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&de);
		if (SUCCEEDED(hr)) {
			IMMDevice* micDevicePtr;
			hr = de->GetDefaultAudioEndpoint(EDataFlow::eCapture, ERole::eCommunications, &micDevicePtr);
			if (SUCCEEDED(hr)) {
				hr = micDevicePtr->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&micVolume);
				if (SUCCEEDED(hr)) {
					BOOL wasMuted;
					micVolume->GetMute(&wasMuted);

					//toggle the mute
					micVolume->SetMute(!wasMuted, nullptr);
				}
				micDevicePtr->Release();
			}
			de->Release();
		}
	//}
	//CoUninitialize();
}

void swapOutput() {
	//HRESULT hr = CoInitialize(NULL);
	//if (SUCCEEDED(hr)) {
		IMMDeviceEnumerator* de;
		HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&de);
		if (SUCCEEDED(hr)) {
			IMMDeviceCollection* outputDeviceList;
			de->EnumAudioEndpoints(EDataFlow::eRender, DEVICE_STATE_ACTIVE, &outputDeviceList);
			if (SUCCEEDED(hr)) {
				//get the default
				IMMDevice* defaultOutput;
				LPWSTR pstrDefault = NULL;	//to store the ID of the default
				hr = de->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eMultimedia, &defaultOutput);
				if (SUCCEEDED(hr)) {
					hr = defaultOutput->GetId(&pstrDefault);
					if (SUCCEEDED(hr)) {
						UINT count;
						hr = outputDeviceList->GetCount(&count);
						if (SUCCEEDED(hr)) {
							//loop through all output devices, and once we find the default, set the next index (wraparound) to be the new default
							IMMDevice* nextOutput;
							LPWSTR pstrNextId = NULL;
							if (count != 1) {	//more than one device to switch between
								for (ULONG i = 0; i < count; i++) {
									hr = outputDeviceList->Item(i, &nextOutput);
									if (SUCCEEDED(hr)) {
										hr = nextOutput->GetId(&pstrNextId);
										if (SUCCEEDED(hr)) {
											if (!lstrcmpi(pstrNextId, pstrDefault)) {	//same IDs
												//set the next as the new default and break
												i++;
												if (i == count) {	//wraparound
													i = 0;
												}
												hr = outputDeviceList->Item(i, &nextOutput);
												if (SUCCEEDED(hr)) {
													hr = nextOutput->GetId(&pstrNextId);
													if (SUCCEEDED(hr)) {
														hr = setDefaultAudioPlaybackDevice(pstrNextId);
														i = count + 1;	//simple way to break and run cleanup
													}
													nextOutput->Release();
												}
											}
										}
										nextOutput->Release();
									}
								}
							}
						}
					}
					defaultOutput->Release();
				}
				outputDeviceList->Release();
			}
			de->Release();
		}
	//}
	//CoUninitialize();
}

LogiLed::KeyName convertFromScan(DWORD scanCode) {
	return LogiLed::KeyName(scanCode);
}

LRESULT CALLBACK globalKeyboardHook(int code, WPARAM wParam, LPARAM lParam) {
	/*if (code != 0 || lParam == 0 || (wParam != WM_KEYDOWN && wParam != WM_KEYUP)) {
		return CallNextHookEx(0, code, wParam, lParam);
	}*/

	KBDLLHOOKSTRUCT* event = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
	if (event->vkCode == TOGGLE_KEY) {
		if (wParam == WM_KEYUP) {
			toggleMute();
		}
	}

	if (event->vkCode == SWITCH_KEY) {
		if (wParam == WM_KEYUP) {
			//switch audio output here
			swapOutput();
		}
	}

	bool consumed = false;
	LogiLed::KeyName eventKey = LogiLed::KeyName::NONE;
	if (event->vkCode == VK_RETURN && event->flags & LLKHF_EXTENDED) {
		eventKey = LogiLed::KeyName::NUM_ENTER;
	}
	//else if (defaultMacroAssignments.find(event->vkCode) != defaultMacroAssignments.end()) {	//check the g keys first
	else if (gKeyAssignments.find(event->vkCode) != gKeyAssignments.end()) {	//check the g keys first
		eventKey = gKeyAssignments.at(event->vkCode);
		//trigger the macro
		curProfile->execMacro((DWORD)eventKey);
		consumed = true;	//consume and don't pass the event on
	}
	else {
		auto ek = LogiLed::scanCodeToKeyName.find(event->vkCode);
		if (ek != LogiLed::scanCodeToKeyName.end()) {
			eventKey = ek->second;
		}
	}
	if (eventKey == LogiLed::KeyName::NONE) {
		return CallNextHookEx(0, code, wParam, lParam);
	}

	//echo effect registering here
	//TODO: capture ctrl_alt_delete and ctrl_shift_escape and force first two key ups
	//TODO: detect when computer put to sleep and resume
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
		curProfile->colorProfile.notifyEcho(eventKey);
		//echoLayer.RegisterKey(eventKey);
		//if (event->vkCode == 0x53 && wParam == WM_KEYDOWN) {
		//	//test send input
		//	INPUT ip;
		//	ip.type = INPUT_KEYBOARD;
		//	ip.ki.wScan = 0;
		//	ip.ki.time = 0;
		//	ip.ki.dwExtraInfo = 0;

		//	ip.ki.wVk = 0x41;
		//	ip.ki.dwFlags = 0;
		//	SendInput(1, &ip, sizeof(INPUT));
		//	Sleep(1000);
		//	ip.ki.dwFlags = KEYEVENTF_KEYUP;
		//	SendInput(1, &ip, sizeof(INPUT));
		//	//now need to trigger the keyup event from the trigger key
		//	ip.ki.wVk = 0x53;
		//	SendInput(1, &ip, sizeof(INPUT));
		//}

	}

	if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
		curProfile->colorProfile.notifyEchoStart(eventKey);
	}

	if (consumed) {
		return S_FALSE;
	}
	return CallNextHookEx(0, code, wParam, lParam);
}

void activeWindowChanged(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
	TCHAR buffer[MAX_PATH] = { 0 };
	DWORD dwProcId = 0;
	GetWindowThreadProcessId(GetForegroundWindow(), &dwProcId);
	char buf[256];
	sprintf_s(buf, "process id %d\n", dwProcId);
	OutputDebugStringA(buf);
	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcId);
	if (hProc) {
		sprintf_s(buf, "handle is %p\n", hProc);
		OutputDebugStringA(buf);

		GetModuleFileNameEx(hProc, NULL, buffer, MAX_PATH);
		OutputDebugString(L"Window changed to: ");
		OutputDebugString(buffer);
		OutputDebugString(L"\n");
		CloseHandle(hProc);
	}
}

BOOL initInstance(HINSTANCE hInstance, int nCmdShow)	//code enters from here
{
	hInst = hInstance;

	HRESULT hr = CoInitialize(NULL);	// Initalize COM
	if (SUCCEEDED(hr)) {
		hSingleInstanceMutex = CreateMutex(nullptr, FALSE, L"Global\\custom_keyboard");
		if (GetLastError() == ERROR_ALREADY_EXISTS) {
			MessageBox(NULL, L"CUSTOM KEYBOARD INSTANCE ALREADY RUNNING", L"ERROR", MB_OK);
			return FALSE;
		}

		//initialize Logitech LED SDK
		bool LedInitialized = LogiLedInitWithName("custom_keyboard");
		if (!LedInitialized) {
			MessageBox(NULL, L"CUSTOM KEYBOARD FAILED TO INITIALIZE LED SDK", L"ERROR", MB_OK);
			return FALSE;
		}
		LogiLedSetTargetDevice(LOGI_DEVICETYPE_PERKEY_RGB | LOGI_DEVICETYPE_PERKEY_RGB_ORD);

		hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, globalKeyboardHook, nullptr, 0); // hook for keyboard events
		//activeWindowHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL, activeWindowChanged, 0, 0, WINEVENT_OUTOFCONTEXT);
		//maybe add one on minimize as seen here: https://github.com/antonpup/Aurora/blob/ec557e1e2437028eda7adaab813cc24463d70bdd/Project-Aurora/Project-Aurora/Utils/ActiveProcessMonitor.cs#L37
		//TODO: eventually add an audio endpoint event hook:https://docs.microsoft.com/en-us/windows/win32/api/mmdeviceapi/nf-mmdeviceapi-immdeviceenumerator-registerendpointnotificationcallback?redirectedfrom=MSDN
		//TODO: create a global endpoint device to listen ^ to reduce the overhead on switching/detecting changes in devices
	}
	OutputDebugString(L"end of init\n");
	//CoUninitialize();
	return TRUE;
}
