#include <stdafx.h>
#include "ProfileColor.h"
#include "LogitechLEDLib.h"
#include "Layers/Layer.h"

//compute a bitmap array, and send to SDK
//also handles the g keys, logo, badge, and mouse colors
void ProfileColor::SendColor() {
	readyToPaint = true;
	while (readyToPaint) {
		//for (Layer* l : ProfileColor::layers) {	//go through each layer and update the color vector
		//for (std::shared_ptr<Layer> l : ProfileColor::layers) {	//go through each layer and update the color vector
		for (auto& l : ProfileColor::layers) {	//go through each layer and update the color vector
			l->Tick(colorVector);
		}

		//get and color the bitmap
		//std::vector<unsigned char> bitmap(colorVector->begin(), colorVector->begin() + LOGI_LED_BITMAP_SIZE);
		std::vector<unsigned char> bitmap(colorVector.begin(), colorVector.begin() + LOGI_LED_BITMAP_SIZE);
		LogiLedSetLightingFromBitmap(&bitmap[0]);
		//color G keys, logo, and badge
		for (LogiLed::KeyName k : LogiLed::NotInBitmap) {
			//get the index for key, and create the color from the color vector
			size_t i = LogiLed::bitmapIndex.at(k);
			//Color c(colorVector->at(i+2), colorVector->at(i+1), colorVector->at(i), colorVector->at(i+3));	//bgra
			Color c(colorVector.at(i + 2), colorVector.at(i + 1), colorVector.at(i), colorVector.at(i + 3));	//bgra
			if (k == LogiLed::KeyName::MOUSE_Z0) {
				LogiLedSetLightingForTargetZone(LogiLed::DeviceType::Mouse, 0, c);
			}
			else if (k == LogiLed::KeyName::MOUSE_Z1) {
				LogiLedSetLightingForTargetZone(LogiLed::DeviceType::Mouse, 1, c);
			}
			else {
				LogiLedSetLightingForKeyWithKeyName(k, c);
			}
		}
		Sleep(ProfileColor::tick);
	}
}