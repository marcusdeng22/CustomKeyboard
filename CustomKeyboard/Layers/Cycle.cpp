#include "stdafx.h"
#include "Cycle.h"
#include "LogitechLEDLib.h"
#include <map>
#include <vector>

Cycle::Cycle() {
	cycleCounter = 0;
}

void Cycle::initialize() {
//Cycle::Cycle(unsigned char* bitmap) {
//Cycle::Cycle(std::vector<unsigned char> * bitmap) {
	color = Color(255, 0, 0);	//start on red
	/*this->bitmap = bitmap;
	wchar_t buf[256];
	wsprintf(buf, L"%x", (this->bitmap));
	OutputDebugString(L"cycle color bitmap addr: ");
	OutputDebugString(buf);
	OutputDebugString(L"\n");*/
	for (auto const & k : LogiLed::bitmapIndex) {	//change this to join from all keys
		affectedKeys.push_back(k.first);
	}
	//affectedKeys(LogiLed::allKeys, LOGI_NUMBER_KEYS);
}

void Cycle::Tick() {
	//OutputDebugString(L"Cycle tick\n");
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
		/*colorMap->bitmap->at(index) = color.b;
		colorMap->bitmap->at(index + 1) = color.g;
		colorMap->bitmap->at(index + 2) = color.r;
		colorMap->bitmap->at(index + 3) = color.a;*/
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
	////set the g keys
	//colorMap->G1 = Color(color.r, color.g, color.b);
	//colorMap->G2 = Color(color.r, color.g, color.b);
	//colorMap->G3 = Color(color.r, color.g, color.b);
	//colorMap->G4 = Color(color.r, color.g, color.b);
	//colorMap->G5 = Color(color.r, color.g, color.b);
	//colorMap->G6 = Color(color.r, color.g, color.b);
	//colorMap->G7 = Color(color.r, color.g, color.b);
	//colorMap->G8 = Color(color.r, color.g, color.b);
	//colorMap->G9 = Color(color.r, color.g, color.b);
	////set the badge
	//colorMap->Badge = Color(color.r, color.g, color.b);
	////set the logo
	//colorMap->Logo = Color(color.r, color.g, color.b);
	////set the mouse
	//colorMap->Mouse0 = Color(color.r, color.g, color.b);
	//colorMap->Mouse1 = Color(color.r, color.g, color.b);

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