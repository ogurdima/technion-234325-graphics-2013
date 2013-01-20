#pragma once
#include <iostream>
#include <vector>
#include "vec.h"
#include "mat.h"
#include "gl/glew.h"
#include "gl/glut.h"
#include "MaterialColor.h"

using namespace std;

#define MAX_LIGHTS 30

typedef enum
{
	FLAT = 0,
	GOURAUD,
	PHONG
} ShadingType;


typedef struct 
{
	GLuint pointMat; // points transformation
	GLuint normMat; // normals transformation
	GLuint ptsHnd;
	GLuint nrmHnd;
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
	ModelBind BindModel(vector<vec4> pts, vector<vec4> normals /*also textures*/, MaterialColor c);

	CameraBind BindCamera();
	void SetUniformMatrices(vector<GLuint> handles, vector<mat4> values);
	void SetUniformMatrix(GLuint handle, mat4 val);
	void SetLightDirections(vector<vec4> lightDirections);

	void DrawTriangles(GLuint vao, int count);
	void SwapBuffers();

private:
	int deviceH;
	int deviceW;

	int program;
};

