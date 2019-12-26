#pragma once
#include <vector>
#include <windows.h>

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
private:
	MacroType mode;
	LPCSTR fileName;
	LPSTR args;
	std::vector<KeyState> keySeq;
	std::vector<int> delays;
	int defaultDelay;
public:
	Macro(LPCSTR f, LPSTR a = NULL);
	Macro(std::vector<KeyState> ks, std::vector<int> d = {}, int defDelay = 50);
	Macro(MacroType m);
	Macro(int delay);
	void exec();
};