#pragma once

#include "Layer.h"
#include <endpointvolume.h>
#include <mmdeviceapi.h>

class Fill : public Layer {
private:
	bool initialized;
	IAudioEndpointVolume* micVolume;

	void paint();
public:
	std::string name = "fill";
	Fill(Condition c = Condition::None);
	~Fill();
	void initialize();
	void initialize(Color c = Color(255, 255, 255));
	void initialize(LogiLed::KeyName k, Color c = Color(255, 255, 255));
	void initialize(std::list<LogiLed::KeyName> l, Color c = Color(255, 255, 255));
	void Tick();
	void updateDevice();
};