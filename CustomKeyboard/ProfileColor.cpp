#include "stdafx.h"
#include "ProfileColor.h"
#include "LogitechLEDLib.h"
#include "Layers/Layer.h"
#include <map>
#include <vector>

//compute a bitmap array, and send to SDK
//also handles the g keys, logo, badge, and mouse colors
void ProfileColor::SendColor() {
	while (true) {
		for (Layer* l : ProfileColor::layers) {
			//OutputDebugString(L"ticking\n");
			l->Tick();
		}
		/*wchar_t buf[256];
		OutputDebugString(L"values from profile color\n");
		wsprintf(buf, L"%d", bitmap[0]);
		OutputDebugString(buf);
		OutputDebugString(L"\n");
		wsprintf(buf, L"%d", bitmap[1]);
		OutputDebugString(buf);
		OutputDebugString(L"\n");
		wsprintf(buf, L"%d", bitmap[2]);
		OutputDebugString(buf);
		OutputDebugString(L"\n");
		wsprintf(buf, L"%d", bitmap[3]);
		OutputDebugString(buf);
		OutputDebugString(L"\n");*/
		//unsigned char* mapping = &(*bitmap)[0];
		////color the bitmap
		//LogiLedSetLightingFromBitmap(&(*colorMapping->bitmap)[0]);
		////color the G keys
		//LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::G_1, colorMapping->G1);
		//LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::G_2, colorMapping->G2);
		//LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::G_3, colorMapping->G3);
		//LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::G_4, colorMapping->G4);
		//LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::G_5, colorMapping->G5);
		//LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::G_6, colorMapping->G6);
		//LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::G_7, colorMapping->G7);
		//LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::G_8, colorMapping->G8);
		//LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::G_9, colorMapping->G9);
		////color logo and badge
		//LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::G_LOGO, colorMapping->Logo);
		//LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::G_BADGE, colorMapping->Badge);
		////color the mouse
		//LogiLedSetLightingForTargetZone(LogiLed::DeviceType::Mouse, 0, colorMapping->Mouse0);	//dpi indicators
		//LogiLedSetLightingForTargetZone(LogiLed::DeviceType::Mouse, 1, colorMapping->Mouse1);	//logo

		//get and color the bitmap
		std::vector<unsigned char> bitmap(colorVector->begin(), colorVector->begin() + LOGI_LED_BITMAP_SIZE);
		LogiLedSetLightingFromBitmap(&bitmap[0]);
		//color G keys, logo, and badge
		/*for (int i = LOGI_LED_BITMAP_SIZE; i < LOGI_COLORMAP_MOUSE_I; i += 4) {
			LogiLedSetLightingForKeyWithKeyName()
		}*/
		for (LogiLed::KeyName k : LogiLed::NotInBitmap) {
			//get the index for key, and create the color from the color vector
			size_t i = LogiLed::bitmapIndex.at(k);
			Color c(colorVector->at(i+2), colorVector->at(i+1), colorVector->at(i), colorVector->at(i+3));
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