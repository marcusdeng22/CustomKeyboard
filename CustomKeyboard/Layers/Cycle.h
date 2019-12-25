#pragma once

#include "Layer.h"

class Cycle : public Layer {
private:
	int cycleCounter;

public:
	Cycle(int d = 5);
	void tick(std::vector<unsigned char>&);
};