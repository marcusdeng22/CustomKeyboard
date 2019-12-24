#pragma once
#include "Layer.h"
#include <vector>
#include <map>

struct ColorAlpha {
	Color curColor;
	double curAlpha;
	bool started;

	ColorAlpha(Color c) {
		curColor = c;
		curAlpha = 1.0;
		started = false;
	}

	void reset(Color c) {
		curColor = c;
		curAlpha = 1.0;
		started = false;
	}
};

class Echo : public Layer {
private:
	Color startColor;	//must be set; default white
	Color targetColor;	//this is the end color; if the alpha for this is 0 then default to transparent and sample the previous layer's value as the matte
	std::map<LogiLed::KeyName, ColorAlpha> keyMapping;	//stores the current alpha value
	double delta;

public:
	Echo();
	void initialize();
	void RegisterKey(LogiLed::KeyName k);
	void RemoveKey(LogiLed::KeyName k);
	void startKey(LogiLed::KeyName k);
	void Tick();
};