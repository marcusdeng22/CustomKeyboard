#pragma once

#include "Layer.h"
#include <vector>

class Cycle : public Layer {
private:
	int cycleCounter = 0;

public:
	//Cycle(unsigned char* bitmap);
	Cycle(std::vector<unsigned char>* bitmap);
	void Tick();
};