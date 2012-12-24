#include "StdAfx.h"
#include "Light.h"


Light::Light(LightType _lightType, LightSource _lightSource, vec4 _location, Rgb _lightColor, vec4 _direction) :
	lightType(_lightType), lightSource(_lightSource), location(_location), lightColor(_lightColor), direction(_direction)
{

}

Light::Light(const Light& l)
{
	lightType = l.lightType;
	lightColor = l.lightColor;
	lightSource = l.lightSource;
	location = l.location;
	direction = l.direction;
}


Light::~Light(void)
{
}


