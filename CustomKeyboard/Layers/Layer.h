#pragma once

#include "LogitechLEDLib.h"
#include "Color.h"
#include <mutex>
#include <algorithm>

enum class Condition {
	None,
	MicMuted,
	AudioOutput
};

class Layer {
protected:
	int delta = 5;
	Condition condition = Condition::None;
	std::list<LogiLed::KeyName> affectedKeys;
	Color color;
	std::vector<unsigned char>* colorVector = nullptr;
	std::mutex keyLock;

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

	virtual void registerKey(LogiLed::KeyName k) {
		keyLock.lock();
		affectedKeys.push_back(k);
		keyLock.unlock();
	}

	virtual void removeKey(LogiLed::KeyName k) {
		keyLock.lock();
		if (std::find(affectedKeys.begin(), affectedKeys.end(), k) != affectedKeys.end()) {
			affectedKeys.remove(k);
		}
		keyLock.unlock();
	}

	virtual void clearKeys() {
		keyLock.lock();
		affectedKeys.clear();
		keyLock.unlock();
	}

	virtual void startKey(LogiLed::KeyName) {}

	virtual void tick(std::vector<unsigned char>&) = 0;
};