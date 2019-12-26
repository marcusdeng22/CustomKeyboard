#include <stdafx.h>
#include "Macro.h"

Macro::Macro(LPCSTR f, LPSTR a) {
	mode = MacroType::launchApp;
	fileName = f;
	args = a;
	defaultDelay = NULL;
}

Macro::Macro(std::vector<KeyState> ks, std::vector<int> d, int defDelay) {
	mode = MacroType::keySeq;
	keySeq = ks;
	delays = d;
	defaultDelay = defDelay;
	fileName = NULL;
	args = NULL;
}

Macro::Macro(MacroType m) {
	if (m == MacroType::power || m == MacroType::audioCycle || m == MacroType::micToggle) {
		mode = m;
	}
	else {
		mode = MacroType::NONE;
	}
	fileName = NULL;
	args = NULL;
	defaultDelay = NULL;
}

Macro::Macro(int delay) {
	mode = MacroType::delay;
	fileName = NULL;
	args = NULL;
	defaultDelay = delay;
}

void Macro::exec() {
	switch (mode)
	{
	case MacroType::launchApp:
		//https://stackoverflow.com/questions/15435994/how-do-i-open-an-exe-from-another-c-exe
		STARTUPINFOA si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));
		CreateProcessA(fileName, args , NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		break;
	case MacroType::keySeq:
		INPUT ip;
			ip.type = INPUT_KEYBOARD;
			ip.ki.wScan = 0;
			ip.ki.time = 0;
			ip.ki.dwExtraInfo = 0;

			for (int i = 0; i < keySeq.size(); i++) {
				ip.ki.wVk = keySeq[i].keyCode;
				if (keySeq[i].isDown) {
					ip.ki.dwFlags = 0;
				}
				else {
					ip.ki.dwFlags = KEYEVENTF_KEYUP;
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
		break;
	case MacroType::delay:
		Sleep(defaultDelay);
		break;
	default:
		break;
	}
}