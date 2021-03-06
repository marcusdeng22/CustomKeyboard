#pragma once

#include <windows.h>
#include <mmdeviceapi.h>
#include <mmsystem.h>
#include <endpointvolume.h>
#include <PropIdl.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <PolicyConfig.h>
#include <thread>
#include <math.h>
#include <map>
#include <Psapi.h>
#include <pathcch.h>
#include <fstream>
#include <memory>
#include <list>
#include <mutex>
#include <powerbase.h>
#include <ctime>
#include <Windows.h>
#include <shellapi.h>
#include <strsafe.h>


#include "resource.h"
#include "LogitechLEDLib.h"
#include "lib/json.hpp"
#include "Profile.h"
#include "Layers/Cycle.h"
#include "Layers/Echo.h"
#include "Layers/Fill.h"

static const std::map<std::string, Condition> toCondition({
	{"None", Condition::None},
	{"MicMuted", Condition::MicMuted},
	{"AudioOutput", Condition::AudioOutput}
});

Condition convertCondition(std::string& s) {
	auto i = toCondition.find(s);
	if (i != toCondition.end()) {
		return i->second;
	}
	return Condition::None;
}

static const std::map<std::string, LogiLed::KeyName> toKey({
	{"ESC", LogiLed::KeyName::ESC},
	{"F1", LogiLed::KeyName::F1},
	{"F2", LogiLed::KeyName::F2},
	{"F3", LogiLed::KeyName::F3},
	{"F4", LogiLed::KeyName::F4},
	{"F5", LogiLed::KeyName::F5},
	{"F6", LogiLed::KeyName::F6},
	{"F7", LogiLed::KeyName::F7},
	{"F8", LogiLed::KeyName::F8},
	{"F9", LogiLed::KeyName::F9},
	{"F10", LogiLed::KeyName::F10},
	{"F11", LogiLed::KeyName::F11},
	{"F12", LogiLed::KeyName::F12},
	{"PRINT_SCREEN", LogiLed::KeyName::PRINT_SCREEN},
	{"SCROLL_LOCK", LogiLed::KeyName::SCROLL_LOCK},
	{"PAUSE_BREAK", LogiLed::KeyName::PAUSE_BREAK},
	{"TILDE", LogiLed::KeyName::TILDE},
	{"ONE", LogiLed::KeyName::ONE},
	{"TWO", LogiLed::KeyName::TWO},
	{"THREE", LogiLed::KeyName::THREE},
	{"FOUR", LogiLed::KeyName::FOUR},
	{"FIVE", LogiLed::KeyName::FIVE},
	{"SIX", LogiLed::KeyName::SIX},
	{"SEVEN", LogiLed::KeyName::SEVEN},
	{"EIGHT", LogiLed::KeyName::EIGHT},
	{"NINE", LogiLed::KeyName::NINE},
	{"ZERO", LogiLed::KeyName::ZERO},
	{"MINUS", LogiLed::KeyName::MINUS},
	{"EQUALS", LogiLed::KeyName::EQUALS},
	{"BACKSPACE", LogiLed::KeyName::BACKSPACE},
	{"INSERT", LogiLed::KeyName::INSERT},
	{"HOME", LogiLed::KeyName::HOME},
	{"PAGE_UP", LogiLed::KeyName::PAGE_UP},
	{"NUM_LOCK", LogiLed::KeyName::NUM_LOCK},
	{"NUM_SLASH", LogiLed::KeyName::NUM_SLASH},
	{"NUM_ASTERISK", LogiLed::KeyName::NUM_ASTERISK},
	{"NUM_MINUS", LogiLed::KeyName::NUM_MINUS},
	{"TAB", LogiLed::KeyName::TAB},
	{"Q", LogiLed::KeyName::Q},
	{"W", LogiLed::KeyName::W},
	{"E", LogiLed::KeyName::E},
	{"R", LogiLed::KeyName::R},
	{"T", LogiLed::KeyName::T},
	{"Y", LogiLed::KeyName::Y},
	{"U", LogiLed::KeyName::U},
	{"I", LogiLed::KeyName::I},
	{"O", LogiLed::KeyName::O},
	{"P", LogiLed::KeyName::P},
	{"OPEN_BRACKET", LogiLed::KeyName::OPEN_BRACKET},
	{"CLOSE_BRACKET", LogiLed::KeyName::CLOSE_BRACKET},
	{"BACKSLASH", LogiLed::KeyName::BACKSLASH},
	{"KEYBOARD_DELETE", LogiLed::KeyName::KEYBOARD_DELETE},
	{"END", LogiLed::KeyName::END},
	{"PAGE_DOWN", LogiLed::KeyName::PAGE_DOWN},
	{"NUM_SEVEN", LogiLed::KeyName::NUM_SEVEN},
	{"NUM_EIGHT", LogiLed::KeyName::NUM_EIGHT},
	{"NUM_NINE", LogiLed::KeyName::NUM_NINE},
	{"NUM_PLUS", LogiLed::KeyName::NUM_PLUS},
	{"CAPS_LOCK", LogiLed::KeyName::CAPS_LOCK},
	{"A", LogiLed::KeyName::A},
	{"S", LogiLed::KeyName::S},
	{"D", LogiLed::KeyName::D},
	{"F", LogiLed::KeyName::F},
	{"G", LogiLed::KeyName::G},
	{"H", LogiLed::KeyName::H},
	{"J", LogiLed::KeyName::J},
	{"K", LogiLed::KeyName::K},
	{"L", LogiLed::KeyName::L},
	{"SEMICOLON", LogiLed::KeyName::SEMICOLON},
	{"APOSTROPHE", LogiLed::KeyName::APOSTROPHE},
	{"ENTER", LogiLed::KeyName::ENTER},
	{"NUM_FOUR", LogiLed::KeyName::NUM_FOUR},
	{"NUM_FIVE", LogiLed::KeyName::NUM_FIVE},
	{"NUM_SIX", LogiLed::KeyName::NUM_SIX},
	{"LEFT_SHIFT", LogiLed::KeyName::LEFT_SHIFT},
	{"Z", LogiLed::KeyName::Z},
	{"X", LogiLed::KeyName::X},
	{"C", LogiLed::KeyName::C},
	{"V", LogiLed::KeyName::V},
	{"B", LogiLed::KeyName::B},
	{"N", LogiLed::KeyName::N},
	{"M", LogiLed::KeyName::M},
	{"COMMA", LogiLed::KeyName::COMMA},
	{"PERIOD", LogiLed::KeyName::PERIOD},
	{"FORWARD_SLASH", LogiLed::KeyName::FORWARD_SLASH},
	{"RIGHT_SHIFT", LogiLed::KeyName::RIGHT_SHIFT},
	{"ARROW_UP", LogiLed::KeyName::ARROW_UP},
	{"NUM_ONE", LogiLed::KeyName::NUM_ONE},
	{"NUM_TWO", LogiLed::KeyName::NUM_TWO},
	{"NUM_THREE", LogiLed::KeyName::NUM_THREE},
	{"NUM_ENTER", LogiLed::KeyName::NUM_ENTER},
	{"LEFT_CONTROL", LogiLed::KeyName::LEFT_CONTROL},
	{"LEFT_WINDOWS", LogiLed::KeyName::LEFT_WINDOWS},
	{"LEFT_ALT", LogiLed::KeyName::LEFT_ALT},
	{"SPACE", LogiLed::KeyName::SPACE},
	{"RIGHT_ALT", LogiLed::KeyName::RIGHT_ALT},
	{"RIGHT_WINDOWS", LogiLed::KeyName::RIGHT_WINDOWS},
	{"APPLICATION_SELECT", LogiLed::KeyName::APPLICATION_SELECT},
	{"RIGHT_CONTROL", LogiLed::KeyName::RIGHT_CONTROL},
	{"ARROW_LEFT", LogiLed::KeyName::ARROW_LEFT},
	{"ARROW_DOWN", LogiLed::KeyName::ARROW_DOWN},
	{"ARROW_RIGHT", LogiLed::KeyName::ARROW_RIGHT},
	{"NUM_ZERO", LogiLed::KeyName::NUM_ZERO},
	{"NUM_PERIOD", LogiLed::KeyName::NUM_PERIOD},
	{"G_1", LogiLed::KeyName::G_1},
	{"G_2", LogiLed::KeyName::G_2},
	{"G_3", LogiLed::KeyName::G_3},
	{"G_4", LogiLed::KeyName::G_4},
	{"G_5", LogiLed::KeyName::G_5},
	{"G_6", LogiLed::KeyName::G_6},
	{"G_7", LogiLed::KeyName::G_7},
	{"G_8", LogiLed::KeyName::G_8},
	{"G_9", LogiLed::KeyName::G_9},
	{"G_LOGO", LogiLed::KeyName::G_LOGO},
	{"G_BADGE", LogiLed::KeyName::G_BADGE},
	{"MOUSE_Z0", LogiLed::KeyName::MOUSE_Z0},
	{"MOUSE_Z1", LogiLed::KeyName::MOUSE_Z1}
});

//looks up the LogiLed::KeyName equivalent of a string
DWORD convertKey(std::string& s) {
	auto i = toKey.find(s);
	if (i != toKey.end()) {
		return (DWORD)i->second;
	}
	return (DWORD)LogiLed::KeyName::NONE;
}

//converts a vector of strings to a list of equivalent LogiLed::KeyName
std::list<LogiLed::KeyName> convertList(std::vector<std::string>& source) {
	std::list<LogiLed::KeyName> ret;
	for (auto s : source) {
		auto i = toKey.find(s);
		if (i != toKey.end()) {
			ret.push_back(i->second);
		}
	}
	return ret;
}

static const std::map<std::string, MacroType> toMacroType({
	{"launchApp", MacroType::launchApp},
	{"keySeq", MacroType::keySeq},
	{"power", MacroType::power},
	{"audioCycle", MacroType::audioCycle},
	{"micToggle", MacroType::micToggle},
	{"delay", MacroType::delay},
	{"NONE", MacroType::NONE},
});

MacroType convertMacroType(std::string& s) {
	auto i = toMacroType.find(s);
	if (i != toMacroType.end()) {
		return i->second;
	}
	return MacroType::NONE;
}

static const std::map<std::string, DWORD> toVKCode({
	{"ESC", VK_ESCAPE},
	{"F1", VK_F1},
	{"F2", VK_F2},
	{"F3", VK_F3},
	{"F4", VK_F4},
	{"F5", VK_F5},
	{"F6", VK_F6},
	{"F7", VK_F7},
	{"F8", VK_F8},
	{"F9", VK_F9},
	{"F10", VK_F10},
	{"F11", VK_F11},
	{"F12", VK_F12},
	{"PRINT_SCREEN", VK_SNAPSHOT},
	{"SCROLL_LOCK", VK_SCROLL},
	{"PAUSE", VK_PAUSE},
	{"TILDE", VK_OEM_3},
	{"0", 0x31},
	{"1", 0x32},
	{"2", 0x33},
	{"3", 0x34},
	{"4", 0x35},
	{"5", 0x36},
	{"6", 0x37},
	{"7", 0x38},
	{"8", 0x39},
	{"9", 0x30},
	{"-", VK_OEM_MINUS},			//minus sign on keyboard
	{"=", VK_OEM_PLUS},				//plus sign on keyboard
	{"BACK", VK_BACK},			//backspace
	{"INSERT", VK_INSERT},
	{"HOME", VK_HOME},
	{"PAGE_UP", VK_PRIOR},			//page up
	{"NUM_LOCK", VK_NUMLOCK},
	{"NUM_SLASH", VK_DIVIDE},
	{"NUM_ASTERISK", VK_MULTIPLY},
	{"NUM_MINUS", VK_SUBTRACT},
	{"TAB", VK_TAB},
	{"Q", 0x51},
	{"W", 0x57},
	{"E", 0x45},
	{"R", 0x52},
	{"T", 0x54},
	{"Y", 0x59},
	{"U", 0x55},
	{"I", 0x49},
	{"O", 0x4f},
	{"P", 0x50},
	{"OPEN_BRACKET", VK_OEM_4},
	{"CLOSE_BRACKET", VK_OEM_6},
	{"BACKSLASH", VK_OEM_5},
	{"KEYBOARD_DELETE", VK_DELETE},
	{"END", VK_END},
	{"PAGE_DOWN", VK_NEXT},
	{"NUM_SEVEN", VK_NUMPAD7},
	{"NUM_EIGHT", VK_NUMPAD8},
	{"NUM_NINE", VK_NUMPAD9},
	{"NUM_PLUS", VK_ADD},
	{"CAPS_LOCK", VK_CAPITAL},		//caps lock
	{"A", 0x41},
	{"S", 0x53},
	{"D", 0x44},
	{"F", 0x46},
	{"G", 0x47},
	{"H", 0x48},
	{"J", 0x4a},
	{"K", 0x4b},
	{"L", 0x4c},
	{"SEMICOLON", VK_OEM_1},
	{"APOSTROPHE", VK_OEM_7},
	{"ENTER", VK_RETURN},			//keyboard enter
	{"NUM_ENTER", VK_NUMPAD_ENTER},	//numpad enter
	{"NUM_FOUR", VK_NUMPAD4},
	{"NUM_FIVE", VK_NUMPAD5},
	{"NUM_SIX", VK_NUMPAD6},
	{"LEFT_SHIFT", VK_LSHIFT},
	{"Z", 0x5a},
	{"X", 0x58},
	{"C", 0x43},
	{"V", 0x56},
	{"B", 0x42},
	{"N", 0x4e},
	{"M", 0x4d},
	{"COMMA", VK_OEM_COMMA},
	{"PERIOD", VK_OEM_PERIOD},
	{"FORWARD_SLASH", VK_OEM_2},
	{"RIGHT_SHIFT", VK_RSHIFT},
	{"ARROW_UP", VK_UP},
	{"NUM_ONE", VK_NUMPAD1},
	{"NUM_TWO", VK_NUMPAD2},
	{"NUM_THREE", VK_NUMPAD3},
	//{"VK_NUMPAD_ENTER", VK_NUMPAD_ENTER},	//this was custom defined
	{"LEFT_CONTROL", VK_LCONTROL},
	{"LEFT_WINDOWS", VK_LWIN},
	{"LEFT_ALT", VK_LMENU},
	{"SPACE", VK_SPACE},
	{"RIGHT_ALT", VK_RMENU},
	{"RIGHT_WINDOWS", VK_RWIN},
	{"APPLICATION_SELECT", VK_APPS},
	{"RIGHT_CONTORL", VK_RCONTROL},
	{"ARROW_LEFT", VK_LEFT},
	{"ARROW_DOWN", VK_DOWN},
	{"ARROW_RIGHT", VK_RIGHT},
	{"NUM_ZERO", VK_NUMPAD0},
	{"NUM_PERIOD", VK_DECIMAL},
	//NO DEFINED LOGI KEYS PAST HERE
	{"LBUTTON", VK_LBUTTON},
	{"RBUTTON", VK_RBUTTON},
	{"CANCEL", VK_CANCEL},
	{"MBUTTON", VK_MBUTTON},
	{"XBUTTON1", VK_XBUTTON1},
	{"XBUTTON2", VK_XBUTTON2},
	{"CLEAR", VK_CLEAR},
	{"KANA", VK_KANA},
	{"HANGEUL", VK_HANGEUL},
	{"HANGUL", VK_HANGUL},
	{"JUNJA", VK_JUNJA},
	{"FINAL", VK_FINAL},
	{"HANJA", VK_HANJA},
	{"KANJI", VK_KANJI},
	{"CONVERT", VK_CONVERT},
	{"NONCONVERT", VK_NONCONVERT},
	{"ACCEPT", VK_ACCEPT},
	{"MODECHANGE", VK_MODECHANGE},
	{"HELP", VK_HELP},
	{"APPS", VK_APPS},
	{"SLEEP", VK_SLEEP},
	{"SEPARATOR", VK_SEPARATOR},	//?
	{"F13", VK_F13},
	{"F14", VK_F14},
	{"F15", VK_F15},
	{"F16", VK_F16},
	{"F17", VK_F17},
	{"F18", VK_F18},
	{"F19", VK_F19},
	{"F20", VK_F20},
	{"F21", VK_F21},
	{"F22", VK_F22},
	{"F23", VK_F23},
	{"F24", VK_F24},
	{"BROWSER_BACK", VK_BROWSER_BACK},
	{"BROWSER_FORWARD", VK_BROWSER_FORWARD},
	{"BROWSER_REFRESH", VK_BROWSER_REFRESH},
	{"BROWSER_STOP", VK_BROWSER_STOP},
	{"BROWSER_SEARCH", VK_BROWSER_SEARCH},
	{"BROWSER_FAVORITES", VK_BROWSER_FAVORITES},
	{"BROWSER_HOME", VK_BROWSER_HOME},
	{"VOLUME_MUTE", VK_VOLUME_MUTE},
	{"VOLUME_DOWN", VK_VOLUME_DOWN},
	{"VOLUME_UP", VK_VOLUME_UP},
	{"MEDIA_NEXT_TRACK", VK_MEDIA_NEXT_TRACK},
	{"MEDIA_PREV_TRACK", VK_MEDIA_PREV_TRACK},
	{"MEDIA_STOP", VK_MEDIA_STOP},
	{"MEDIA_PLAY_PAUSE", VK_MEDIA_PLAY_PAUSE},
	{"LAUNCH_MAIL", VK_LAUNCH_MAIL},
	{"LAUNCH_MEDIA_SELECT", VK_LAUNCH_MEDIA_SELECT},
	{"LAUNCH_APP1", VK_LAUNCH_APP1},
	{"LAUNCH_APP2", VK_LAUNCH_APP2},
	{"OEM_8", VK_OEM_8},
	{"OEM_102", VK_OEM_102},
	{"PROCESSKEY", VK_PROCESSKEY},
	{"PACKET", VK_PACKET},
	{"ATTN", VK_ATTN},
	{"CRSEL", VK_CRSEL},
	{"EXSEL", VK_EXSEL},
	{"EREOF", VK_EREOF},
	{"PLAY", VK_PLAY},
	{"ZOOM", VK_ZOOM},
	{"NONAME", VK_NONAME},
	{"PA1", VK_PA1},
	{"OEM_CLEAR", VK_OEM_CLEAR}
});

//from: https://docs.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-6.0/aa299374(v=vs.60)?redirectedfrom=MSDN
//and from AHK inspection
//susceptible to numlock
static const std::map<std::string, UINT> toScanCode({
	{"ESC", 0x01},
	{"F1", 0x3b},
	{"F2", 0x3c},
	{"F3", 0x3d},
	{"F4", 0x3e},
	{"F5", 0x3f},
	{"F6", 0x40},
	{"F7", 0x41},
	{"F8", 0x42},
	{"F9", 0x43},
	{"F10", 0x44},
	{"F11", 0x57},
	{"F12", 0x58},
	{"PRINT_SCREEN", 0x37},		//0x37 bad mapping, but is what Microsoft says -> numMult; AHK code 0x137 has same result
	{"SCROLL_LOCK", 0x46},
	//{"PAUSE", 0x45},			//no scan code from Microsoft; AHK code is NUM_LOCK
	{"TILDE", 0x29},
	{"0", 0x0b},
	{"1", 0x02},
	{"2", 0x03},
	{"3", 0x04},
	{"4", 0x05},
	{"5", 0x06},
	{"6", 0x07},
	{"7", 0x08},
	{"8", 0x09},
	{"9", 0x0a},
	{"-", 0x0c},				//minus sign on keyboard
	{"=", 0x0d},				//plus sign on keyboard
	{"BACK", 0x0e},				//backspace
	{"INSERT", 0x52},			//numlock -> num0
	{"HOME", 0x47},				//numlock -> num7
	{"PAGE_UP", 0x49},			//numlock -> num9
	{"NUM_LOCK", 0x45},
	{"NUM_SLASH", 0x35},	//from AHK, triggers keyboard slash, not keypad
	{"NUM_ASTERISK", 0x37},	//from AHK
	{"NUM_MINUS", 0x4a},	//from AHK
	{"TAB", 0x0f},
	{"Q", 0x10},
	{"W", 0x11},
	{"E", 0x12},
	{"R", 0x13},
	{"T", 0x14},
	{"Y", 0x15},
	{"U", 0x16},
	{"I", 0x17},
	{"O", 0x18},
	{"P", 0x19},
	{"OPEN_BRACKET", 0x1a},
	{"CLOSE_BRACKET", 0x1b},
	{"BACKSLASH", 0x2b},
	{"KEYBOARD_DELETE",0x53},	//numlock -> num.
	{"END", 0x4f},				//numlock -> num1
	{"PAGE_DOWN", 0x51},		//numlock -> num3
	//{"NUM_SEVEN", VK_NUMPAD7},	//numpad keys are overwritten with the corresponding no numlock counterp
	//{"NUM_EIGHT", VK_NUMPAD8},
	//{"NUM_NINE", VK_NUMPAD9},
	{"NUM_PLUS", 0x4e},			//from AHK
	{"CAPS_LOCK", 0x3a},		//caps lock
	{"A", 0x1e},
	{"S", 0x1f},
	{"D", 0x20},
	{"F", 0x21},
	{"G", 0x22},
	{"H", 0x23},
	{"J", 0x24},
	{"K", 0x25},
	{"L", 0x26},
	{"SEMICOLON", 0x27},
	{"APOSTROPHE", 0x28},
	{"ENTER", 0x1c},			//keyboard enter
	//{"NUM_ENTER", 0x11c},		//numpad enter; no scan code
	//{"NUM_FOUR", VK_NUMPAD4},
	//{"NUM_FIVE", VK_NUMPAD5},
	//{"NUM_SIX", VK_NUMPAD6},
	{"LEFT_SHIFT", 0x2a},
	{"Z", 0x2c},
	{"X", 0x2d},
	{"C", 0x2e},
	{"V", 0x2f},
	{"B", 0x30},
	{"N", 0x31},
	{"M", 0x32},
	{"COMMA", 0x33},
	{"PERIOD", 0x34},
	{"FORWARD_SLASH", 0x35},
	{"RIGHT_SHIFT", 0x136},			//is the same as left shift, unless AHK is correct
	{"ARROW_UP", 0x48},
	//{"NUM_ONE", VK_NUMPAD1},
	//{"NUM_TWO", VK_NUMPAD2},
	//{"NUM_THREE", VK_NUMPAD3},
	{"LEFT_CONTROL", 0x1d},
	//{"LEFT_WINDOWS", 0x15b},			//no scan code
	{"LEFT_ALT", 0x38},
	{"SPACE", 0x39},
	{"RIGHT_ALT", 0x38},				//same as LEFT_ALT
	//{"RIGHT_WINDOWS", 0x15c},			//no scan code
	//{"APPLICATION_SELECT", 0x15d},	//no scan code
	//{"RIGHT_CONTORL", 0x11d},			//no scan code
	{"ARROW_LEFT", 0x4b},
	{"ARROW_DOWN", 0x50},
	{"ARROW_RIGHT", 0x4d},
	//{"NUM_ZERO", VK_NUMPAD0},
	//{"NUM_PERIOD", VK_DECIMAL}
	{"NUM_CLEAR", 0x4c}					//not a physical key, but triggered; numlock -> num5
});

UINT convertToLLCode(std::string& s, bool isVirtual) {
	if (isVirtual) {
		auto i = toVKCode.find(s);
		if (i != toVKCode.end()) {
			return (UINT)i->second;
		}
		return 0;	//note this is the same as VK_NUMPAD_ENTER!
	}
	else {
		auto i = toScanCode.find(s);
		if (i != toScanCode.end()) {
			return (UINT)i->second;
		}
		return 0;
	}
}

std::vector<KeyState> parseKeyInput(std::vector<std::string>& source, bool isVirtual = true) {
	std::vector<KeyState> ret;
	for (auto s : source) {
		size_t delim = s.find(":");
		if (delim == std::string::npos) {
			ret.push_back(KeyState(convertToLLCode(s, isVirtual), isVirtual, true));	//down
			ret.push_back(KeyState(convertToLLCode(s, isVirtual), isVirtual, false));	//up
		}
		else {
			std::string keyCode = s.substr(delim + 1, s.length());
			if (s.substr(0, delim) == "DOWN") {
				ret.push_back(KeyState(convertToLLCode(keyCode, isVirtual), isVirtual, true));	//down
			}
			else {
				ret.push_back(KeyState(convertToLLCode(keyCode, isVirtual), isVirtual, false));	//up
			}
		}
	}
	return ret;
}

std::vector<int> parseKeyDelay(std::vector<KeyState>& keySeq, std::vector<int>& delaySeq, int defaultDelay) {
	std::vector<int> ret;
	if (keySeq.size() == 0) {
		return ret;
	}
	for (int i = 0; i < keySeq.size() - 1; i++) {
		if (i < delaySeq.size()) {
			ret.push_back(delaySeq[i]);
		}
		else {
			ret.push_back(defaultDelay);
		}
	}
	return ret;
}

LogiLed::KeyName convertFromScan(DWORD scanCode) {
	return LogiLed::KeyName(scanCode);
}

BOOL initInstance(HINSTANCE, int);
BOOL initLogi(bool msgbox = false);
void parseSettings();
void watchFile(HANDLE h);
void setActiveProfile(bool needToAcquire = true);

void stopPainter();
void startPainter();
void restartPainter(bool wait = false);