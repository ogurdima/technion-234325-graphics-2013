#include "StdAfx.h"
#include "Renderer.h"


Renderer::Renderer(int _w, int _h) :
deviceW(_w),
deviceH(_h)
{
	cout << "Renderer::Renderer: " << deviceW << "x" << deviceH << endl;
}


Renderer::~Renderer(void)
{
}


void Renderer::SetShadingProgram(GLuint _program)
{
	program = _program;
}

//bool Renderer::InitModel(MeshModel m)
//{
//	GLuint vao;
//	return false;
//}