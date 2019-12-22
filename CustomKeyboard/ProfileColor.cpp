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
			OutputDebugString(L"ticking\n");
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
		//color the bitmap
		LogiLedSetLightingFromBitmap(&(*colorMapping->bitmap)[0]);
		//color the G keys
		LogiLedSetLightingForKeyWithKeyName(LogiLed::KeyName::G_1, colorMapping->G1.r, colorMapping->G1.g, colorMapping->G1.b);
		LogiLedSetLightingForTargetZone(LogiLed::Mouse, 0, 100, 100, 100);	//dpi indicators
		LogiLedSetLightingForTargetZone(LogiLed::Mouse, 1, 100, 100, 100);	//logo
		Sleep(ProfileColor::tick);
	}
}