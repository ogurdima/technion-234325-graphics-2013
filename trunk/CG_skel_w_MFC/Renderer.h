#pragma once
#include <iostream>
#include <vector>
#include "vec.h"
#include "mat.h"
#include "gl/glew.h"
#include "gl/glut.h"
#include "MaterialColor.h"
#include "MeshModel.h"
#include "Binds.h"
#include "Texture.h"

using namespace std;

#define MAX_LIGHTS 30

typedef enum
{
	FLAT = 0,
	GOURAUD,
	PHONG,
	TOON,
	SILHOUETTE,
	LINE,
	ENV,
} ShadingType;

typedef struct 
{
	GLuint		viewLoc;
	GLuint		projectionLoc;
	GLuint		modelLoc; 
	GLuint		normalTransformLoc; 
	GLuint		emissiveLoc;
	GLuint		ambientLoc;
	GLuint		specularLoc;
	GLuint		diffuseLoc;
	GLuint		shininessLoc;
	GLuint		samplerLoc;
	GLuint		useTexLoc;
	GLuint		envCubeMapLoc;
} UniformLocations;

class Renderer
{
public:
	Renderer(int _w, int _h);
	~Renderer(void);

	void			BindModel(MeshModel* model);

	//ModelBind		BindModel(vector<vec4> p, vector<vec4> n, vector<vec2> textures);
	void			BindTexture(MeshModel* m, Texture& t);
	void			BindEnvTexture(MeshModel* m, vector<Texture>& t);
	void			UnbindModel(MeshModel* mb);
	void			SetModelUniforms(MeshModel* m);
	void			SetModelvao(MeshModel* m);
	//void			RebindModelUniforms(ModelBind* mb);

	void			SetTexture(GLuint handle);

	void			SetUniformMatrices(vector<GLuint> handles, vector<mat4> values);
	void			SetUniformMatrix(GLuint handle, mat4 val);
	void			SetUniformVec3(GLuint handle, vec3 val);
	void			SetUniformVec2(GLuint handle, vec2 val);
	void			SetUniform1b(GLuint handle, bool val);
	void			SetUniform1i(GLuint handle, int i);

	void			SetUniform(GLuint handle, float val);
	void			SetParallelLights(vector<vec4> lightDirections, vector<vec3> lightColors);
	void			SetPointLights(vector<vec4> lightPositions, vector<vec3> lightColors);

	//void			DrawParallelSource(Rgb col, vec4 dir, mat4 toScreen);
	void			DrawModel(MeshModel* m);
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
	int						deviceH;
	int						deviceW;

	ShadingType				shading;
	GLuint					oglPrograms[7];
	UniformLocations		uLoc;
	
	void			SetUniformLocations();
	GLuint			BindLineBuffer(vector<vec4> verteces, vector<vec3> colors);
	void			SwapBuffers();
};

