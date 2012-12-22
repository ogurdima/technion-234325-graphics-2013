#pragma once

#include "vec.h"
#include "Rgb.h"

enum LightType { POINT_L, PARALLEL_L, AREA_L};

class Light
{
	LightType lightType;
	vec4 location;
	Rgb lightColor;

public:
	Light(void);
	~Light(void);
};
