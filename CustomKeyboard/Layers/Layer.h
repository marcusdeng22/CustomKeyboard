#pragma once

#include "LogitechLEDLib.h"
#include <list>
#include "Color.h"
#include <vector>

enum class Condition {
	None,
	MicMuted
};

class Layer {
protected:
	int delta = 5;
	Condition condition = Condition::None;
	std::list<LogiLed::KeyName> affectedKeys;
	//std::list<LogiLed::ExtraKeys> affectedExtras;
	Color color;
	//unsigned char *bitmap;
	//std::vector<unsigned char>* bitmap;
	//ColorMap *colorMap = nullptr;
	std::vector<unsigned char>* colorVector = nullptr;
	bool associated = false;

public:
	virtual void initialize() = 0;

	//void Associate(ColorMap *ptr) {
	void Associate(std::vector<unsigned char>* ptr) {
		colorVector = ptr;
		associated = true;
	}

	/*void SetCondition(Condition c) {
		condition = c;
	}*/

	Condition GetCondition() {
		return condition;
	}

	//bool ConditionMet() {
	//	return true;	//TODO: implement condition checking
	//}

	void SetDelta(int v) {
		delta = v;
	}

	int GetDelta() {
		return delta;
	}

	void RegisterKey(LogiLed::KeyName k) {	//do we need a lock on this?
		affectedKeys.push_back(k);
	}

	void RemoveKey(LogiLed::KeyName k) {
		affectedKeys.remove(k);
	}

	virtual void Tick() = 0;
};