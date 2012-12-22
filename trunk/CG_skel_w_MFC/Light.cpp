#include "StdAfx.h"
#include "Light.h"


Light::Light(LightType _lightType, LightSource _lightSource, vec4 _location, Rgb _lightColor) :
	lightType(_lightType), lightSource(_lightSource), location(_location), lightColor(_lightColor)
{

}

Light::Light(const Light& l)
{
	lightType = l.lightType;
	lightColor = l.lightColor;
	lightSource = l.lightSource;
	location = l.location;
}


Light::~Light(void)
{
}


