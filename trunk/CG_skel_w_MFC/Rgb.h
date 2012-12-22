#pragma once

#include "mat.h"
#include <WinDef.h>

struct Rgb {
	Rgb() : r(0), g(0), b(0) { }

	Rgb(float _r, float _g, float _b) {
		_r = abs(_r);
		_g = abs(_g);
		_b = abs(_b);
		float max = max(1.0, max(_r, max(_g,_b) ) );
		r = _r/max;
		g = _g/max;
		b = _b/max;
	}

	Rgb operator+(const Rgb& rhs) {
		return Rgb(r+rhs.r, g+rhs.g, b+rhs.b);
	}

	Rgb operator*(float f) {
		return Rgb(r*f, g*f, b*f);
	}

	float r;
	float g;
	float b;
};