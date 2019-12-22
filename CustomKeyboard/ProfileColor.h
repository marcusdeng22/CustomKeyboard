#pragma once
#include "stdafx.h"
#include "Layers/Layer.h"
#include "LogitechLEDLib.h"
#include "Color.h"
#include <vector>
#include <thread>

class ProfileColor {
private:
	std::vector<Layer*> layers;
	int tick = 20;	//update keyboard 50 times a second
	//unsigned char bitmap[LOGI_LED_BITMAP_SIZE] = { 0 };
	//std::vector<unsigned char>* bitmap = new std::vector<unsigned char>(LOGI_LED_BITMAP_SIZE);
	//ColorMap *colorMapping = new ColorMap();
	std::vector<unsigned char>* colorVector = new std::vector<unsigned char>(LOGI_COLORMAP_FULL_SIZE);	//each 4 indices is BGRA value

public:
	ProfileColor() {
		//bitmap->resize(LOGI_LED_BITMAP_SIZE, 0);
	}

	~ProfileColor() {
		OutputDebugString(L"cleaning bitmap pointer\n");
		//delete bitmap;
		//delete colorMapping;
		delete colorVector;
	}

	/*unsigned char* GetBitmap() {
		wchar_t buf[256];
		wsprintf(buf, L"%x", bitmap);
		OutputDebugString(L"Profile color bitmap addr: ");
		OutputDebugString(buf);
		OutputDebugString(L"\n");
		return bitmap;
	}*/

	/*std::vector<unsigned char>* GetBitmap() {
		return bitmap;
	}*/

	/*ColorMap* GetColorMap() {
		return colorMapping;
	}*/

	void AddLayer(Layer* l) {
		//l->Associate(colorMapping);
		l->Associate(colorVector);
		layers.push_back(l);
	}

	//compute a bitmap array, and send to SDK
	//also handles the g keys, logo, badge, and mouse colors
	void SendColor();
};