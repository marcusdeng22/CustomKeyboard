#pragma once

#include <vector>
//#include "LogitechLEDLib.h"
#include <math.h>

//typedef enum class Mode {
//	RGB,
//	PER
//} Mode;

class Color {	//rgb on a scale of 0-255
//private:
	//Mode mode = Mode::RGB;
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

	//black color
	Color(int a) {
		this->r = 0;
		this->g = 0;
		this->b = 0;
		this->a = 255;
	}

	//converts R 0-255 to percent 0-100
	int convR() {
		return (int)(r / 255.0 * 100 + 0.5);
	}

	//converts G 0-255 to percent 0-100
	int convG() {
		return (int)(g / 255.0 * 100 + 0.5);
	}

	//converts B 0-255 to percent 0-100
	int convB() {
		return (int)(b / 255.0 * 100 + 0.5);
	}

	//converts A 0-255 to percent 0-100
	int convA() {
		return (int)(a / 255.0 * 100 + 0.5);
	}

	//returns a percent 0-1 of R
	double perR() {
		return r / 255.0;
	}

	//returns a percent 0-1 of G
	double perG() {
		return g / 255.0;
	}

	//returns a percent 0-1 of B
	double perB() {
		return b / 255.0;
	}

	//returns a percent 0-1 of A
	double perA() {
		return a / 255.0;
	}

	//overloaded toRGB() with a black matte
	Color toRGB() {
		Color t(0);
		return toRGB(t);
	}

	//converts the RGBA value to RGB; the alpha value stored in the return variable is not accurate
	//the result is intended for one time use only
	Color toRGB(Color& matte) {
		double alpha = perA();
		int t_r, t_g, t_b;
		t_r = (int)round(255 * (((1 - alpha) * matte.perR()) + (alpha * perR())));
		t_g = (int)round(255 * (((1 - alpha) * matte.perG()) + (alpha * perG())));
		t_b = (int)round(255 * (((1 - alpha) * matte.perB()) + (alpha * perB())));
		if (t_r > 255) { t_r = 255; }
		if (t_g > 255) { t_g = 255; }
		if (t_b > 255) { t_b = 255; }
		if (t_r < 0) { t_r = 0; }
		if (t_g < 0) { t_g = 0; }
		if (t_b < 0) { t_b = 0; }
		Color c(t_r, t_g, t_b);
		return c;
	}

	/*Mode Mode() {
		return mode;
	}*/

	//void Convert() {
	//	if (mode == Mode::RGB) {
	//		mode = Mode::PER;
	//		//convert to percentage
	//		r = (unsigned char)(r / 255.0 * 100 + 0.5);
	//		g = (unsigned char)(g / 255.0 * 100 + 0.5);
	//		b = (unsigned char)(b / 255.0 * 100 + 0.5);
	//		a = (unsigned char)(a / 255.0 * 100 + 0.5);
	//	}
	//	else {
	//		mode = Mode::RGB;
	//		//convert to RGB
	//		r = (unsigned char)(r / 100.0 * 255 + 0.5);
	//		g = (unsigned char)(g / 100.0 * 255 + 0.5);
	//		b = (unsigned char)(b / 100.0 * 255 + 0.5);
	//		a = (unsigned char)(a / 100.0 * 255 + 0.5);
	//	}
	//}
};


