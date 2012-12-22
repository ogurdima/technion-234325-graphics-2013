#pragma once

#include "vec.h"
#include "Rgb.h"

enum LightSource { POINT_S, PARALLEL_S, AREA_S};
enum LightType { AMBIENT_L, REGULAR_L};

class Light
{
	LightType lightType;
	LightSource lightSource;
	vec4 location;
	Rgb lightColor;
	float lightIntencity;


public:
	Light(void);
	~Light(void);
};
