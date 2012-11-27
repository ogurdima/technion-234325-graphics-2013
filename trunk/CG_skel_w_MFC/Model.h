#pragma once
#include "Renderer.h"
using namespace std;
class Model {
protected:
	virtual ~Model() {}
public:
	void virtual draw(Renderer * r = NULL, Rgb color = Rgb(0.7,0.7,0.7) )=0;
	vector<vec4> virtual coordinates()= 0;
	vec4 virtual origin() = 0;
	void virtual addLeftWorldTransformation(mat4 transform)=0;
};

