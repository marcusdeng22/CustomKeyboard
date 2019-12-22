#pragma once

#include <vector>
#include "LogitechLEDLib.h"

typedef enum class Mode {
	RGB,
	PER
} Mode;

class Color {	//rgb on a scale of 0-255
private:
	Mode mode = Mode::RGB;
public:
	unsigned char r, g, b, a;

	Color() {
		r = 0;
		g = 0;
		b = 0;
		a = 0;
	}

	Color(int r, int g, int b) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = 255;
	}

	Color(int r, int g, int b, int a) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	Color(int a) {
		this->r = 0;
		this->g = 0;
		this->b = 0;
		this->a = 100;
		this->mode = Mode::PER;
	}

	Mode Mode() {
		return mode;
	}

	void Convert() {
		if (mode == Mode::RGB) {
			mode = Mode::PER;
			//convert to percentage
			r = (unsigned char)(r / 255.0 * 100 + 0.5);
			g = (unsigned char)(g / 255.0 * 100 + 0.5);
			b = (unsigned char)(b / 255.0 * 100 + 0.5);
			a = (unsigned char)(a / 255.0 * 100 + 0.5);
		}
		else {
			mode = Mode::RGB;
			//convert to RGB
			r = (unsigned char)(r / 100.0 * 255 + 0.5);
			g = (unsigned char)(g / 100.0 * 255 + 0.5);
			b = (unsigned char)(b / 100.0 * 255 + 0.5);
			a = (unsigned char)(a / 100.0 * 255 + 0.5);
		}
	}
};

struct ColorMap {
	std::vector<unsigned char>* bitmap = new std::vector<unsigned char>(LOGI_LED_BITMAP_SIZE);
	Color G1;
	Color G2;
	Color G3;
	Color G4;
	Color G5;
	Color G6;
	Color G7;
	Color G8;
	Color G9;
	Color Badge;
	Color Logo;
	Color Mouse0;
	Color Mouse1;

	ColorMap() : G1(0), G2(0), G3(0), G4(0), G5(0), G6(0), G7(0), G8(0), G9(0), Badge(0), Logo(0), Mouse0(0), Mouse1(0) {}
};