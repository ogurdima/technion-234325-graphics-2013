#pragma once
#include <iostream>
#include "vec.h"
#include "mat.h"
#include "gl/glew.h"


using namespace std;

class Renderer
{
public:
	Renderer(int _w, int _h);
	~Renderer(void);

	void SetShadingProgram(GLuint _program);
	//bool InitModel(MeshModel m);

private:
	int deviceH;
	int deviceW;

	GLuint program;
};

