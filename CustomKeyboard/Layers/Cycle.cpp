#include "stdafx.h"
#include "Cycle.h"
#include "LogitechLEDLib.h"

Cycle::Cycle() {
	cycleCounter = 0;
	color = Color(255, 0, 0);	//start on red
}

Cycle::Cycle(int d) {
	Cycle();
	delta = d;
}

void Cycle::initialize() {
	for (auto const & k : LogiLed::bitmapIndex) {	//change this to join from all keys?
		affectedKeys.push_back(k.first);
	}
}

void Cycle::Tick() {
	if (!associated) {
		return;
	}
	//set the bitmap
	for (LogiLed::KeyName k : affectedKeys) {
		//lookup the index in the bitmap
		size_t index = LogiLed::bitmapIndex.at(k);
		(*colorVector)[index] = color.b;
		(*colorVector)[index + 1] = color.g;
		(*colorVector)[index + 2] = color.r;
		(*colorVector)[index + 3] = color.a;
	}

	//increment colors
	if (cycleCounter == 0) {
		if (color.g + delta >= 255) {	//increase green
			color.g = 255;
			cycleCounter += 1;
		}
		else {
			color.g += delta;
		}
	}
	else if (cycleCounter == 1) {	//decrease red
		if (color.r - delta <= 0) {
			color.r = 0;
			cycleCounter += 1;
		}
		else {
			color.r -= delta;
		}
	}
	else if (cycleCounter == 2) {	//increase blue
		if (color.b + delta >= 255) {
			color.b = 255;
			cycleCounter += 1;
		}
		else {
			color.b += delta;
		}
	}
	else if (cycleCounter == 3) {	//decrease green
		if (color.g - delta <= 0) {
			color.g = 0;
			cycleCounter += 1;
		}
		else {
			color.g -= delta;
		}
	}
	else if (cycleCounter == 4) {	//increase red
		if (color.r + delta >= 255) {
			color.r = 255;
			cycleCounter += 1;
		}
		else {
			color.r += delta;
		}
	}
	else {	//decrease blue
		if (color.b - delta <= 0) {
			color.b = 0;
			cycleCounter = 0;
		}
		else {
			color.b -= delta;
		}
	}
}