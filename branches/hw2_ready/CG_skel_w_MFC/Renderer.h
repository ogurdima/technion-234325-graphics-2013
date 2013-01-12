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
	float *m_zbuffer;	// m_bufW*m_bufH
	float *m_colorBuf;	// 3*m_bufW*m_bufH
	int m_width, m_height, m_bufW, m_bufH, m_aaX, m_specularPower;
	Camera * m_camera;
	vector<Light*> m_lights;
	ShadingType m_shadingType;
	Rgb m_bg;
	Rgb m_fogColor;
	bool m_fogEffect;

	//////////////////////////////
	// openGL stuff. Don't touch.
	GLuint gScreenTex;
	GLuint gScreenVtc;
	GLuint gBuffer;
	void CreateOpenGLBuffer();
	void InitOpenGLRendering();
	//////////////////////////////

	//--------------------------------------------------------------------
	// Drawing helpers
	//--------------------------------------------------------------------
	void DrawNgonsSlow(vector<Vertex>& vertices, int n, Rgb color = Rgb(1,1,1));

	//--------------------------------------------------------------------
	// Clipping related
	//--------------------------------------------------------------------
	bool clip(vec3& v1, vec3& v2);
	bool clip(vec4& v1, vec4& v2);
	void GetDrawablePoly(vector<Vertex>& vertices, vector<Vertex>& outPoly);
	void GetDrawablePoly(vector<Vertex>& vertices, vector<Rgb>& colors, vector<Vertex>& outPoly, vector<Rgb>& outColors);
	void GetDrawablePoly(vector<Vertex>& vertices, vector<MaterialColor>& colors, vector<vec4>& normals, vector<Vertex>& outPoly, vector<MaterialColor>& outColors, vector<vec4>& outNormals);

	//--------------------------------------------------------------------
	// Rasterizing and Fragment processing related
	//--------------------------------------------------------------------
	Rgb GetColor(Vertex at, vec4 n, vec4 camLoc, MaterialColor material);
	void FlatRasterizePolygon(vector<Vertex>& poly, Rgb color);
	void GouraudRasterizePolygon(vector<Vertex>& poly, vector<Rgb>& colors);
	void PhongRasterizePolygon(vector<Vertex>& poly, vector<MaterialColor>& colors, vector<vec4>& normals);
	void DrawLine(vec2 p1, vec2 p2, Rgb col = Rgb(1,1,1));
	void DrawLine(vec4 p1, vec4 p2, Rgb col = Rgb(1,1,1));
	void PlotPixel(int x, int y, Rgb color = Rgb(1,1,1));
	void PlotPixel(int x, int y, float z, Rgb color);

	//--------------------------------------------------------------------
	// Buffers stuff
	//--------------------------------------------------------------------
	void CreateLocalBuffer();
	void FlushBuffers();
	void ClearColorBuffer();
	void ClearDepthBuffer();
	void AliasBufToOutputBuf();

	//--------------------------------------------------------------------
	// Misc
	//--------------------------------------------------------------------
	vec2 ScaleFactor();
	Vertex projectedToDisplay(Vertex v);

public:
	//--------------------------------------------------------------------
	// General interface
	//--------------------------------------------------------------------
	Renderer(Rgb bg = Rgb(1,1,1));
	Renderer(int width, int height, Rgb bg = Rgb(1,1,1));
	~Renderer(void);
	void Init();
	Rgb GetBackgroundColor() { return m_bg; }
	void SetBackgroundColor(Rgb col) { m_bg = col; SwapBuffers(); }

	//--------------------------------------------------------------------
	// Shading and rendering
	//--------------------------------------------------------------------
	void SetCamera(Camera* c);
	void SetLights(vector<Light*> lights);
	void SetShading(ShadingType t);
	void SetAntiAliasing(int x);
	void SetFogColor(Rgb col);
	void ToggleFog();
	
	//--------------------------------------------------------------------
	// Buffers stuff
	//--------------------------------------------------------------------
	void CreateBuffers(int width, int height);
	void SwapBuffers();
	
	//--------------------------------------------------------------------
	// Drawing stuff
	//--------------------------------------------------------------------
	// wireframe
	void DrawLine3D(vec3 v1, vec3 v2, Rgb col = Rgb(1,1,1));
	void DrawNgons(vector<Vertex>& vertices, int n, Rgb color = Rgb(1,1,1));
	// full
	void DrawTriangles(vector<Vertex>& vertices, MaterialColor defaultColor, vector<vec4>& vertexNormals = vector<vec4>(), vector<MaterialColor> vertexColors = vector<MaterialColor>());
};
