#include "stdafx.h"
#include "Echo.h"
#include "LogitechLEDLib.h"

Echo::Echo(double d, Color start, Color end) {
	delta = d;
	startColor = start;
	targetColor = end;
}

//override the register key; we don't want to use a list
void Echo::registerKey(LogiLed::KeyName k) {
	keyLock.lock();
	//check if key is in map; if so reset the alpha
	if (keyMapping.find(k) == keyMapping.end()) {
		keyMapping.insert({ k, ColorAlpha(startColor) });
	}
	else {
		keyMapping.at(k).reset(startColor);
	}
	keyLock.unlock();
}

void Echo::removeKey(LogiLed::KeyName k) {
	keyLock.lock();
	if (keyMapping.find(k) != keyMapping.end()) {
		keyMapping.erase(k);
	}
	keyLock.unlock();
}

void Echo::clearKeys() {
	keyLock.lock();
	OutputDebugString(L"echo clear keys hit\n");
	keyMapping.clear();
	keyLock.unlock();
}

//start the animation; this prevents weird things when holding down keys
void Echo::startKey(LogiLed::KeyName k) {
	keyLock.lock();
	auto i = keyMapping.find(k);
	if (i != keyMapping.end()) {
		i->second.started = true;
	}
	keyLock.unlock();
}

void Echo::tick(std::vector<unsigned char>& colorVector) {
	keyLock.lock();
	std::vector<LogiLed::KeyName> toRemove;
	for (auto& mapping : keyMapping) {
		//lookup the index in the bitmap
		size_t index = LogiLed::bitmapIndex.at(mapping.first);
		//if the target color has zero alpha, sample the current value before changing it
		Color sample = targetColor;
		if (targetColor.a == 0) {	//sample the background if the target color is transparent
			sample = Color((colorVector)[index + 2], (colorVector)[index + 1], (colorVector)[index], (colorVector)[index + 3]);
		}
		//get the ColorAlpha struct for the key
		ColorAlpha* ca = &mapping.second;
		//update the colors
		(colorVector)[index] = ca->curColor.b;
		(colorVector)[index + 1] = ca->curColor.g;
		(colorVector)[index + 2] = ca->curColor.r;
		(colorVector)[index + 3] = ca->curColor.a;	//should be 255 since start color should have A=255

		if (ca->started) {	//start animation
			if (ca->curAlpha - delta <= 0) {
				//deregister this key after ticked to target
				toRemove.push_back(mapping.first);
			}
			else {
				//update the alpha
				ca->curAlpha = ca->curAlpha - delta;
				//calculate the new color
				ca->curColor.r = (unsigned char)round(((1 - ca->curAlpha) * sample.r) + (ca->curAlpha * ca->curColor.r));
				ca->curColor.g = (unsigned char)round(((1 - ca->curAlpha) * sample.g) + (ca->curAlpha * ca->curColor.g));
				ca->curColor.b = (unsigned char)round(((1 - ca->curAlpha) * sample.b) + (ca->curAlpha * ca->curColor.b));
			}
		}
	}
	for (LogiLed::KeyName k : toRemove) {
		keyMapping.erase(k);
	}
	keyLock.unlock();
}