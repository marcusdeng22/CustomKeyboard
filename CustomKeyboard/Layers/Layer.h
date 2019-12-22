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
	int delta = 2;
	Condition condition;
	std::list<LogiLed::KeyName> affectedKeys;
	Color color;
	//unsigned char *bitmap;
	std::vector<unsigned char>* bitmap;

public:
	void SetCondition(Condition c) {
		condition = c;
	}

	Condition GetCondition() {
		return condition;
	}

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