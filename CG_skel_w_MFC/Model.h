#pragma once
#include <vector>
#include "Vertex.h"
#include "Rgb.h"


using namespace std;
class Model {
protected:
	
public:
	//virtual void Draw(Renderer * r = NULL)=0;
	virtual vec3 Origin() = 0;
	virtual ~Model() {}
};

