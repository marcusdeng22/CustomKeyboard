#include <stdafx.h>
#include "Macro.h"

Macro::Macro(std::string f, std::string a) {
	micVolume = nullptr;
	mode = MacroType::launchApp;
	fileName = f;
	args = a;
	defaultDelay = NULL;
}

Macro::Macro(std::vector<KeyState> ks, std::vector<int> d, int defDelay) {
	micVolume = nullptr;
	mode = MacroType::keySeq;
	keySeq = ks;
	delays = d;
	defaultDelay = defDelay;
}

Macro::Macro(MacroType m) {
	micVolume = nullptr;
	if (m == MacroType::audioCycle || m == MacroType::micToggle) {
		/*HRESULT hr = CoInitialize(NULL);
		if (SUCCEEDED(hr)) {*/
			mode = m;
		/*}
		else {
			mode = MacroType::NONE;
		}*/
	}
	else if (m == MacroType::power) {
		mode = m;
	}
	else {
		mode = MacroType::NONE;
	}
	defaultDelay = NULL;
}

Macro::Macro(int delay) {
	micVolume = nullptr;
	mode = MacroType::delay;
	defaultDelay = delay;
}

Macro::~Macro() {
	/*if (mode == MacroType::audioCycle || mode == MacroType::micToggle) {
		CoUninitialize();
	}*/
}

void Macro::exec() {
	switch (mode) {
	case MacroType::launchApp:
		OutputDebugString(L"launching macro\n");
		OutputDebugStringA(fileName.c_str());
		OutputDebugString(L"\n");
		OutputDebugStringA(args.c_str());
		OutputDebugStringW(L"\n");
		//https://stackoverflow.com/questions/15435994/how-do-i-open-an-exe-from-another-c-exe
		STARTUPINFOA si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		si.wShowWindow = SW_SHOW;	//try SW_SHOWNORMAL if this doesn't work
		si.dwFlags = STARTF_USESHOWWINDOW;
		ZeroMemory(&pi, sizeof(pi));
		CreateProcessA(fileName.c_str(), &(args[0]) , NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		break;
	case MacroType::keySeq:
		INPUT ip;
		ip.type = INPUT_KEYBOARD;
		ip.ki.wScan = 0;
		ip.ki.wVk = 0;
		ip.ki.dwFlags = 0;
		ip.ki.time = 0;
		ip.ki.dwExtraInfo = 0;

		for (int i = 0; i < keySeq.size(); i++) {
			if (keySeq[i].keyType == KeyType::VK) {
				ip.ki.wVk = keySeq[i].keyCode;
				if (ip.ki.wVk == VK_NUMPAD_ENTER) {
					ip.ki.wVk = VK_RETURN;
					ip.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
				}
				if (keySeq[i].isDown) {
					ip.ki.dwFlags = 0;
				}
				else {
					ip.ki.dwFlags = KEYEVENTF_KEYUP;
				}
			}
			else if (keySeq[i].keyType == KeyType::SC) {
				ip.ki.wScan = keySeq[i].keyCode;
				ip.ki.dwFlags = KEYEVENTF_SCANCODE;
				if (keySeq[i].isDown) {
					ip.ki.dwFlags |= 0;
				}
				else {
					ip.ki.dwFlags |= KEYEVENTF_KEYUP;
				}
			}
			SendInput(1, &ip, sizeof(INPUT));
			if (i != keySeq.size() - 1) {	//delay only if not last key
				if (i < delays.size()) {
					Sleep(delays[i]);
				}
				else {
					Sleep(defaultDelay);
				}
			}
		}
		break;
	case MacroType::power:
		//send the sleep command here: https://docs.microsoft.com/en-us/windows/win32/api/powrprof/nf-powrprof-setsuspendstate
		SetSuspendState(FALSE, NULL, FALSE);
		break;
	case MacroType::audioCycle:
		Macro::swapOutput();
		break;
	case MacroType::micToggle:
		Macro::toggleMute();
		break;
	case MacroType::delay:
		Sleep(defaultDelay);
		break;
	default:
		break;
	}
}


HRESULT Macro::setDefaultAudioPlaybackDevice(LPCWSTR devID) {
	IPolicyConfigVista* pPolicyConfig;

	HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient), NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID*)&pPolicyConfig);
	if (SUCCEEDED(hr))
	{
		hr = pPolicyConfig->SetDefaultEndpoint(devID, ERole::eConsole);
		//pPolicyConfig->Release();
		SAFE_RELEASE(pPolicyConfig);
	}
	return hr;
}

void Macro::toggleMute() {
	HRESULT hr = CoInitialize(NULL);
	if (SUCCEEDED(hr)) {
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
				//micDevicePtr->Release();
				SAFE_RELEASE(micDevicePtr);
			}
			//de->Release();
			SAFE_RELEASE(de);
		}
	}
	CoUninitialize();
}

void Macro::swapOutput() {
	HRESULT hr = CoInitialize(NULL);
	if (SUCCEEDED(hr)) {
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
												//nextOutput->Release();
												SAFE_RELEASE(nextOutput);
											}
										}
									}
									//nextOutput->Release();
									SAFE_RELEASE(nextOutput);
								}
							}
						}
					}
				}
				//defaultOutput->Release();
				SAFE_RELEASE(defaultOutput);
			}
			//outputDeviceList->Release();
			SAFE_RELEASE(outputDeviceList);
		}
		//de->Release();
		SAFE_RELEASE(de);
	}
	}
	CoUninitialize();
}
