#pragma once

#include "resource.h"
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


#include "LogitechLEDLib.h"
#include "lib/json.hpp"
#include "Profile.h"
#include "Layers/Cycle.h"
#include "Layers/Echo.h"
#include "Layers/Fill.h"

static const std::map<std::string, Condition> toCondition({
	{"None", Condition::None},
	{"MicMuted", Condition::MicMuted}
});

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

std::list<LogiLed::KeyName> convertList(std::vector<std::string>& source) {
	std::list<LogiLed::KeyName> ret;
	for (auto s : source) {
		ret.push_back(toKey.at(s));
	}
	return ret;
}

BOOL initInstance(HINSTANCE, int);
void parseSettings();