#pragma once
#include <vector>
#include "CG_skel_w_MFC.h"
#include "vec.h"
#include "mat.h"
#include "GL/glew.h"
#include "Camera.h"
#include "Vertex.h"
#include "Rgb.h"
#include "Light.h"
#include "MaterialColor.h"

using namespace std;

enum ShadingType {SHADING_FLAT, SHADING_GOURARD, SHADING_PHONG, SHADING_WIREFRAME };

class Renderer
{
	float *m_outBuffer; // 3*width*height
	float *m_zbuffer;	// width*height
	int m_width, m_height;
	Camera * m_camera;
	Rgb m_bg;
	int* contourX;
	vector<Light*> m_lights;
	int m_specularPower;
	ShadingType m_shadingType;
	Rgb m_fogColor;
	bool m_fogEffect;
	
	float *m_colorBuf;
	int m_bufW, m_bufH;
	int m_aaX;


	//////////////////////////////
	// openGL stuff. Don't touch.
	GLuint gScreenTex;
	GLuint gScreenVtc;
	GLuint gBuffer;
	void CreateOpenGLBuffer();
	void InitOpenGLRendering();
	//////////////////////////////


	
	Rgb GetColor(Vertex at, vec4 n, vec4 camLoc, MaterialColor material);

	void DrawTriangle2D(vec2 v1, vec2 v2, vec2 v3, Rgb col = Rgb(1,1,1) );
	void DrawLine(vec2 p1, vec2 p2, Rgb col = Rgb(1,1,1));
	void DrawLine(vec4 p1, vec4 p2, Rgb col = Rgb(1,1,1));
	void PlotPixel(int x, int y, Rgb color = Rgb(1,1,1));
	vec2 ScaleFactor();
	bool clip(vec3& v1, vec3& v2);
	bool clip(vec4& v1, vec4& v2);

public:
	Renderer(Rgb bg = Rgb(1,1,1));
	Renderer(int width, int height, Rgb bg = Rgb(1,1,1));
	~Renderer(void);
	void Init();
	void SetLights(vector<Light*> lights);
	void SetShading(ShadingType t);
	//--------------------------------------------------------------------
	// Buffer stuff
	//--------------------------------------------------------------------
	void CreateBuffers(int width, int height);
	void CreateLocalBuffer();
	void SwapBuffers();
	void FlushBuffer();
	void ClearColorBuffer();
	void ClearDepthBuffer();

	void SetAntiAliasing(int x);
	void AliasBufToOutputBuf();

	//--------------------------------------------------------------------
	// Drawing stuff
	//--------------------------------------------------------------------
	void DrawNgons(vector<Vertex>& vertices, int n, Rgb color = Rgb(1,1,1));
	void DrawNgonsFast(vector<Vertex>& vertices, int n, Rgb color = Rgb(1,1,1));
	void DrawNgonsSlow(vector<Vertex>& vertices, int n, Rgb color = Rgb(1,1,1));

	void DrawTriangles(vector<Vertex>& vertices);
	void DrawTriangle(vec4& v1, vec4& v2, vec4& v3);
	void RasterizeTriangle(vec4 v1, vec4 v2, vec4 v3);
	void ScanLeft(vec4 v1,vec4 v2,int* contourX );
	void ScanRight(vec4 v1,vec4 v2,int* contourX );

	// new
	Vertex projectedToDisplay(Vertex v);
	void fullTriangle(Vertex v1, Vertex v2, Vertex v3);
	void DDrawTriangles(vector<Vertex>& vertices, MaterialColor defaultColor, vector<vec4>& vertexNormals = vector<vec4>(), vector<MaterialColor> vertexColors = vector<MaterialColor>());
	
	void PlotPixel(int x, int y, float z, Rgb color);

	void FlatRasterizePolygon(vector<Vertex>& poly, Rgb color);
	void GouraudRasterizePolygon(vector<Vertex>& poly, vector<Rgb>& colors);
	void PhongRasterizePolygon(vector<Vertex>& poly, vector<MaterialColor>& colors, vector<vec4>& normals);

	void GetDrawablePoly(vector<Vertex>& vertices, vector<Vertex>& outPoly);
	void GetDrawablePoly(vector<Vertex>& vertices, vector<Rgb>& colors, vector<Vertex>& outPoly, vector<Rgb>& outColors);
	void GetDrawablePoly(vector<Vertex>& vertices, vector<MaterialColor>& colors, vector<vec4>& normals, vector<Vertex>& outPoly, vector<MaterialColor>& outColors, vector<vec4>& outNormals);

	//lines
	void DrawLine3D(vec3 v1, vec3 v2, Rgb col = Rgb(1,1,1));
	void DrawVisibleBoundary();

	//--------------------------------------------------------------------
	// Camera stuff
	//--------------------------------------------------------------------
	void SetCamera(Camera* c);

};
