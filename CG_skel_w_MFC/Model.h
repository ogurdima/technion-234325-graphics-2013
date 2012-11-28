#pragma once
#include "Renderer.h"
#include "Vertex.h"
using namespace std;
class Model {
protected:
	virtual ~Model() {}
public:
	void virtual draw(Renderer * r = NULL, Rgb color = Rgb(0.7,0.7,0.7) )=0;
	vector<vec3> virtual coordinates()= 0;
	vec3 virtual origin() = 0;
	void virtual addLeftWorldTransformation(mat4 transform)=0;
	virtual void drawBoundingBox(Renderer * r = NULL, Rgb color = Rgb(0.4,0.4,0.4) ) = 0;
	void virtual drawNormals(Renderer * r, float normalLength = 1, Rgb color = Rgb(0.7,0.7,0.7)) = 0;
};

