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
	PHONG,
	TOON,
	SILHOUETTE,
	LINE
} ShadingType;

typedef struct 
{
	GLuint		modelLoc; // points transformation
	GLuint		normalTransformLoc; // normals transformation
	GLuint		emissiveLoc;
	GLuint		ambientLoc;
	GLuint		specularLoc;
	GLuint		diffuseLoc;
	GLuint		shininessLoc;
	GLuint		vao;
	int			size;
	GLuint*		buffers;
} ModelBind;

typedef struct 
{
	GLuint viewHnd;
	GLuint projectHnd;
} CameraBind;

class VaoBinding
{
public:
	GLuint vao;
	GLsizei size;
	GLuint * buffers;
};

class Renderer
{
public:
	Renderer(int _w, int _h);
	~Renderer(void);

	ModelBind		BindModel(vector<vec4> p, vector<vec4> n);
	void			RebindModelUniforms(ModelBind* mb);

	void			SetUniformMatrices(vector<GLuint> handles, vector<mat4> values);
	void			SetUniformMatrix(GLuint handle, mat4 val);
	void			SetUniformVec3(GLuint handle, vec3 val);
	void			SetUniform(GLuint handle, float val);
	void			SetParallelLights(vector<vec4> lightDirections, vector<vec3> lightColors);
	void			SetPointLights(vector<vec4> lightPositions, vector<vec3> lightColors);

	//void			DrawParallelSource(Rgb col, vec4 dir, mat4 toScreen);

	void			DrawTriangles(GLuint vao, int count);
	void			DrawSilhouette(GLuint vao, int count);
	void			DrawWFLines(vector<vec4> verteces, vector<vec3> colors);
	void			ToggleAntialiasing();

	void			InitDraw(); // must
	void			SetCamera(mat4 view, mat4 projection);
	void			FinishShading(); // optional
	void			FinishDraw(); // must

	void			EnableFrontFaceCull();
	void			DisableFrontFaceCull();

	void			InitShaders();
	
	void			SetShading(ShadingType _type);
	ShadingType		Shading();

private:
	int deviceH;
	int deviceW;

	ShadingType				shading;
	GLuint					oglPrograms[6];
	

	GLuint			BindLineBuffer(vector<vec4> verteces, vector<vec3> colors);
	void			SwapBuffers();
};

