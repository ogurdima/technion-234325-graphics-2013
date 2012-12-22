#pragma once

#include "vec.h"
#include "Rgb.h"

enum LightSource { POINT_S, PARALLEL_S, AREA_S};
enum LightType { AMBIENT_L, REGULAR_L};

class Light
{
public:
	LightType lightType;
	LightSource lightSource;
	vec4 location;
	Rgb lightColor;



	Light(LightType _lightType, LightSource _lightSource, vec4 _location, Rgb _lightColor);
	Light(const Light& l);
	~Light(void);
};
