#pragma once

#include "stdafx.h"
#include <vector>
#include <thread>
#include <atomic>
#include <memory>
#include "Layers/Layer.h"
#include "LogitechLEDLib.h"
#include "Color.h"

class ProfileColor {
private:
	std::vector<Layer*> layers;
	//std::vector<std::shared_ptr<Layer>> layers;
	int tick;
	std::vector<unsigned char>* colorVector = new std::vector<unsigned char>(LOGI_COLORMAP_FULL_SIZE);	//each 4 indices is BGRA value
	std::atomic_bool readyToPaint{ false };

public:
	ProfileColor() {
		tick = 20;	//update keyboard 50 times a second
	}

	~ProfileColor() {
		delete colorVector;
	}

	void AddLayer(Layer* l) {
	//void AddLayer(std::shared_ptr<Layer> l) {
		l->Associate(colorVector);
		layers.push_back(l);
		//layers.push_back(std::make_shared<Layer>(l));
	}

	//compute a bitmap array, and send to SDK
	//also handles the g keys, logo, badge, and mouse colors
	void SendColor();

	void stop() {
		readyToPaint = false;
	}

	ProfileColor& operator = (const ProfileColor& pc) {
		layers = pc.layers;
		tick = pc.tick;
		colorVector = pc.colorVector;
		readyToPaint = false;
		return *this;
	}
};