#pragma once

#include "LogitechLEDLib.h"
#include "Color.h"
#include <string>

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
	//bool associated = false;
	bool associated = true;

public:
	std::string name = "layer";
	//virtual void initialize() = 0;

	void Associate(std::vector<unsigned char>* ptr) {
		colorVector = ptr;
		associated = true;
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

	virtual void RegisterKey(LogiLed::KeyName k) {	//do we need a lock on this?
		affectedKeys.push_back(k);
	}

	virtual void RemoveKey(LogiLed::KeyName k) {
		affectedKeys.remove(k);
	}

	virtual void startKey(LogiLed::KeyName) {}

	virtual void Tick(std::vector<unsigned char>&) = 0;
};