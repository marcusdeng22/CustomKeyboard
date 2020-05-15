#include "stdafx.h"
#include "Fill.h"
#include <string>

EndpointNotifClient::EndpointNotifClient(Fill* fillLayer) {
	_cRef = 1;
	//store the Fill layer reference
	this->fillLayer = fillLayer;
}

HRESULT EndpointNotifClient::OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDeviceId) {
	//only monitor render flows
	if (flow == eRender) {
		//lookup the index of the new default device, and set the color index of Fill to that
		OutputDebugString(L"Default render device changed!");
		OutputDebugStringW(pwstrDeviceId);
		OutputDebugString(L"\n");

		fillLayer->findAndSetIndex(pwstrDeviceId);
	}
	return S_OK;
}

//TODO: update the device when changing the default device
void Fill::initMic() {
	initialized = false;
	if (condition == Condition::MicMuted) {
		HRESULT hr = CoInitialize(NULL);
		if (SUCCEEDED(hr)) {
			//IMMDeviceEnumerator* de;
			hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&de);
			if (SUCCEEDED(hr)) {
				IMMDevice* micDevicePtr;
				hr = de->GetDefaultAudioEndpoint(EDataFlow::eCapture, ERole::eCommunications, &micDevicePtr);
				if (SUCCEEDED(hr)) {
					hr = micDevicePtr->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&micVolume);
					initialized = true;
				}
				//micDevicePtr->Release();
				SAFE_RELEASE(micDevicePtr);
			}
			//de->Release();
			SAFE_RELEASE(de);
		}
	}
}

void Fill::initOutput() {
	initialized = false;
	outputNotif = new EndpointNotifClient(this);
	if (condition == Condition::AudioOutput) {
		//register a callback on default device changed
		HRESULT hr = CoInitialize(NULL);
		if (SUCCEEDED(hr)) {
			//IMMDeviceEnumerator* de;
			hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&de);
			if (SUCCEEDED(hr)) {
				de->RegisterEndpointNotificationCallback(outputNotif);
				initialized = true;
				//find the default output device and its index, and set the color index to it
				detectDefault();
			}
		}
	}
}

void Fill::doSetup(Condition cond, std::vector<Color> colorList) {
	micVolume = nullptr;
	condition = cond;
	colorData = colorList;
	OutputDebugString(L"Setting up fill layer\n");
	if (condition == Condition::MicMuted) {
		OutputDebugString(L"Init mic\n");
		initMic();
	}
	else if (condition == Condition::AudioOutput) {
		OutputDebugString(L"Init audio output\n");
		initOutput();
	}
	else {
		initialized = true;
	}
}

Fill::Fill(Condition cond, std::vector<Color> colorList) {
	Fill::doSetup(cond, colorList);
	for (auto const& k : LogiLed::bitmapIndex) {
		affectedKeys.push_back(k.first);
	}
}

Fill::Fill(std::list<LogiLed::KeyName> l, Condition cond, std::vector<Color> colorList) {
	Fill::doSetup(cond, colorList);
	affectedKeys = l;
}

Fill::~Fill() {
	if (condition == Condition::MicMuted) {
		CoUninitialize();
	}
	else if (condition == Condition::AudioOutput) {
		de->UnregisterEndpointNotificationCallback(outputNotif);
		CoUninitialize();
	}
	if (de) {
		SAFE_RELEASE(de);
	}
}

void Fill::detectDefault() {
	//find the default output device, and set the color index to its index
	IMMDevice* defaultOutput;
	LPWSTR pstrDefault = NULL;	//to store the ID of the default
	HRESULT hr = de->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eMultimedia, &defaultOutput);
	if (SUCCEEDED(hr)) {
		hr = defaultOutput->GetId(&pstrDefault);
		if (SUCCEEDED(hr)) {
			findAndSetIndex(pstrDefault);
		}
	}
	SAFE_RELEASE(defaultOutput);
}

void Fill::findAndSetIndex(LPCWSTR name) {
	IMMDeviceCollection* outputDeviceList;
	de->EnumAudioEndpoints(EDataFlow::eRender, DEVICE_STATE_ACTIVE, &outputDeviceList);
	UINT count;
	HRESULT hr = outputDeviceList->GetCount(&count);
	if (SUCCEEDED(hr)) {
		//loop through all output devices, and once we find the default, grab the index
		IMMDevice* nextOutput;
		LPWSTR pstrNextId = NULL;
		for (ULONG i = 0; i < count; i++) {
			hr = outputDeviceList->Item(i, &nextOutput);
			if (SUCCEEDED(hr)) {
				hr = nextOutput->GetId(&pstrNextId);
				if (SUCCEEDED(hr)) {
					if (lstrcmpi(pstrNextId, name)) {	//same IDs
						//set the color index
						Fill::setColorDataIndex(i);
						break;
					}
				}
				SAFE_RELEASE(nextOutput);
			}
		}
	}
	SAFE_RELEASE(outputDeviceList);
}

void Fill::paint(std::vector<unsigned char>& colorVector) {
	keyLock.lock();
	for (LogiLed::KeyName k : affectedKeys) {
		//lookup the index in the bitmap
		size_t index = LogiLed::bitmapIndex.at(k);

		Color tcolor = colorData[colorIndex];
		(colorVector)[index] = tcolor.b;
		(colorVector)[index + 1] = tcolor.g;
		(colorVector)[index + 2] = tcolor.r;
		(colorVector)[index + 3] = tcolor.a;
	}
	keyLock.unlock();
}

void Fill::tick(std::vector<unsigned char>& colorVector) {
	if (condition == Condition::MicMuted && initialized) {
		BOOL muted;
		micVolume->GetMute(&muted);
		if (!muted) {
			Fill::paint(colorVector);
		}	//else leave the bitmap as is
	}
	else {
		Fill::paint(colorVector);
	}
}