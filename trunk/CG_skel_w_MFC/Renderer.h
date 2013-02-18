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

	ModelBind		BindModel(vector<vec4> pts, vector<vec4> normals /*also textures*/, MaterialColor c);

	//CameraBind		BindCamera();
	void			SetUniformMatrices(vector<GLuint> handles, vector<mat4> values);
	void			SetUniformMatrix(GLuint handle, mat4 val);
	void			SetParallelLights(vector<vec4> lightDirections, vector<vec3> lightColors);
	void			SetPointLights(vector<vec4> lightPositions, vector<vec3> lightColors);

	//void			DrawParallelSource(Rgb col, vec4 dir, mat4 toScreen);

	void			DrawTriangles(GLuint vao, int count);
	void			DrawWFLines(vector<vec4> verteces, vector<vec3> colors);
	void			SwapBuffers();

	void			ToggleAntialiasing();

	void			InitDraw(mat4 view, mat4 projection);
	void			FinishDraw();

	void			InitShaders();
	void			SetCameraMatices();
	void			SetLights();
	GLuint			BindLineBuffer(vector<vec4> verteces, vector<vec3> colors);
	void			SetShading(ShadingType _type);
	ShadingType		Shading();

private:
	int deviceH;
	int deviceW;

	ShadingType				shading;
	GLuint					oglPrograms[3];
	GLuint					oglLineProgram;
	CameraBind				oglCameraBind;
};

