#pragma once

#include "Layer.h"

class Cycle : public Layer {
private:
	int cycleCounter;

public:
	std::string name = "cycle";
	//Cycle();
	Cycle(int d = 5);
	//void initialize();
	void Tick(std::vector<unsigned char>&);
};