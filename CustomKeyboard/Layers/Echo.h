#pragma once

#include "Layer.h"

struct ColorAlpha {
	Color curColor;
	double curAlpha;
	bool started;

	ColorAlpha(Color c) {
		curColor = c;
		curAlpha = 1.0;	//scaled alpha value 0-1
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
	std::string name = "echo";
	//Echo();
	//Echo(int d) {}
	Echo(double d = 0.01, Color start = Color(255, 255, 255), Color end = Color());
	/*void initialize();
	void initialize(Color start, Color end = Color());*/
	void RegisterKey(LogiLed::KeyName k);
	void RemoveKey(LogiLed::KeyName k);
	virtual void startKey(LogiLed::KeyName k) override;
	void Tick(std::vector<unsigned char>&);
};