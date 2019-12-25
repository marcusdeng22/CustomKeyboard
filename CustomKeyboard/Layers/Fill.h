#pragma once

#include "Layer.h"
#include <endpointvolume.h>
#include <mmdeviceapi.h>

class Fill : public Layer {
private:
	bool initialized;
	IAudioEndpointVolume* micVolume;

	void paint(std::vector<unsigned char>&);
public:
	Fill(Condition cond = Condition::None, Color c = Color(255, 255, 255));
	Fill(LogiLed::KeyName k, Condition cond = Condition::None, Color c = Color(255, 255, 255));
	Fill(std::list<LogiLed::KeyName> l, Condition cond = Condition::None, Color c = Color(255, 255, 255));
	~Fill();
	void tick(std::vector<unsigned char>&);
	void updateDevice();
};