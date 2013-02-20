#pragma once

#include "Rgb.h"

class MaterialColor
{
public:
	MaterialColor():
		ambient(0.2,0.2,0.2),
		diffuse(0.7,0.7,0.7),
		emissive(0,0,0),
		specular(1,1,1),
		shininess(4)
		{}
	Rgb ambient;
	Rgb diffuse;
	Rgb emissive;
	Rgb specular;
	int shininess;

};

