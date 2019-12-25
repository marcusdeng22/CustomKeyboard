#pragma once

#include "LogitechLEDLib.h"
#include "Color.h"

enum class Condition {
	None,
	MicMuted
};

class Layer {
protected:
	int delta = 5;
	Condition condition = Condition::None;
	std::list<LogiLed::KeyName> affectedKeys;
	Color color;
	std::vector<unsigned char>* colorVector = nullptr;

public:
	Condition getCondition() {
		return condition;
	}

	void setDelta(int v) {
		delta = v;
	}

	int getDelta() {
		return delta;
	}

	virtual void registerKey(LogiLed::KeyName k) {	//do we need a lock on this?
		affectedKeys.push_back(k);
	}

	virtual void removeKey(LogiLed::KeyName k) {
		affectedKeys.remove(k);
	}

	virtual void startKey(LogiLed::KeyName) {}

	virtual void tick(std::vector<unsigned char>&) = 0;
};