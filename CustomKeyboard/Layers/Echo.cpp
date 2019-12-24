#include "stdafx.h"
#include "Echo.h"
#include "LogitechLEDLib.h"
#include <map>
#include <vector>
#include <math.h>

Echo::Echo() {
	delta = 0.01;
}

void Echo::initialize() {
	startColor = Color(255, 255, 255);	//start on white
}

void Echo::RegisterKey(LogiLed::KeyName k) {
	//Layer::RegisterKey(k);
	//check if key is in map; if so reset the alpha
	if (keyMapping.find(k) == keyMapping.end()) {
		keyMapping.insert({ k, ColorAlpha(startColor) });
	}
	else {
		keyMapping.at(k).reset(startColor);
	}
	/*OutputDebugString(L"registered key\n");
	ColorAlpha t(Color(0, 0, 0));
	char buf[256];
	OutputDebugString(L"registered value echo\n");
	sprintf_s(buf, "%f %f\n", t.curAlpha, delta);
	OutputDebugStringA(buf);*/
}

void Echo::RemoveKey(LogiLed::KeyName k) {
	keyMapping.erase(k);
}

void Echo::startKey(LogiLed::KeyName k) {
	keyMapping.at(k).started = true;
}

void Echo::Tick() {
	//OutputDebugString(L"Echo tick\n");
	if (!associated) {
		return;
	}
	std::vector<LogiLed::KeyName> toRemove;
	for (auto& mapping : keyMapping) {
		//lookup the index in the bitmap
		size_t index = LogiLed::bitmapIndex.at(mapping.first);
		//if the target color has zero alpha, sample the current value before changing it
		Color sample = targetColor;
		if (targetColor.a == 0) {
			//OutputDebugString(L"sampling background\n");
			sample = Color((*colorVector)[index + 2], (*colorVector)[index + 1], (*colorVector)[index], (*colorVector)[index + 3]);
		}
		//get the ColorAlpha struct for the key
		ColorAlpha* ca = &mapping.second;
		//update the colors
		(*colorVector)[index] = ca->curColor.b;
		(*colorVector)[index + 1] = ca->curColor.g;
		(*colorVector)[index + 2] = ca->curColor.r;
		(*colorVector)[index + 3] = ca->curColor.a;

		/*char buf[256];
		OutputDebugString(L"registered value echo\n");
		sprintf_s(buf, "%f\n", ca->curAlpha);
		OutputDebugStringA(buf);*/

		if (ca->started) {
			if (ca->curAlpha - delta <= 0) {
				//deregister this key after tick
				toRemove.push_back(mapping.first);
			}
			else {
				//update the colors and alpha
				ca->curAlpha = ca->curAlpha - delta;
				/*sprintf_s(buf, "updated alpha: %f\n", ca->curAlpha);
				OutputDebugStringA(buf);*/
				//calculate the new color
				ca->curColor.r = (unsigned char)round(((1 - ca->curAlpha) * sample.r) + (ca->curAlpha * ca->curColor.r));
				ca->curColor.g = (unsigned char)round(((1 - ca->curAlpha) * sample.g) + (ca->curAlpha * ca->curColor.g));
				ca->curColor.b = (unsigned char)round(((1 - ca->curAlpha) * sample.b) + (ca->curAlpha * ca->curColor.b));
			}
		}
	}
	for (LogiLed::KeyName k : toRemove) {
		RemoveKey(k);
		//OutputDebugString(L"removed key\n");
	}
}