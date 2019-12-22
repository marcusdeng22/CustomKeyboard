#include "stdafx.h"
#include "Cycle.h"
#include "LogitechLEDLib.h"
#include <map>
#include <vector>

//Cycle::Cycle(unsigned char* bitmap) {
Cycle::Cycle(std::vector<unsigned char> * bitmap) {
	color = Color(255, 0, 0);	//start on red
	this->bitmap = bitmap;
	wchar_t buf[256];
	wsprintf(buf, L"%x", (this->bitmap));
	OutputDebugString(L"cycle color bitmap addr: ");
	OutputDebugString(buf);
	OutputDebugString(L"\n");
	for (auto const & k : LogiLed::bitmapIndex) {
		affectedKeys.push_back(k.first);
	}
}

void Cycle::Tick() {
	//set the bitmap
	for (LogiLed::KeyName k : affectedKeys) {
		//lookup the index in the bitmap
		int index = LogiLed::bitmapIndex.at(k);
		(*bitmap)[index] = color.b;
		(*bitmap)[index + 1] = color.g;
		(*bitmap)[index + 2] = color.r;
		(*bitmap)[index + 3] = color.a;
	}
	/*wchar_t buf[256];
	OutputDebugString(L"values from tick\n");
	wsprintf(buf, L"%d", (*bitmap)[0]);
	OutputDebugString(buf);
	OutputDebugString(L"\n");
	wsprintf(buf, L"%d", (*bitmap)[1]);
	OutputDebugString(buf);
	OutputDebugString(L"\n");
	wsprintf(buf, L"%d", (*bitmap)[2]);
	OutputDebugString(buf);
	OutputDebugString(L"\n");
	wsprintf(buf, L"%d", (*bitmap)[3]);
	OutputDebugString(buf);
	OutputDebugString(L"\n");*/
	//set the g keys
	//set the badge
	//set the logo
	//set the mouse
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