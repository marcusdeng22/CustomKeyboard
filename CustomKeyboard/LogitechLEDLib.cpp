#include "stdafx.h"
#include "LogitechLEDLib.h"

//overloaded functions to apply a Color object
bool LogiLedSetLighting(Color &color) {
	Color c = color.toRGB();
	return LogiLedSetLighting(c.convR(), c.convG(), c.convB());
}

bool LogiLedFlashLighting(Color& color, int msDur, int msInt) {
	Color c = color.toRGB();
	return LogiLedFlashLighting(c.convR(), c.convG(), c.convB(), msDur, msInt);
}

bool LogiLedPulseLighting(Color& color, int msDur, int msInt) {
	Color c = color.toRGB();
	return LogiLedPulseLighting(c.convR(), c.convG(), c.convB(), msDur, msInt);
}

bool LogiLedSetLightingForKeyWithScanCode(int keyCode, Color& color) {
	Color c = color.toRGB();
	return LogiLedSetLightingForKeyWithScanCode(keyCode, c.convR(), c.convG(), c.convB());
}

bool LogiLedSetLightingForKeyWithHidCode(int keyCode, Color& color) {
	Color c = color.toRGB();
	return LogiLedSetLightingForKeyWithHidCode(keyCode, c.convR(), c.convG(), c.convB());
}

bool LogiLedSetLightingForKeyWithQuartzCode(int keyCode, Color& color) {
	Color c = color.toRGB();
	return LogiLedSetLightingForKeyWithQuartzCode(keyCode, c.convR(), c.convG(), c.convB());
}

bool LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName keyName, Color& color) {
	Color c = color.toRGB();
	return LogiLedSetLightingForKeyWithKeyName(keyName, c.convR(), c.convG(), c.convB());
}

bool LogiLedFlashSingleKey(LogiLed::KeyName keyName, Color& color, int msDur, int msInt) {
	Color c = color.toRGB();
	return LogiLedFlashSingleKey(keyName, c.convR(), c.convG(), c.convB(), msDur, msInt);
}

bool LogiLedPulseSingleKey(LogiLed::KeyName keyName, Color& color1, Color& color2, int msDur, int msInt) {
	Color c1 = color1.toRGB();
	Color c2 = color2.toRGB();
	return LogiLedPulseSingleKey(keyName, c1.convR(), c1.convG(), c1.convB(), c2.convR(), c2.convG(), c2.convB(), msDur, msInt);
}

bool LogiLedSetLightingForTargetZone(LogiLed::DeviceType device, int zone, Color& color) {
	Color c = color.toRGB();
	return LogiLedSetLightingForTargetZone(device, zone, c.convR(), c.convG(), c.convB());
}