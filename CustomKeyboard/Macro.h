#pragma once
#include <vector>
#include <windows.h>
#include <mmdeviceapi.h>
#include <mmsystem.h>
#include <endpointvolume.h>
#include <PropIdl.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <PolicyConfig.h>
#include <string>
#include <powrprof.h>

enum class MacroType {
	launchApp,
	keySeq,
	power,
	audioCycle,
	micToggle,
	delay,
	NONE
};

struct KeyState {
	UINT keyCode;
	bool isDown;
	KeyState(UINT k, bool down) {
		keyCode = k;
		isDown = down;	//set to false to send key up
	}
};

class Macro {
protected:
	MacroType mode;
	std::string fileName;
	std::string args;
	std::vector<KeyState> keySeq;
	std::vector<int> delays;
	int defaultDelay;
	IAudioEndpointVolume* micVolume;	//TODO: use a smart pointer
	HRESULT setDefaultAudioPlaybackDevice(LPCWSTR devID);
	void toggleMute();
	void swapOutput();
public:
	Macro(std::string f, std::string a);
	Macro(std::vector<KeyState> ks, std::vector<int> d = {}, int defDelay = 50);
	Macro(MacroType m);
	Macro(int delay);
	~Macro();
	void exec();
};