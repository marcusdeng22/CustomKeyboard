#pragma once

#include <vector>
#include <math.h>

class Color {	//rgb on a scale of 0-255
public:
	unsigned char r, g, b, a;

	Color() {
		this->r = 0;
		this->g = 0;
		this->b = 0;
		this->a = 0;
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

	Color(std::vector<int>& vc) {
		if (vc.size() >= 3) {
			this->r = vc[0];
			this->g = vc[1];
			this->b = vc[2];
			this->a = vc.size() >= 4 ? vc[4] : 255;
		}
		else {
			//Color();
			this->r = 0;
			this->g = 0;
			this->b = 0;
			this->a = 0;
		}
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
};


