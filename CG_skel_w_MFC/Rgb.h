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

	vec3 toHSL()
	{
		float max = max(r, max(g,b));
		float min = min(r, min(g,b));
		float h = (max + min) / 2;
		float s = (max + min) / 2;
		float l = (max + min) / 2;

		if(max == min)
		{
			h = s = 0; // achromatic
		}
		else
		{
			float d = max - min;
			s = l > 0.5 ? d / (2 - max - min) : d / (max + min);
			if( max == r ) 
				h = (g - b) / d + (g < b ? 6 : 0);
			else if( max == g)
				h = (b - r) / d + 2;
			else if( max == b)
				h = (r - g) / d + 4;
			else
			{
				bool Ok = true;
			}
		}
		h /= 6;
		return vec3(h, s, l);
	}

	static void HSLtoRGB_Subfunction(unsigned int& c, const float& temp1, const float& temp2, const float& temp3)
	{
		if((temp3 * 6) < 1)
			c = (unsigned int)((temp2 + (temp1 - temp2)*6*temp3)*100);
		else
			if((temp3 * 2) < 1)
				c = (unsigned int)(temp1*100);
			else
				if((temp3 * 3) < 2)
					c = (unsigned int)((temp2 + (temp1 - temp2)*(.66666 - temp3)*6)*100);
				else
					c = (unsigned int)(temp2*100);
		return;
	}

	void setHSL(vec3 hsl)
	{
		unsigned int _r = 0;
		unsigned int _g = 0;
		unsigned int _b = 0;

		float L = hsl.z;
		float S = hsl.y;
		float H = hsl.x;

		if(S == 0)
		{
			r = L;
			g = L;
			b = L;
			return;
		}
		else
		{
			float temp1 = 0;
			if(L < .50)
			{
				temp1 = L*(1 + S);
			}
			else
			{
				temp1 = L + S - (L*S);
			}

			float temp2 = 2*L - temp1;

			float temp3 = 0;
			for(int i = 0 ; i < 3 ; i++)
			{
				switch(i)
				{
				case 0: // red
					{
						temp3 = H + .33333f;
						if(temp3 > 1)
							temp3 -= 1;
						HSLtoRGB_Subfunction(_r,temp1,temp2,temp3);
						break;
					}
				case 1: // green
					{
						temp3 = H;
						HSLtoRGB_Subfunction(_g,temp1,temp2,temp3);
						break;
					}
				case 2: // blue
					{
						temp3 = H - .33333f;
						if(temp3 < 0)
							temp3 += 1;
						HSLtoRGB_Subfunction(_b,temp1,temp2,temp3);
						break;
					}
				default:
					{

					}
				}
			}
		}
		r = ((float)_r)/100;
		g = ((float)_g)/100;
		b = ((float)_b)/100;
	}

	float r;
	float g;
	float b;


};

