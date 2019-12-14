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

// Configuration

namespace {
	const auto SWITCH_KEY = VK_F23;
	const auto TOGGLE_KEY = VK_F24;
};

// Global Variables:
HINSTANCE hInst;
HANDLE hSingleInstanceMutex;
HHOOK hKeyboardHook;
IAudioEndpointVolume* micVolume;

// Forward declarations of functions included in this code module:
BOOL                InitInstance(HINSTANCE, int);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

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

	return (int)msg.wParam;
}

HRESULT SetDefaultAudioPlaybackDevice(LPCWSTR devID)
{
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

LRESULT CALLBACK GlobalKeyboardHook(
	int code,
	WPARAM wParam,
	LPARAM lParam
) {
	if (code != 0 || lParam == 0 || (wParam != WM_KEYDOWN && wParam != WM_KEYUP)) {
		return CallNextHookEx(0, code, wParam, lParam);
	}

	KBDLLHOOKSTRUCT* event = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
	if (event->vkCode == TOGGLE_KEY) {
		if (wParam == WM_KEYDOWN) {
			//SetMute(MuteBehavior::TOGGLE, /* button up = */ false);
			ToggleMute();
		}
		return S_FALSE;
	}

	if (event->vkCode == SWITCH_KEY) {
		if (wParam == WM_KEYDOWN) {
			//switch audio output here
			SwapOutput();
		}
		return S_FALSE;
	}

	return CallNextHookEx(0, code, wParam, lParam);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)	//code enters from here
{
	hInst = hInstance;

	HRESULT hr = CoInitialize(NULL);	// Initalize COM
	if (SUCCEEDED(hr)) {
		hSingleInstanceMutex = CreateMutex(nullptr, FALSE, L"Global\\micmutetoggle");
		if (GetLastError() == ERROR_ALREADY_EXISTS) {
			return FALSE;
		}

		hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, GlobalKeyboardHook, nullptr, 0); // hook for keyboard events
	}
	CoUninitialize();
	return TRUE;
}
