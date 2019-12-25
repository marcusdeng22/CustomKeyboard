#include "stdafx.h"
#include "Fill.h"

//TODO: update the device when changing the default device
void Fill::updateDevice() {
	initialized = false;
	if (condition == Condition::MicMuted) {
		HRESULT hr = CoInitialize(NULL);
		if (SUCCEEDED(hr)) {
			IMMDeviceEnumerator* de;
			hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&de);
			if (SUCCEEDED(hr)) {
				IMMDevice* micDevicePtr;
				hr = de->GetDefaultAudioEndpoint(EDataFlow::eCapture, ERole::eCommunications, &micDevicePtr);
				if (SUCCEEDED(hr)) {
					hr = micDevicePtr->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&micVolume);
					initialized = true;
				}
				micDevicePtr->Release();
			}
			de->Release();
		}
	}
}

Fill::Fill(Condition cond, Color c) {
	OutputDebugString(L"first fill constructor\n");
	micVolume = nullptr;
	condition = cond;
	if (condition == Condition::MicMuted) {
		updateDevice();
	}
	else {
		initialized = true;
	}
	color = c;
	for (auto const& k : LogiLed::bitmapIndex) {	//change this to join from all keys?
		affectedKeys.push_back(k.first);
	}
}

Fill::Fill(LogiLed::KeyName k, Condition cond, Color c) {
	OutputDebugString(L"second fill constructor\n");
	micVolume = nullptr;
	condition = cond;
	if (condition == Condition::MicMuted) {
		updateDevice();
	}
	else {
		initialized = true;
	}
	color = c;
	RegisterKey(k);
}

Fill::Fill(std::list<LogiLed::KeyName> l, Condition cond, Color c) {
	OutputDebugString(L"third fill constructor\n");
	micVolume = nullptr;
	condition = cond;
	if (condition == Condition::MicMuted) {
		updateDevice();
	}
	else {
		initialized = true;
	}
	color = c;
	affectedKeys = l;
}


//Fill::Fill(Condition c) {
//	micVolume = nullptr;
//	condition = c;
//	if (condition == Condition::MicMuted) {
//		updateDevice();
//	}
//	else {
//		initialized = true;
//	}
//}

Fill::~Fill() {
	if (condition == Condition::MicMuted) {
		//delete micVolume;
		CoUninitialize();
	}
}

//void Fill::initialize() {
//	initialize(Color(255, 255, 255));	//default to white
//}
//
//void Fill::initialize(Color c) {
//	color = c;
//	for (auto const& k : LogiLed::bitmapIndex) {	//change this to join from all keys?
//		affectedKeys.push_back(k.first);
//	}
//}
//
//void Fill::initialize(LogiLed::KeyName k, Color c) {
//	color = c;
//	RegisterKey(k);
//}
//
//void Fill::initialize(std::list<LogiLed::KeyName> l, Color c) {
//	color = c;
//	affectedKeys = l;
//}

void Fill::paint(std::vector<unsigned char>& colorVector) {
	for (LogiLed::KeyName k : affectedKeys) {
		//lookup the index in the bitmap
		size_t index = LogiLed::bitmapIndex.at(k);
		//fill the bitmap indices
		//(*colorVector)[index] = color.b;
		//(*colorVector)[index + 1] = color.g;
		//(*colorVector)[index + 2] = color.r;
		//(*colorVector)[index + 3] = color.a;
		(colorVector)[index] = color.b;
		(colorVector)[index + 1] = color.g;
		(colorVector)[index + 2] = color.r;
		(colorVector)[index + 3] = color.a;
	}
}

void Fill::Tick(std::vector<unsigned char>& colorVector) {
	if (!associated) {
		return;
	}

	if (condition == Condition::MicMuted && initialized) {
		BOOL muted;
		micVolume->GetMute(&muted);
		if (!muted) {
			paint(colorVector);
		}	//else leave the bitmap as is
	}
	else {
		paint(colorVector);
	}
}