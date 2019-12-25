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
	//std::vector<Layer*> layers;
	std::vector<std::shared_ptr<Layer>> layers;	//was unique
	//std::vector<std::shared_ptr<Layer>> layers;
	int tick;
	//std::vector<unsigned char>* colorVector = new std::vector<unsigned char>(LOGI_COLORMAP_FULL_SIZE);	//each 4 indices is BGRA value
	std::vector<unsigned char> colorVector = std::vector<unsigned char>(LOGI_COLORMAP_FULL_SIZE);
	std::atomic_bool readyToPaint{ false };
	std::list<size_t> echoLayers = std::list<size_t>();

public:
	ProfileColor() {
		tick = 20;	//update keyboard 50 times a second
	}

	~ProfileColor() {
		//delete colorVector;
	}

	template<typename T, typename... Args>
	void AddLayer(Args&&... args) {
		OutputDebugString(L"adding layer\n");
		//layers.push_back(std::make_shared<T>(std::forward<Args>(args)...));	//was unique
		layers.push_back(std::make_shared<T>(args...));
		if (std::is_same<T, Echo>::value) {
			OutputDebugStringW(L"added echo layer\n");
			echoLayers.push_back(layers.size() - 1);
		}
	}

	//void AddLayer(LayerType t) {
	//	OutputDebugString(L"single argument add\n");
	//	if (t == LayerType::Cycle) {
	//		layers.push_back(std::make_unique<Cycle>());
	//	}
	//	else if (t == LayerType::Echo) {
	//		layers.push_back(std::make_unique<Echo>());
	//	}
	//	else if (t == LayerType::Fill) {
	//		layers.push_back(std::make_unique<Fill>());
	//	}
	//}

	//template<typename... Args>
	//void AddLayer(LayerType t, Args&& ... args) {
	//	OutputDebugString(L"variable argument add\n");
	//	char buf[256];
	//	sprintf_s(buf, "layer type: %d\n", t);
	//	OutputDebugStringA(buf);
	//	if (t == LayerType::Cycle) {
	//		//layers.emplace_back(&Cycle(args));
	//		OutputDebugString(L"making cycle layer\n");
	//		sprintf_s(buf, "%d args\n", sizeof...(Args));
	//		OutputDebugStringA(buf);
	//		//layers.push_back(std::make_unique<Cycle>(args...));
	//	}
	//	else if (t == LayerType::Echo) {
	//		//layers.emplace_back(&Echo(args));
	//		Echo e (std::forward<Args>(args)...);
	//		//layers.push_back(std::make_unique<Echo>(args...));
	//		OutputDebugString(L"making echo layer\n");
	//		sprintf_s(buf, "%d args\n", sizeof...(Args));
	//		OutputDebugStringA(buf);
	//		sprintf_s(buf, "%d %d %d\n", args...);
	//		OutputDebugStringA(buf);
	//	}
	//	else if(t == LayerType::Fill) {
	//		//layers.emplace_back(&Fill(args));
	//		layers.push_back(std::make_unique<Fill>(args...));
	//		OutputDebugString(L"making fill layer\n");
	//		sprintf_s(buf, "%d args\n", sizeof...(Args));
	//		OutputDebugStringA(buf);
	//		sprintf_s(buf, "%d %d %d\n", args...);
	//		OutputDebugStringA(buf);
	//	}
	//}

	//void AddLayer(Layer* l) {
	////void AddLayer(std::shared_ptr<Layer> l) {
	//	l->Associate(colorVector);
	//	layers.push_back(l);
	//	//layers.push_back(std::make_shared<Layer>(l));
	//}

	//compute a bitmap array, and send to SDK
	//also handles the g keys, logo, badge, and mouse colors
	void SendColor();

	void stop() {
		readyToPaint = false;
	}

	/*ProfileColor& operator = (const ProfileColor& pc) {
		layers = pc.layers;
		tick = pc.tick;
		colorVector = pc.colorVector;
		readyToPaint = false;
		return *this;
	}*/

	void notifyEcho(LogiLed::KeyName k) {
		char buf[256];
		sprintf_s(buf, "number of echo layers: %d\n", echoLayers.size());
		OutputDebugStringA(buf);
		for (size_t i : echoLayers) {
			layers[i]->RegisterKey(k);
		}
	}

	void notifyEchoStart(LogiLed::KeyName k) {
		for (size_t i : echoLayers) {
			layers[i]->startKey(k);
		}
	}
};