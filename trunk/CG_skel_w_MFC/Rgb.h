#pragma once

#include "mat.h"
#include "vec.h"


struct Rgb {
	Rgb() : r(0), g(0), b(0) { }

	explicit Rgb(float _r, float _g, float _b) {
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

	Rgb operator*(Rgb& rhs) {
		return Rgb(r*rhs.r, g*rhs.g, b*rhs.b);
	}

	Rgb& operator+=(Rgb& rhs) {
		r += rhs.r;
		g+= rhs.g;
		b+=rhs.b;
		float max = max(1.0, max(r, max(g,b) ) );
		r = r/max;
		g = g/max;
		b = b/max;
		return *this;
	}

	vec3 toVec3()
	{
		return vec3(r, g, b);
	}

	float r;
	float g;
	float b;


};

