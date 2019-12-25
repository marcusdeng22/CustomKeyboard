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
	micVolume = nullptr;
	condition = cond;
	if (condition == Condition::MicMuted) {
		updateDevice();
	}
	else {
		initialized = true;
	}
	color = c;
	registerKey(k);
}

Fill::Fill(std::list<LogiLed::KeyName> l, Condition cond, Color c) {
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

Fill::~Fill() {
	if (condition == Condition::MicMuted) {
		CoUninitialize();
	}
}

void Fill::paint(std::vector<unsigned char>& colorVector) {
	for (LogiLed::KeyName k : affectedKeys) {
		//lookup the index in the bitmap
		size_t index = LogiLed::bitmapIndex.at(k);
		(colorVector)[index] = color.b;
		(colorVector)[index + 1] = color.g;
		(colorVector)[index + 2] = color.r;
		(colorVector)[index + 3] = color.a;
	}
}

void Fill::tick(std::vector<unsigned char>& colorVector) {
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