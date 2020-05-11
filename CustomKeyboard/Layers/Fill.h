#pragma once

#include "Layer.h"
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include "../Safety.h"
//#include "EndpointNotifClient.h"

class Fill;

class EndpointNotifClient : public IMMNotificationClient {
private:
	LONG _cRef;
	Fill* fillLayer = nullptr;

public:
	EndpointNotifClient(Fill* fillLayer);
	~EndpointNotifClient() {}

	//IUnknown methods
	ULONG AddRef() { return InterlockedIncrement(&_cRef); }
	ULONG Release() {
		ULONG ulRef = InterlockedDecrement(&_cRef);
		if (0 == ulRef) {
			delete this;
		}
		return ulRef;
	}
	HRESULT QueryInterface(REFIID riid, VOID** ppvInterface) {
		if (IID_IUnknown == riid) {
			AddRef();
			*ppvInterface = (IUnknown*)this;
		}
		else if (__uuidof(IMMNotificationClient) == riid) {
			AddRef();
			*ppvInterface = (IMMNotificationClient*)this;
		}
		else {
			*ppvInterface = NULL;
			return E_NOINTERFACE;
		}
		return S_OK;
	}

	//IMMNotificationClient methods
	HRESULT OnDeviceAdded(LPCWSTR pwstrDeviceId) { return S_OK; }
	HRESULT OnDeviceRemoved(LPCWSTR pwstrDeviceId) { return S_OK; }
	HRESULT OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState) { return S_OK; }
	HRESULT OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key) { return S_OK; }
	HRESULT OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDeviceId);	//only this is implemented
};

class Fill : public Layer {
private:
	bool initialized;
	IMMDeviceEnumerator* de;
	IAudioEndpointVolume* micVolume;	//TODO: use a smart pointer
	EndpointNotifClient* outputNotif = NULL;

	std::vector<Color> colorData;
	int colorIndex = 0;

	void initMic();
	void initOutput();
	void detectDefault();
	void paint(std::vector<unsigned char>&);
	void doSetup(Condition cond, std::vector<Color> colorList);
public:
	Fill(Condition cond, std::vector<Color> colorList);
	Fill(std::list<LogiLed::KeyName> l, Condition cond, std::vector<Color> colorList);
	~Fill();

	//int getColorDataLength() { return colorData.size(); }
	void findAndSetIndex(LPCWSTR name);
	void setColorDataIndex(int i) { colorIndex = i % colorData.size(); }
	void tick(std::vector<unsigned char>&);
};