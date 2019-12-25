#pragma once

#include "stdafx.h"
#include <vector>
#include <thread>
#include <atomic>
#include <memory>
#include "Layers/Layer.h"
#include "Layers/Cycle.h"
#include "Layers/Echo.h"
#include "Layers/Fill.h"
#include "LogitechLEDLib.h"
#include "Color.h"

enum class LayerType {
	Cycle,
	Echo,
	Fill
};

class ProfileColor {
private:
	std::vector<std::shared_ptr<Layer>> layers;	//was unique
	int tick;
	std::vector<unsigned char> colorVector = std::vector<unsigned char>(LOGI_COLORMAP_FULL_SIZE);	//each 4 indices is BGRA value
	std::atomic_bool readyToPaint{ false };
	std::list<size_t> echoLayers = std::list<size_t>();

public:
	ProfileColor() {
		tick = 20;	//update keyboard 50 times a second
	}

	template<typename T, typename... Args>
	void addLayer(Args&&... args) {
		//layers.push_back(std::make_shared<T>(std::forward<Args>(args)...));	//was unique
		layers.push_back(std::make_shared<T>(args...));
		if (std::is_same<T, Echo>::value) {
			echoLayers.push_back(layers.size() - 1);
		}
	}

	//compute a bitmap array, and send to SDK
	//also handles the g keys, logo, badge, and mouse colors
	void sendColor();

	void stop() {
		readyToPaint = false;
	}

	void notifyEcho(LogiLed::KeyName k) {
		for (size_t i : echoLayers) {
			layers[i]->registerKey(k);
		}
	}

	void notifyEchoStart(LogiLed::KeyName k) {
		for (size_t i : echoLayers) {
			layers[i]->startKey(k);
		}
	}
};