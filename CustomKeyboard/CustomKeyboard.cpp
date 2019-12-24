// MicMuteToggle.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "windows.h"
#include "mmdeviceapi.h"
#include "mmsystem.h"
#include "endpointvolume.h"
#include "CustomKeyboard.h"
#include "PropIdl.h"
#include "Functiondiscoverykeys_devpkey.h"
#include "PolicyConfig.h"
#include "LogitechLEDLib.h"
#include "thread"
#include "math.h"
#include "map"
//#include "string"	//temp debug
#include "future"
#include "chrono"

#include "ProfileColor.h"
#include "Layers/Cycle.h"
#include "Layers/Echo.h"

// Configuration

namespace {
	const auto SWITCH_KEY = VK_F23;
	const auto TOGGLE_KEY = VK_F24;
	const auto DELTA = 2;
	struct ColorP {
		int r, g, b;
	};

	bool operator== (const Color& l, const Color& r) {
		return l.r == r.r && l.g == r.g && l.b == r.b;
	}

	bool operator!= (const Color& l, const Color& r) {
		return !(l == r);
	}
};

// Global Variables:
HINSTANCE hInst;
HANDLE hSingleInstanceMutex;
HHOOK hKeyboardHook;
IAudioEndpointVolume* micVolume;
//start with red
ColorP globalColor = { 100, 0, 0 };
//thread manager
//std::list<std::thread> threadManager;
//std::map<LogiLed::KeyName, std::thread> threadManager;
std::map<LogiLed::KeyName, std::future<int>> threadManager;

// Forward declarations of functions included in this code module:
BOOL InitInstance(HINSTANCE, int);
//void CycleColors();
//int EchoColor(DWORD code, ColorP targetColor);

Cycle cycleLayer;
Echo echoLayer;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	////start the default cycle
	//std::thread cycler(CycleColors);

	//create the default profile
	ProfileColor defaultProfile;
	//Cycle cycleLayer(defaultProfile.GetBitmap());
	//Cycle cycleLayer;
	/*wchar_t buf[256];
	wsprintf(buf, L"%x", defaultProfile.GetBitmap());
	OutputDebugString(L"Profile color bitmap addr from main: ");
	OutputDebugString(buf);
	OutputDebugString(L"\n");*/
	cycleLayer.initialize();
	echoLayer.initialize();
	defaultProfile.AddLayer(&cycleLayer);
	//Echo echoLayer;
	defaultProfile.AddLayer(&echoLayer);
	std::thread t(&ProfileColor::SendColor, defaultProfile);


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

	ReleaseMutex(hSingleInstanceMutex);
	OutputDebugString(L"released mutex");

	return (int)msg.wParam;
}

HRESULT SetDefaultAudioPlaybackDevice(LPCWSTR devID) {
	IPolicyConfigVista* pPolicyConfig;

	HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient), NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID*)&pPolicyConfig);
	if (SUCCEEDED(hr))
	{
		hr = pPolicyConfig->SetDefaultEndpoint(devID, ERole::eConsole);
		pPolicyConfig->Release();
	}
	return hr;
}

void ToggleMute() {
	HRESULT hr = CoInitialize(NULL);
	if (SUCCEEDED(hr)) {
		IMMDeviceEnumerator* de;
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&de);
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
	}
	CoUninitialize();
}

void SwapOutput() {
	HRESULT hr = CoInitialize(NULL);
	if (SUCCEEDED(hr)) {
		IMMDeviceEnumerator* de;
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&de);
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
														hr = SetDefaultAudioPlaybackDevice(pstrNextId);
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
	}
	CoUninitialize();
}

//int red = 100, green = 0, blue = 0;

LogiLed::KeyName convertFromScan(DWORD scanCode) {
	//return static_cast<LogiLed::KeyName>(scanCode);
	return LogiLed::KeyName(scanCode);
}

LRESULT CALLBACK GlobalKeyboardHook(int code, WPARAM wParam, LPARAM lParam) {
	/*if (code != 0 || lParam == 0 || (wParam != WM_KEYDOWN && wParam != WM_KEYUP)) {
		return CallNextHookEx(0, code, wParam, lParam);
	}*/

	KBDLLHOOKSTRUCT* event = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
	if (event->vkCode == TOGGLE_KEY) {
		if (wParam == WM_KEYUP) {
			ToggleMute();
		}
		//return S_FALSE;
		//return CallNextHookEx(0, code, wParam, lParam);
	}

	if (event->vkCode == SWITCH_KEY) {
		if (wParam == WM_KEYUP) {
			//switch audio output here
			SwapOutput();
		}
		//return S_FALSE;
		//return CallNextHookEx(0, code, wParam, lParam);
	}

	LogiLed::KeyName eventKey;
	if (event->vkCode == VK_RETURN && event->flags & LLKHF_EXTENDED) {
		eventKey = LogiLed::KeyName::NUM_ENTER;
	}
	else {
		eventKey = LogiLed::scanCodeToKeyName.at(event->vkCode);
	}
	if (eventKey == LogiLed::KeyName::NONE) {
		return CallNextHookEx(0, code, wParam, lParam);
	}

	//echo effect here
	//LogiLedSaveCurrentLighting();	//?? save and restore?
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
		//LogiLedPulseSingleKey(convertFromScan(event->scanCode), red, green, blue, 100, 100, 100, 1000, false);
		//ColorP targetColor = {100, 100, 100};	//white
		//EchoColor(event->scanCode, targetColor);
		//Sleep(3000);
		//std::thread echoThread(EchoColor, event->scanCode, targetColor);
		//threadManager.emplace_front(std::thread(EchoColor, event->scanCode, targetColor));
		//threadManager.emplace(convertFromScan(event->scanCode), std::thread(EchoColor, event->scanCode, targetColor));
		//threadManager.emplace(convertFromScan(event->scanCode), std::async(EchoColor, event->scanCode, targetColor));

		//if (event->vkCode == VK_RETURN) {
		//	char buf[256];
		//	OutputDebugString(L"detected enter key\n");
		//	sprintf_s(buf, "scan code: %d\nflags: %d\n", event->scanCode, event->flags);
		//	OutputDebugStringA(buf);
		//	if (event->flags & LLKHF_EXTENDED) {
		//		OutputDebugString(L"extended\n");	//this is the keypad return
		//	}
		//	else {
		//		OutputDebugString(L"not extended\n");
		//	}
		//}

		//if (event->vkCode == VK_MENU || event->vkCode == VK_LMENU || event->vkCode == VK_RMENU) {
		//	OutputDebugString(L"vk detected alt down\n");
		//}
		//
		//char buf[256];
		//sprintf_s(buf, "REGISTERING KEY %x\n", event->vkCode);
		//OutputDebugStringA(buf);
		////echoLayer.RegisterKey(convertFromScan(event->scanCode));
		//echoLayer.RegisterKey(LogiLed::scanCodeToKeyName.at(event->vkCode));

		/*if (event->vkCode == VK_RETURN && event->flags & LLKHF_EXTENDED) {
			echoLayer.RegisterKey(LogiLed::scanCodeToKeyName.at(VK_NUMPAD_ENTER));
		}
		else {
			echoLayer.RegisterKey(LogiLed::scanCodeToKeyName.at(event->vkCode));
		}

		char buf[256];
		sprintf_s(buf, "key pressed: %x\n", event->vkCode);
		OutputDebugStringA(buf);*/

		//if(event->vkCode == VK_RETURN && wParam == WM_KEYDOWN) {
		//	//test send input
		//	INPUT ip;
		//	ip.type = INPUT_KEYBOARD;
		//	ip.ki.wScan = 0;
		//	ip.ki.time = 0;
		//	ip.ki.dwExtraInfo = 0;

		//	ip.ki.wVk = VK_SLEEP;
		//	ip.ki.dwFlags = 0;
		//	SendInput(1, &ip, sizeof(INPUT));
		//	Sleep(100);
		//	ip.ki.dwFlags = KEYEVENTF_KEYUP;
		//	SendInput(1, &ip, sizeof(INPUT));
		//}

		echoLayer.RegisterKey(eventKey);
	}

	if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
		//if (event->vkCode == VK_RETURN && event->flags & LLKHF_EXTENDED) {
		//	echoLayer.startKey(LogiLed::scanCodeToKeyName.at(VK_NUMPAD_ENTER));
		//}
		//else {
		//	echoLayer.startKey(LogiLed::scanCodeToKeyName.at(event->vkCode));
		//}
		////echoLayer.startKey(LogiLed::scanCodeToKeyName.at(event->vkCode));
		echoLayer.startKey(eventKey);
	}

	/*if (GetAsyncKeyState(VK_MENU) & 0x8000) {
		char buf[256];
		sprintf_s(buf, "DETECTED ALT PRESS %x\n", event->vkCode);
		OutputDebugStringA(buf);
	}*/


	return CallNextHookEx(0, code, wParam, lParam);
}

//using namespace std::chrono_literals;
//
//void CycleColors() {
//	int cycle = 0;
//	while (true) {
//		//LogiLedSetLighting(globalColor.r, globalColor.g, globalColor.b);
//		//std::map<LogiLed::KeyName, std::future<int>>::iterator it = threadManager.begin();
//		//for (LogiLed::KeyName k : LogiLed::allKeys) {
//		//	if (threadManager.find(k) == threadManager.end()) {	//in map of threads in flight
//		//		LogiLedSetLightingForKeyWithKeyName(k, globalColor.r, globalColor.g, globalColor.b);
//		//	}
//		//	else {
//		//		//TODO: check if thread has finished!
//		//		std::future_status status = threadManager.at(k).wait_for(0ms);
//		//		if (status == std::future_status::ready) {
//		//			//remove from the map
//		//			threadManager.erase(k);
//		//			LogiLedSetLightingForKeyWithKeyName(k, globalColor.r, globalColor.g, globalColor.b);
//		//		}
//		//		//LogiLedSetLightingForKeyWithKeyName(k, globalColor.r, globalColor.g, globalColor.b);
//		//	}
//		//}
//		unsigned char bitmap[LOGI_LED_BITMAP_SIZE] = { 0 };
//		/*for (auto k : LogiLed::allKeys) {
//			auto i = LogiLed::bitmapIndex.at(k);*/
//		for (auto& k : LogiLed::bitmapIndex) {
//			auto i = k.second;
//			bitmap[i] = globalColor.r;		//these are on a scale of 0-255, unlike the other functions
//			bitmap[i + 1] = globalColor.g;
//			bitmap[i + 2] = globalColor.b;
//			bitmap[i + 3] = 255;	//set to visible
//		}
//		LogiLedSetLightingFromBitmap(bitmap);
//		LogiLedSetLightingForTargetZone(LogiLed::Mouse, 0, globalColor.r, globalColor.g, globalColor.b);	//dpi indicators
//		LogiLedSetLightingForTargetZone(LogiLed::Mouse, 1, globalColor.r, globalColor.g, globalColor.b);	//logo
//		if (cycle == 0) {
//			if (globalColor.g == 100) {	//increase green
//				cycle += 1;
//			}
//			else {
//				globalColor.g += DELTA;
//			}
//		}
//		else if (cycle == 1) {	//decrease red
//			if (globalColor.r == 0) {
//				cycle += 1;
//			}
//			else {
//				globalColor.r -= DELTA;
//			}
//		}
//		else if (cycle == 2) {	//increase blue
//			if (globalColor.b == 100) {
//				cycle += 1;
//			}
//			else {
//				globalColor.b += DELTA;
//			}
//		}
//		else if (cycle == 3) {	//decrease green
//			if (globalColor.g == 0) {
//				cycle += 1;
//			}
//			else {
//				globalColor.g -= DELTA;
//			}
//		}
//		else if (cycle == 4) {	//increase red
//			if (globalColor.r == 100) {
//				cycle += 1;
//			}
//			else {
//				globalColor.r += DELTA;
//			}
//		}
//		else {	//decrease blue
//			if (globalColor.b == 0) {
//				cycle = 0;
//			}
//			else {
//				globalColor.b -= DELTA;
//			}
//		}
//		//delay
//		Sleep(20);
//	}
//}
//
////set to targetColor and resolve to globalColor
//int EchoColor(DWORD code, Color targetColor) {
//	//set lighting on key down
//	LogiLedSetLightingForKeyWithScanCode(code, targetColor.r, targetColor.g, targetColor.b);
//	double alpha = 1.0;
//	double decay = 0.05;
//	do {
//		alpha -= decay;
//		if (alpha < 0) {
//			break;
//		}
//		targetColor.r = (int) round(((1 - alpha) * globalColor.r) + (alpha * targetColor.r));
//		targetColor.g = (int) round(((1 - alpha) * globalColor.g) + (alpha * targetColor.g));
//		targetColor.b = (int) round(((1 - alpha) * globalColor.b) + (alpha * targetColor.b));
//		LogiLedSetLightingForKeyWithScanCode(code, targetColor.r, targetColor.g, targetColor.b);
//		Sleep(100);
//	} while (alpha > 0);
//	return 0;
//}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)	//code enters from here
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
		//LogiLedSetTargetDevice(LOGI_DEVICETYPE_PERKEY_RGB);
		LogiLedSetTargetDevice(LOGI_DEVICETYPE_PERKEY_RGB | LOGI_DEVICETYPE_PERKEY_RGB_ORD);
		//test
		//LogiLedSetLighting(100, 0, 0);	//sets all
		//LogiLedSetLightingForTargetZone(LogiLed::Mouse, 0, 100, 0, 0);	//dpi indicators
		//LogiLedSetLightingForTargetZone(LogiLed::Mouse, 1, 100, 100, 0);	//logo

		hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, GlobalKeyboardHook, nullptr, 0); // hook for keyboard events
	}
	OutputDebugString(L"end of init\n");
	CoUninitialize();
	return TRUE;
}
