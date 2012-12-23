#pragma once

#include "Rgb.h"

class MaterialColor
{
public:
	MaterialColor():
		ambient(0.2,0.2,0.2),
		diffuse(0.6,0.6,0),
		emissive(0.1,0.1,0.1),
		specular(0.9,0.9,0.9)
		{}
	Rgb ambient;
	Rgb diffuse;
	Rgb emissive;
	Rgb specular;

};

