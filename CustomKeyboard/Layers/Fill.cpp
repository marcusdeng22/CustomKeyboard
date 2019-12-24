#include "stdafx.h"
#include "Fill.h"

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

Fill::Fill(Condition c) {
	micVolume = nullptr;
	condition = c;
	if (condition == Condition::MicMuted) {
		updateDevice();
	}
	else {
		initialized = true;
	}
}

Fill::~Fill() {
	delete micVolume;
	//delete micDevicePtr;
	CoUninitialize();
}

void Fill::initialize() {
	color = Color(255, 255, 255);
}

void Fill::initialize(Color c) {
	color = c;	//default to white
}

void Fill::initialize(LogiLed::KeyName k, Color c) {
	initialize(c);
	RegisterKey(k);
}

void Fill::initialize(std::list<LogiLed::KeyName> l, Color c) {
	initialize(c);
	affectedKeys = l;
}

void Fill::paint() {
	for (LogiLed::KeyName k : affectedKeys) {
		//lookup the index in the bitmap
		size_t index = LogiLed::bitmapIndex.at(k);
		//fill the bitmap indices
		(*colorVector)[index] = color.b;
		(*colorVector)[index + 1] = color.g;
		(*colorVector)[index + 2] = color.r;
		(*colorVector)[index + 3] = color.a;
	}
}

void Fill::Tick() {
	if (!associated) {
		return;
	}

	if (condition == Condition::MicMuted && initialized) {
		BOOL muted;
		micVolume->GetMute(&muted);
		if (!muted) {
			paint();
		}	//else leave the bitmap as is
	}
	else {
		paint();
	}
}