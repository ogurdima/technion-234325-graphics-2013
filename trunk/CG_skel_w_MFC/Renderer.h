#pragma once
#include <iostream>
#include <vector>
#include "vec.h"
#include "mat.h"
#include "gl/glew.h"
#include "gl/glut.h"


using namespace std;

typedef struct 
{
	GLuint modelHnd;
	GLuint ptsHnd;
	GLuint vao;
} ModelBind;

typedef struct 
{
	GLuint viewHnd;
	GLuint projectHnd;
} CameraBind;

class Renderer
{
public:
	Renderer(int _w, int _h);
	~Renderer(void);

	void SetShadingProgram(GLuint _program);
	ModelBind BindModel(vector<vec4> pts, vector<vec4> normals /*also textures*/);
	CameraBind BindCamera();
	void SetUniformMatrices(vector<GLuint> handles, vector<mat4> values);
	void SetUniformMatrix(GLuint handle, mat4 val);

	void DrawTriangles(GLuint vao, int count);
	void SwapBuffers();

private:
	int deviceH;
	int deviceW;

	int program;
};

