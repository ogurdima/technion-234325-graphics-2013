#pragma once
#include <vector>
#include "CG_skel_w_MFC.h"
#include "vec.h"
#include "mat.h"
#include "GL/glew.h"
#include "Camera.h"
#include "Vertex.h"

using namespace std;
class Renderer
{
	float *m_outBuffer; // 3*width*height
	float *m_zbuffer;	// width*height
	int m_width, m_height;
	Camera * m_camera;

	//////////////////////////////
	// openGL stuff. Don't touch.
	GLuint gScreenTex;
	GLuint gScreenVtc;
	GLuint gBuffer;
	void CreateOpenGLBuffer();
	void InitOpenGLRendering();
	//////////////////////////////

	void CreateBuffers(int width, int height);
	void CreateLocalBuffer();

public:
	Renderer();
	Renderer(int width, int height);
	~Renderer(void);
	void Init();
	//--------------------------------------------------------------------
	// Buffer stuff
	//--------------------------------------------------------------------
	void SwapBuffers();
	void FlushBuffer();
	void ClearColorBuffer();
	void ClearDepthBuffer();
	void SetDemoBuffer();

	//--------------------------------------------------------------------
	// Transformation stuff
	//--------------------------------------------------------------------
	vec2 ProjectPoint(vec3 p);
	vec2 ProjectPoint(vec4 p);
	vec3 ObjectToCamera(vec4 p);
	

	//--------------------------------------------------------------------
	// Drawing stuff
	//--------------------------------------------------------------------
	void DrawTriangle2D(vec2 v1, vec2 v2, vec2 v3);
	void DrawTriangles(const vector<vec3>* vertices, const vector<vec3>* normals=NULL);
	void Draw(const vector<Vertex> vertices);
	void DrawOld(vector<Vertex> vertices);
	void DrawLine(vec2 p1, vec2 p2);
	void DrawLine3D(vec4 v1, vec4 v2);
	void plotPixel(int x, int y);
	//--------------------------------------------------------------------
	// Camera stuff
	//--------------------------------------------------------------------
	void SetCameraTransform(const mat4& cTransform);
	void SetProjection(const mat4& projection);
	void SetCamera(Camera* c);

	//--------------------------------------------------------------------
	// Models stuff
	//--------------------------------------------------------------------
	void SetObjectMatrices(const mat4& oTransform, const mat3& nTransform);
	
};
