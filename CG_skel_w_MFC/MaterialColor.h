#pragma once

#include "Rgb.h"

class MaterialColor
{
public:
	MaterialColor():
		ambient(0,0,0),
		diffuse(0,0,0),
		emissive(0.2,0.2,0),
		specular(0,0,0)
		{}
	Rgb ambient;
	Rgb diffuse;
	Rgb emissive;
	Rgb specular;

};

