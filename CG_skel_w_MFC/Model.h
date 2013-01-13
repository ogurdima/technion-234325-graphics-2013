#pragma once
#include <vector>
#include "Renderer.h"
#include "Vertex.h"
#include "Rgb.h"


using namespace std;
class Model {
protected:
	
public:
	virtual void draw(Renderer * r = NULL)=0;
	virtual vec3 origin() = 0;
	virtual ~Model() {}
};

