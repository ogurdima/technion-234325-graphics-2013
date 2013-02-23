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
	GLuint		useNormalMapLoc;
	GLuint		normalMapLoc;
	GLuint		useVertexAnimation;
	GLuint		vertexAnimationParamLoc;
} UniformLocations;

class Renderer
{
public:
	Renderer(int _w, int _h);
	~Renderer(void);

	void			SetShading(ShadingType _type);
	ShadingType		Shading();

	void			BindModel(MeshModel* model);
	void			UnbindModel(MeshModel* mb);
	void			DrawModel(MeshModel* m);
	void			BindTexture(MeshModel* m, Texture& t);
	void			BindNormalTexture(MeshModel* m, Texture& t);
	void			CopyFrameToTexture(GLenum dir, MeshModel* m);

	void			DrawWFLines(vector<vec4> verteces, vector<vec3> colors);
	
	void			InitDraw();
	void			SetCamera(mat4 view, mat4 projection);
	void			SetParallelLights(vector<vec4> lightDirections, vector<vec3> lightColors);
	void			SetPointLights(vector<vec4> lightPositions, vector<vec3> lightColors);
	void			FinishShading();
	void			FinishDraw();

	void			EnableFrontFaceCull();
	void			DisableFrontFaceCull();
	void			ToggleAntialiasing();

private:
	int						deviceH;
	int						deviceW;

	ShadingType				shading;
	GLuint					oglPrograms[7];
	UniformLocations		uLoc;
	
	void			InitShaders();
	void			SaveUniformLocations();
	GLuint			BindLineBuffer(vector<vec4> verteces, vector<vec3> colors);
	void			SwapBuffers();

	void			SetModelUniforms(MeshModel* m);
	void			SetModelvao(MeshModel* m);
	void			DrawTriangles(GLuint vao, int count);

	// deprecated
	void			SetTexture(GLuint handle);
	void			SetUniformMatrices(vector<GLuint> handles, vector<mat4> values);
	void			SetUniformMatrix(GLuint handle, mat4 val);
	void			SetUniformVec3(GLuint handle, vec3 val);
	void			SetUniformVec2(GLuint handle, vec2 val);
	void			SetUniform1b(GLuint handle, bool val);
	void			SetUniform1i(GLuint handle, int i);
	void			SetUniform(GLuint handle, float val);
	void			DrawSilhouette(GLuint vao, int count);
};

