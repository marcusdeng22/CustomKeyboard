#pragma once

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