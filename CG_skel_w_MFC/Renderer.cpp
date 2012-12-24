#include "StdAfx.h"
#include "Renderer.h"
#include "CG_skel_w_MFC.h"
#include "InitShader.h"
#include "GL\freeglut.h"
#include <assert.h>

#define INDEX(width,x,y,c) (x+y*width)*3+c

bool clipZ(vec4& v1, vec4& v2, float zLow , float zHigh);

Renderer::Renderer(Rgb bg) :
m_width(512), 
	m_height(512), 
	m_outBuffer(NULL),
	m_zbuffer(NULL),
	m_bg(bg),
	m_specularPower(5),
	m_shadingType(SHADING_FLAT)
{
	InitOpenGLRendering();
	CreateBuffers(512,512);
}

Renderer::Renderer(int width, int height, Rgb bg) :
m_width(width), 
	m_height(m_height), 
	m_outBuffer(NULL),
	m_zbuffer(NULL),
	contourX(NULL),
	m_bg(bg),
	m_specularPower(5),
	m_shadingType(SHADING_FLAT)
{
	InitOpenGLRendering();
	CreateBuffers(width, height);
}

Renderer::~Renderer(void)
{
	if (m_outBuffer != NULL) {
		delete m_outBuffer;
		m_outBuffer = NULL;
	}

	if (m_zbuffer != NULL) {
		delete m_zbuffer;
		m_zbuffer = NULL;
	}

}

//--------------------------------------------------------------------------
// Buffers
//--------------------------------------------------------------------------
void Renderer::CreateBuffers(int width, int height)
{
	m_width=width;
	m_height=height;	
	CreateOpenGLBuffer(); //Do not remove this line.
	if (m_outBuffer != NULL) {
		delete m_outBuffer;
		m_outBuffer = NULL;
	}
	m_outBuffer = new float[3*m_width*m_height];
	if (m_zbuffer != NULL) {
		delete m_zbuffer;
		m_zbuffer = NULL;
	}
	m_zbuffer = new float[m_width*m_height];
	if (contourX != NULL) {
		delete[] contourX;
		contourX = NULL;
	}
	contourX = new int[m_height * 2];
	FlushBuffer();
}

void Renderer::ClearDepthBuffer()
{
	for(int i=0; i<m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			m_zbuffer[j + i*m_width] = -100500;
		}
	}
}

void Renderer::FlushBuffer()
{
	ClearColorBuffer();
	ClearDepthBuffer();
}

void Renderer::ClearColorBuffer() 
{
	for(int i=0; i<m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			m_outBuffer[INDEX(m_width,j,i,0)]=m_bg.r;	m_outBuffer[INDEX(m_width,j,i,1)]=m_bg.g;	m_outBuffer[INDEX(m_width,j,i,2)]=m_bg.b;
		}
	}
}

//--------------------------------------------------------------------------
// Drawing
//--------------------------------------------------------------------------
void Renderer::DrawNgons(vector<Vertex>& vertices, int n, Rgb color) 
{
	//DrawNgonsFast(vertices, n, color);
	DrawNgonsSlow(vertices, n, color);
}

void Renderer::DrawNgonsSlow(vector<Vertex>& vertices, int n, Rgb color) 
{
	if (n < 2) return;
	mat4 projection = m_camera->Projection();
	mat4 view = m_camera->View();
	float zHigh = -m_camera->ZNear();
	float zLow = -m_camera->ZFar();
	Vertex cur, next;
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( (i+1) % n == 0 && (i+1) >= n) //draw srarting from current n vertices back
		{

			int startIdx = i-n+1;
			for (int j = 0; j < n; j++) {
				// cur and next are copied by value - the array does not change
				cur = view * vertices[startIdx + j];
				next = view * vertices[startIdx + ((j+1)%n)];
				// cur and next from here are in camera coordinates
				if ( clipZ(cur, next, zLow, zHigh) ) 
				{
					// clipZ (maybe) changed values of cur and next, now they are between zNear and zFar
					cur = projection * cur;
					next = projection * next;
					// cur and next from here are projected after they were clipped by Z
					if( clip(cur, next) )
					{
						// clip (maybe) changed values of cur and next, now they are entirely inside canonic view volume, we can draw them
						DrawLine(cur, next, color);
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------
// Clipping
//--------------------------------------------------------------------------
bool Renderer::clip(vec3& v1, vec3& v2)
{
	// x
	if( (v1.x > 1 && v2.x > 1) || (v1.x < -1 && v2.x < -1) )
		return false;

	if( v1.x != v2.x && !( v1.x >= -1 && v1.x <= 1 && v2.x >= -1 && v2.x <= 1) )
	{
		if(v1.x > v2.x)
		{
			vec3 tmp = v1;
			v1 = v2;
			v2 = tmp;
		}
		if(v1.x < -1)
		{
			vec3 k = v2 - v1;
			float t = ( -1 - v1.x) / k. x;
			v1 = v1 + t * k;
		}
		if(v2.x > 1)
		{
			vec3 k = v2 - v1;
			float t = ( 1 - v1.x) / k. x;
			v2 = v1 + t * k;
		}
	}

	// y
	if( (v1.y > 1 && v2.y > 1) || (v1.y < -1 && v2.y < -1) )
		return false;
	if( v1.y != v2.y && !( v1.y >= -1 && v1.y <= 1 && v2.y >= -1 && v2.y <= 1) )
	{
		if(v1.y > v2.y)
		{
			vec3 tmp = v1;
			v1 = v2;
			v2 = tmp;
		}
		if(v1.y < -1)
		{
			vec3 k = v2 - v1;
			float t = ( -1 - v1.y) / k. y;
			v1 = v1 + t * k;
		}
		if(v2.y > 1)
		{
			vec3 k = v2 - v1;
			float t = ( 1 - v1.y) / k. y;
			v2 = v1 + t * k;
		}
	}

	// z
	if( (v1.z > 1 && v2.z > 1) || (v1.z < -1 && v2.z < -1) )
		return false;
	if( v1.z != v2.z && !( v1.z >= -1 && v1.z <= 1 && v2.z >= -1 && v2.z <= 1) )
	{
		if(v1.z > v2.z)
		{
			vec3 tmp = v1;
			v1 = v2;
			v2 = tmp;
		}
		if(v1.z < -1)
		{
			vec3 k = v2 - v1;
			float t = ( -1 - v1.z) / k. z;
			v1 = v1 + t * k;
		}
		if(v2.z > 1)
		{
			vec3 k = v2 - v1;
			float t = ( 1 - v1.z) / k. z;
			v2 = v1 + t * k;
		}
	}
	return true;
}

bool Renderer::clip(vec4& v1, vec4& v2)
{
	assert(v1.w != 0 && v2.w != 0);
	vec3 vv1 = vec3(v1.x,v1.y, v1.z) / v1.w;
	vec3 vv2 = vec3(v2.x,v2.y, v2.z) / v2.w;
	bool res = clip(vv1, vv2);
	if( res )
	{
		v1 = vec4(vv1, 1);
		v2 = vec4(vv2, 1);
	}
	return res;
}

bool clipZ(vec4& v1, vec4& v2, float zLow , float zHigh)
{
	v1 = v1 / v1.w;
	v2 = v2 / v2.w;

	if( (v1.z > zHigh && v2.z > zHigh) || (v1.z < zLow && v2.z < zLow) )
		return false;
	if( v1.z != v2.z && !( v1.z >= zLow && v1.z <= zHigh && v2.z >= zLow && v2.z <= zHigh) )
	{
		
		if(v1.z < zLow)
		{
			vec4 k = v2 - v1;
			k.w = 0;
			float t = ( zLow - v1.z) / k. z;
			v1 = v1 + t * k;

		}
		if(v2.z > zHigh)
		{
			vec4 k = v2 - v1;
			k.w = 0;
			float t = ( zHigh - v1.z) / k. z;
			v2 = v1 + t * k;
		}

		if(v1.z > zHigh)
		{
			vec4 k = v2 - v1;
			k.w = 0;
			float t = ( zHigh - v1.z) / k. z;
			v1 = v1 + t * k;

		}
		if(v2.z < zLow)
		{
			vec4 k = v2 - v1;
			k.w = 0;
			float t = ( zLow - v1.z) / k. z;
			v2 = v1 + t * k;
		}
	}
	return true;
}

//--------------------------------------------------------------------------
// Bresenham
//--------------------------------------------------------------------------
void Renderer::DrawLine(vec2 p1, vec2 p2, Rgb col)
{
	int x1 = (int)p1.x;
	int x2 = (int)p2.x;
	int y1 = (int)p1.y;
	int y2 = (int)p2.y;

	if (	((x1 < 0 || x1 > m_width) || (y1 < 0 || y1 > m_height)) &&
		((x2 < 0 || x2 > m_width) || (y2 < 0 || y2 > m_height))	)
	{
		return;
	}

	bool steep = abs(y2 - y1) > abs(x2 - x1);

	if(steep)
	{
		swap(x1,y1);
		swap(x2,y2);
	}
	if(x1 > x2)
	{
		swap(x1,x2);
		swap(y1,y2);
	}

	int dx = x2 - x1;
	int dy = abs(y2 - y1);

	int e =  dx/2;
	int y = y1;
	int yStep = (y1 < y2) ? 1 : -1;

	for(int x = x1; x <= x2; ++x)
	{
		if(steep)
		{
			PlotPixel(y,x, col);
		}
		else
		{
			PlotPixel(x,y, col);
		}
		e -= dy;
		if(e < 0)
		{
			e += dx;
			y += yStep;
		}
	}
}

//--------------------------------------------------------------------------
// Helps to transform from view plane to device coordinates
//--------------------------------------------------------------------------
vec2 Renderer::ScaleFactor() 
{
	vec2 res;
	res.x = m_width/2;
	res.y = m_height/2;
	return res;

	float screenAR = (float) m_width / (float) m_height;
	if (m_camera->Aspect() > screenAR)
	{
		res.x = m_width/2;
		res.y = res.x / m_camera->Aspect();
	}
	else
	{
		res.y = m_height/2;
		res.x = res.y * m_camera->Aspect();
	}
	return res;
}

//--------------------------------------------------------------------------
// Bresenham wrapper for vec4. 
//--------------------------------------------------------------------------
void Renderer::DrawLine(vec4 p1, vec4 p2, Rgb col)
{
	vec2 sf = ScaleFactor();
	mat4 sp = Scale( sf.x , sf.y, 1)  * Translate(1,1,0);
	p1 = sp * p1;
	p2 = sp * p2;
	p1 = p1 / p1.w;
	p2 = p2 / p2.w;
	DrawLine(vec2(p1.x,p1.y),vec2(p2.x,p2.y),col);
}

//--------------------------------------------------------------------------
// Safely plot pixel in the given integer coordinates of the given color
//--------------------------------------------------------------------------
inline void Renderer::PlotPixel(int x, int y, Rgb color)
{
	if (x < 0 || x >= m_width || y < 0 || y >= m_height)
		return;
	m_outBuffer[INDEX(m_width,x,y,0)]=color.r;	m_outBuffer[INDEX(m_width,x,y,1)]=color.g;	m_outBuffer[INDEX(m_width,x,y,2)]=color.b;
}

void Renderer::SetCamera(Camera* c)
{
	m_camera = c;
}

void Renderer::SetShading(ShadingType t)
{
	m_shadingType = t;
}

Rgb Renderer::GetColor(Vertex at, vec4 n, vec4 camLoc, MaterialColor material)
{
	Rgb c;
	c += material.emissive;
	for(int i = 0; i < m_lights.size(); i++)
	{
		Light* light = m_lights[i];
		vec4 lloc = light->location;
		if(light->lightType == AMBIENT_L)
		{
			c += light->lightColor * material.ambient;
			continue;
		}
		if(light->lightSource == POINT_S)
		{
			vec4 incomingRay = normalize(at - lloc);
			vec4 reflectedRay =  2 * n * dot(incomingRay, n) - incomingRay;
			float coss = dot(n, incomingRay);
			if(coss < 0)
			{
				c += (light->lightColor * coss) * material.diffuse; //diffuse
			}
			float csss = dot(normalize(reflectedRay), normalize(at - camLoc));
			if(csss > 0)
			{
				float powww = pow(csss, m_specularPower);
				c += material.specular * powww;
			}
		}
	}
	return c;
}

inline float interpolate(float t, float a, float b)
{
	return a + t * (b - a);
}

inline vec4 interpolate(float t, vec4 a, vec4 b)
{
	return a + t * (b - a);
}

inline Rgb interpolate(float t, Rgb a, Rgb b)
{
	return Rgb(  a.r + t * (b.r - a.r),  a.g + t * (b.g - a.g), a.b + t * (b.b - a.b) );
}

inline MaterialColor interpolate(float t, MaterialColor a, MaterialColor b)
{
	MaterialColor mc;
	mc.ambient = interpolate(t,a.ambient,b.ambient);
	mc.diffuse = interpolate(t,a.diffuse,b.diffuse);
	mc.emissive = interpolate(t,a.emissive,b.emissive);
	mc.specular = interpolate(t,a.specular,b.specular);
	return mc;
}

void Renderer::GetDrawablePoly(vector<Vertex>& vertices, vector<Vertex>& outPoly)
{
	float zHigh = -m_camera->ZNear();
	float zLow = -m_camera->ZFar();
	mat4 view = m_camera->View();
	for( int i = 0; i < vertices.size(); ++i)
	{
		Vertex p1 = view * vertices[i];
		Vertex p2 = view * vertices[(i+1) % vertices.size()];
		if(clipZ(p1,p2,zLow,zHigh))
		{
			if (! (vertices[i].z < zHigh && vertices[i].z > zLow) ) // if not inside
			{
				outPoly.push_back(p1);
			}
			outPoly.push_back(p2);
		}
	}
	
}

void Renderer::GetDrawablePoly(vector<Vertex>& vertices, vector<Rgb>& colors, vector<Vertex>& outPoly, vector<Rgb>& outColors)
{
	float zHigh = -m_camera->ZNear();
	float zLow = -m_camera->ZFar();
	mat4 view = m_camera->View();
	for( int i = 0; i < vertices.size(); ++i)
	{
		Vertex v1 = view * vertices[i];
		Vertex v2 = view * vertices[(i+1) % vertices.size()];
		Vertex p1 = v1;
		Vertex p2 = v2;
		if(clipZ(p1,p2,zLow,zHigh))
		{
			if (! (vertices[i].z < zHigh && vertices[i].z > zLow) ) // if not inside
			{
				outPoly.push_back(p1);
				float t = ( p1.z - v1.z)  / (v2.z - v1.z);
				outColors.push_back( interpolate(t,colors[i], colors[(i+1) % vertices.size()]));
			}
			float t = ( p2.z - v1.z)  / (v2.z - v1.z);
			outColors.push_back( interpolate(t,colors[i], colors[(i+1) % vertices.size()]));
			outPoly.push_back(p2);
		}
	}
}

void Renderer::GetDrawablePoly(vector<Vertex>& vertices, vector<MaterialColor>& colors, vector<vec4>& normals, vector<Vertex>& outPoly, vector<MaterialColor>& outColors, vector<vec4>& outNormals)
{
	float zHigh = -m_camera->ZNear();
	float zLow = -m_camera->ZFar();
	mat4 view = m_camera->View();

	for( int i = 0; i < vertices.size(); ++i)
	{
		Vertex vv1 = vertices[i];
		Vertex vv2 = vertices[(i+1) % vertices.size()];
		Vertex v1 = view * vv1;
		Vertex v2 = view * vv2;
		Vertex p1 = v1;
		Vertex p2 = v2;
		if(clipZ(p1,p2,zLow,zHigh))
		{
			if (! (vertices[i].z < zHigh && vertices[i].z > zLow) ) // if not inside
			{
				
				float t = ( p1.z - v1.z)  / (v2.z - v1.z);
				outPoly.push_back(interpolate(t, vv1,vv2));
				vec4 n = normalize(interpolate(t,normals[i], normals[(i+1) % vertices.size()]));
				outNormals.push_back( interpolate(t,normals[i], normals[(i+1) % vertices.size()]));
				outColors.push_back( interpolate(t,colors[i], colors[(i+1) % vertices.size()]));
			}
			float t = ( p2.z - v1.z)  / (v2.z - v1.z);
			outPoly.push_back(interpolate(t, vv1,vv2));
			outNormals.push_back( interpolate(t,normals[i], normals[(i+1) % vertices.size()]));
			outColors.push_back( interpolate(t,colors[i], colors[(i+1) % vertices.size()]));
		}
	}
}

// New
void Renderer::DDrawTriangles(vector<Vertex>& vertices, MaterialColor defaultColor, vector<vec4>& vertexNormals, vector<MaterialColor>& vertexColors)
{
	mat4 projection = m_camera->Projection();
	Vertex cur, next;

	if (SHADING_WIREFRAME == m_shadingType) 
	{
		DrawNgons(vertices, 3, Rgb(0,1,0));
		return;
	}
	
	if (m_shadingType != SHADING_FLAT)
	{
		if ((vertexColors.size() != vertices.size()))
		{
			vertexColors.clear();
			for (int j = 0; j < vertices.size(); j++)
			{
				vertexColors.push_back(defaultColor);
			}
		}
	}

	for (int i = 0; i < vertices.size(); i+=3)
	{
		Vertex v1 = vertices[i];
		Vertex v2 = vertices[i+1];
		Vertex v3 = vertices[i+2];
		vector<Vertex> poly;
		vec4 camLoc = vec4(m_camera->Eye(),0);
		vector<Rgb> colors;
		vector<Vertex> pts;
		pts.push_back(v1);
		pts.push_back(v2);
		pts.push_back(v3);

		// flat
		if( m_shadingType == SHADING_FLAT)
		{
			vec4 n = vec4(normalize( cross(v2 - v1, v3 - v2)), 0);
			vec4 m = vec4((v1.x  + v2.x + v3.x)/3,(v1.y  + v2.y + v3.y)/3,(v1.z  + v2.z + v3.z)/3, 0);
			Rgb c = GetColor(m, n, camLoc, defaultColor);
			GetDrawablePoly(pts,poly);
			FlatRasterizePolygon(poly,c);
		}
		else if (m_shadingType == SHADING_GOURARD)
		{
			for (int j = i; j < i+3; j++)
			{
				Vertex v = vertices[j];
				vec4 n = normalize(vertexNormals[j]);
				MaterialColor vCol = vertexColors[j];
				Rgb tmpC = GetColor(v, n, camLoc, vCol);
				colors.push_back(tmpC);
			}
			vector<Rgb> outColors;
			GetDrawablePoly(pts, colors, poly, outColors);
			GouraudRasterizePolygon(poly,outColors);
		}
		else if ( m_shadingType == SHADING_PHONG)
		{
			vector<MaterialColor> matColors;
			vector<vec4> normals;
			for (int j = i; j < i+3; j++)
			{
				matColors.push_back(vertexColors[j]);
				normals.push_back(vertexNormals[j]);
			}
			vector<MaterialColor> outMatColors;
			vector<vec4> outNormals;
			GetDrawablePoly(pts, matColors, normals, poly, outMatColors, outNormals);
			PhongRasterizePolygon(poly,outMatColors, outNormals);
		}

		// second
		
		
		//fullTriangle(v1,v2,v3);
	}
}

Vertex Renderer::projectedToDisplay(Vertex v)
{
	vec2 sf = ScaleFactor();
	mat4 sp = Scale( sf.x , sf.y, 1)  * Translate(1,1,0) ;
	v = sp * v;
	v = v / v.w;
	return v;
}

void Renderer::PhongRasterizePolygon(vector<Vertex>& poly, vector<MaterialColor>& colors, vector<vec4>& normals)
{
	mat4 projection = m_camera->Projection();
	mat4 view = m_camera->View();
	vec4 camLoc = vec4(m_camera->Eye(),0);
	vector<Vertex> screen;
	int ymin = m_height;
	int ymax = 0;
	for(int i = 0; i < poly.size(); ++i)
	{
		Vertex v = projectedToDisplay(projection * view * poly[i]);
		v.z = poly[i].z;
		ymin = min(ymin, (int)v.y);
		ymax = max(ymax, (int)v.y);
		screen.push_back(v);
	}

	ymin = max(0, ymin);
	ymax = min(m_height, ymax);

	// ymin is the first scanline now, ymax is the last

	for (int y = ymin ; y <= ymax ; y++) 
	{
		Vertex l, r;
		MaterialColor lc, rc;
		vec4 ln,rn;
		vec4 lv, rv;
		l.x = m_width;
		r.x = 0;
		for (int i = 0; i < screen.size(); i++) 
		{
			int j = (i+1) % screen.size();
			if ((screen[i].y > y) != (screen[j].y > y)) // if two vertices are on diffrent sides of the scanline
			{
				float t = ((float)y - screen[i].y)  / (screen[j].y - screen[i].y);
				int x = (int) interpolate(t, screen[i].x, screen[j].x);

				if (x < l.x)
				{
					lc = interpolate(t, colors[i], colors[j]);
					ln = interpolate(t, normals[i], normals[j]);
					lv = interpolate(t, poly[i], poly[j]);
					l.x = x;
					l.y = y;
					l.z = interpolate(t, screen[i].z, screen[j].z);
				}
				if (x >= r.x)
				{
					rc = interpolate(t, colors[i], colors[j]);
					rn = interpolate(t, normals[i], normals[j]);
					rv = interpolate(t, poly[i], poly[j]);
					r.x = x;
					r.y = y;
					r.z = interpolate(t, screen[i].z, screen[j].z);
				}
			}
		}

		int lb = max(0, (int)l.x);
		int rb = min(m_width - 1, (int)r.x);

		for (int x = lb; x <= rb; x++)
		{
			float t = ((float)x - l.x) / (r.x - l.x);
			float z = interpolate(t, l.z, r.z);
			
			if (z > m_zbuffer[y*m_width + x])
			{
				MaterialColor pixMat = interpolate(t, lc, rc);
				vec4 at = interpolate(t, lv, rv);
				vec4 n = interpolate(t, ln, rn);
				Rgb pixCol = GetColor(at, n, camLoc, pixMat);
				PlotPixel(x, y, pixCol);
				m_zbuffer[y*m_width + x] = z;
			}
		}
	}
}

void Renderer::GouraudRasterizePolygon(vector<Vertex>& poly, vector<Rgb>& colors)
{
	mat4 projection = m_camera->Projection();
	vector<Vertex> screen;
	int ymin = m_height;
	int ymax = 0;
	for(int i = 0; i < poly.size(); ++i)
	{
		Vertex v = projectedToDisplay(projection * poly[i]);
		v.z = poly[i].z;
		ymin = min(ymin, (int)v.y);
		ymax = max(ymax, (int)v.y);
		screen.push_back(v);
	}

	ymin = max(0, ymin);
	ymax = min(m_height, ymax);

	// ymin is the first scanline now, ymax is the last

	for (int y = ymin ; y <= ymax ; y++) 
	{
		Vertex l, r;
		Rgb lc(1,1,1), rc(1,1,1);
		l.x = m_width;
		r.x = 0;
		for (int i = 0; i < screen.size(); i++) 
		{
			int j = (i+1) % screen.size();
			if ((screen[i].y > y) != (screen[j].y > y)) // if two vertices are on diffrent sides of the scanline
			{
				float t = ((float)y - screen[i].y)  / (screen[j].y - screen[i].y);
				int x = (int) interpolate(t, screen[i].x, screen[j].x);

				if (x < l.x)
				{
					lc = interpolate(t, colors[i], colors[j]);
					l.x = x;
					l.y = y;
					l.z = interpolate(t, screen[i].z, screen[j].z);
				}
				if (x >= r.x)
				{
					rc = interpolate(t, colors[i], colors[j]);
					r.x = x;
					r.y = y;
					r.z = interpolate(t, screen[i].z, screen[j].z);
				}
			}
		}

		int lb = max(0, (int)l.x);
		int rb = min(m_width - 1, (int)r.x);

		for (int x = lb; x <= rb; x++)
		{
			float t = ((float)x - l.x) / (r.x - l.x);
			float z = interpolate(t, l.z, r.z);

			if (z > m_zbuffer[y*m_width + x])
			{
				Rgb pixCol = interpolate(t, lc, rc);
				PlotPixel(x, y, pixCol);
				m_zbuffer[y*m_width + x] = z;
			}
		}
	}
}

void Renderer::FlatRasterizePolygon(vector<Vertex>& poly, Rgb color)
{
	mat4 projection = m_camera->Projection();
	vector<Vertex> screen;
	int ymin = m_height;
	int ymax = 0;
	for(int i = 0; i < poly.size(); ++i)
	{
		Vertex v = projectedToDisplay(projection * poly[i]);
		v.z = poly[i].z;
		ymin = min(ymin, (int)v.y);
		ymax = max(ymax, (int)v.y);
		screen.push_back(v);
	}

	ymin = max(0, ymin);
	ymax = min(m_height, ymax);

	// ymin is the first scanline now, ymax is the last
	for (int y = ymin ; y <= ymax ; y++) 
	{
		Vertex l, r;
		
		l.x = m_width;
		r.x = 0;
		for (int i = 0; i < screen.size(); i++) 
		{
			int j = (i+1) % screen.size();
			if ((screen[i].y > y) != (screen[j].y > y)) // if two vertices are on diffrent sides of the scanline
			{
				float t = ((float)y - screen[i].y)  / (screen[j].y - screen[i].y);
				int x = (int) interpolate(t, screen[i].x, screen[j].x);

				if (x < l.x)
				{
					l.x = x;
					l.y = y;
					l.z = interpolate(t, screen[i].z, screen[j].z);
				}
				if (x >= r.x)
				{
					r.x = x;
					r.y = y;
					r.z = interpolate(t, screen[i].z, screen[j].z);
				}
			}
		}

		int lb = max(0, (int)l.x);
		int rb = min(m_width - 1, (int)r.x);

		for (int x = lb; x <= rb; x++)
		{
			float t = ((float)x - l.x) / (r.x - l.x);
			float z = interpolate(t, l.z, r.z);

			if (z > m_zbuffer[y*m_width + x])
			{
				if (x == 0) {
					bool ok = true;
				}
				PlotPixel(x, y, color);
				m_zbuffer[y*m_width + x] = z;
			}
		}
	}
}



void Renderer::fullTriangle(Vertex v1, Vertex v2, Vertex v3)
{
	mat4 projection = m_camera->Projection();
	vector<Vertex> poly, screen;
	vector<Rgb> colors;
	poly.push_back(v1); screen.push_back(projectedToDisplay(projection * v1)); colors.push_back(Rgb(1,0,0));
	poly.push_back(v2); screen.push_back(projectedToDisplay(projection * v2)); colors.push_back(Rgb(0,1,0));
	poly.push_back(v3); screen.push_back(projectedToDisplay(projection * v3)); colors.push_back(Rgb(0,0,1));



	int ymin = m_height;
	int ymax = 0;

	for (int i = 0; i < screen.size(); i++) {
		screen[i].z = poly[i].z;
		ymin = min(ymin, (int)screen[i].y);
		ymax = max(ymax, (int)screen[i].y);
	}

	ymin = max(0, ymin);
	ymax = min(m_height, ymax);

	CString tmp; tmp.Format("Ymin: %d, Ymax: %d\n", ymin, ymax);
	::OutputDebugStringA(tmp);

	// ymin is the first scanline now, ymax is the last

	for (int y = ymin ; y <= ymax ; y++) 
	{
		Vertex l, r;
		Rgb lc(1,1,1), rc(1,1,1);
		l.x = m_width;
		r.x = 0;
		for (int i = 0; i < screen.size(); i++) 
		{
			int j = (i+1) % screen.size();
			if ((screen[i].y > y) != (screen[j].y > y)) // if two vertices are on diffrent sides of the scanline
			{
				float t = ((float)y - screen[i].y)  / (screen[j].y - screen[i].y);
				int x = (int) interpolate(t, screen[i].x, screen[j].x);

				if (x < l.x)
				{
					lc = interpolate(t, colors[i], colors[j]);
					l.x = x;
					l.y = y;
					l.z = interpolate(t, screen[i].z, screen[j].z);
				}
				if (x >= r.x)
				{
					rc = interpolate(t, colors[i], colors[j]);
					r.x = x;
					r.y = y;
					r.z = interpolate(t, screen[i].z, screen[j].z);
				}
			}
		}

		int lb = max(0, (int)l.x);
		int rb = min(m_width - 1, (int)r.x);

		/*float t = ((float)lb - l.x) / (r.x - l.x);
		float z = interpolate(t, l.z, r.z);

		Rgb pixCol = interpolate(t, lc, rc);
		PlotPixel(lb, y, pixCol);
		m_zbuffer[y*m_width + lb] = z;*/

		for (int x = lb; x <= rb; x++)
		{
			float t = ((float)x - l.x) / (r.x - l.x);
			float z = interpolate(t, l.z, r.z);

			if (z > m_zbuffer[y*m_width + x])
			{
				Rgb pixCol = interpolate(t, lc, rc);
				PlotPixel(x, y, pixCol);
				m_zbuffer[y*m_width + x] = z;
			}
		}


	}

	/*DrawLine(vec2(0, ymin), vec2(m_width, ymin), Rgb(0,0,0));
	DrawLine(vec2(0, ymax), vec2(m_width, ymax), Rgb(0,1,0));

	for(int i = 0; i < screen.size(); i++) {
	int j  = (i + 1) % screen.size();
	DrawLine(vec2(screen[i].x, screen[i].y), vec2(screen[j].x, screen[j].y), Rgb((double)i/2,0,0));
	}
	return;*/

}

void Renderer::SetLights(vector<Light*> _lights)
{
	m_lights = _lights;
}
#pragma region  // Don't touch.
/////////////////////////////////////////////////////
//OpenGL stuff. Don't touch.

void Renderer::InitOpenGLRendering()
{
	int a = glGetError();
	a = glGetError();
	glGenTextures(1, &gScreenTex);
	a = glGetError();
	glGenVertexArrays(1, &gScreenVtc);
	//GLuint buffer;
	glBindVertexArray(gScreenVtc);
	glGenBuffers(1, &gBuffer);
	const GLfloat vtc[]={
		-1, -1,
		1, -1,
		-1, 1,
		-1, 1,
		1, -1,
		1, 1
	};
	const GLfloat tex[]={
		0,0,
		m_width,0,
		0,m_height,
		0,m_height,
		m_width,0,
		m_width,m_height};
		glBindBuffer(GL_ARRAY_BUFFER, gBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vtc)+sizeof(tex), NULL, GL_STATIC_DRAW);
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);
		glBufferSubData( GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

		GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
		glUseProgram( program );
		GLint  vPosition = glGetAttribLocation( program, "vPosition" );

		glEnableVertexAttribArray( vPosition );
		glVertexAttribPointer( vPosition, 2, GL_FLOAT, GL_FALSE, 0,
			0 );

		GLint  vTexCoord = glGetAttribLocation( program, "vTexCoord" );
		glEnableVertexAttribArray( vTexCoord );
		glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
			(GLvoid *) sizeof(vtc) );
		glUniform1i( glGetUniformLocation(program, "texture"), 0 );
}

void Renderer::CreateOpenGLBuffer()
{
	glActiveTexture(GL_TEXTURE0);
	const GLfloat vtc[]={
		-1, -1,
		1, -1,
		-1, 1,
		-1, 1,
		1, -1,
		1, 1
	};
	const GLfloat tex[]={
		0,0,
		m_width,0,
		0,m_height,
		0,m_height,
		m_width,0,
		m_width,m_height};
		glBindBuffer(GL_ARRAY_BUFFER, gBuffer);
		glBufferSubData( GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);
		glBindTexture(GL_TEXTURE_RECTANGLE, gScreenTex);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB8, m_width, m_height, 0, GL_RGB, GL_FLOAT, NULL);
		glViewport(0, 0, m_width, m_height);
}

void Renderer::SwapBuffers()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE, gScreenTex);
	glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, m_width, m_height, GL_RGB, GL_FLOAT, m_outBuffer);
	//glGenerateMipmap(GL_TEXTURE_RECTANGLE);

	glBindVertexArray(gScreenVtc);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glutSwapBuffers();
	FlushBuffer();
}
#pragma endregion

// don't work - nah fuck 14 hours 
#pragma region
void foo(vec4& v1, vec4& v2, vec4& v3)
{
	if(v1.y < v2.y || (v1.y == v2.y && v1.x > v2.x))
	{
		swap(v1,v2);
	}
	if(v1.y < v3.y || (v1.y == v3.y && v1.x > v3.x))
	{
		swap(v1,v3);
	}
	if(v2.x > v3.x)
	{
		swap(v2,v3);
	}
}

void clipLowZ(vec4& v1,vec4& v2,float zLow)
{
	v1 = v1 / v1.w;
	v2 = v2 / v2.w;

	if(v1.z < zLow && v2.z < zLow)
		return;
	if( v1.z != v2.z && !( v1.z >= zLow && v2.z >= zLow) )
	{
		if(v1.z > v2.z)
		{
			vec4 tmp = v1;
			v1 = v2;
			v2 = tmp;
		}
		if(v1.z < zLow)
		{
			vec4 k = v2 - v1;
			k.w = 0;
			float t = ( zLow - v1.z) / k. z;
			v1 = v1 + t * k;
		}
	}
}

bool clipY(vec4& v1, vec4& v2, float yLow , float yHigh)
{
	v1 = v1 / v1.w;
	v2 = v2 / v2.w;

	if( (v1.y > yHigh && v2.y > yHigh) || (v1.y < yLow && v2.y < yLow) )
		return false;
	if( v1.y != v2.y && !( v1.y >= yLow && v1.y <= yHigh && v2.y >= yLow && v2.y <= yHigh) )
	{
		if(v1.y > v2.y)
		{
			vec4 tmp = v1;
			v1 = v2;
			v2 = tmp;
		}
		if(v1.y < yLow)
		{
			vec4 k = v2 - v1;
			k.w = 0;
			float t = ( yLow - v1.y) / k.y;
			v1 = v1 + t * k;
		}
		if(v2.z > yHigh)
		{
			vec4 k = v2 - v1;
			k.w = 0;
			float t = ( yHigh - v1.y) / k.y;
			v2 = v1 + t * k;
		}
	}
	return true;
}

bool clipX(vec4& v1, vec4& v2, float xLow , float xHigh)
{
	v1 = v1 / v1.w;
	v2 = v2 / v2.w;

	if( (v1.x > xHigh && v2.x > xHigh) || (v1.x < xLow && v2.x < xLow) )
		return false;
	if( v1.x != v2.x && !( v1.x >= xLow && v1.x <= xHigh && v2.x >= xLow && v2.x <= xHigh) )
	{
		if(v1.x > v2.x)
		{
			vec4 tmp = v1;
			v1 = v2;
			v2 = tmp;
		}
		if(v1.x < xLow)
		{
			vec4 k = v2 - v1;
			k.w = 0;
			float t = ( xLow - v1.x) / k.x;
			v1 = v1 + t * k;
		}
		if(v2.z > xHigh)
		{
			vec4 k = v2 - v1;
			k.w = 0;
			float t = ( xHigh - v1.x) / k.x;
			v2 = v1 + t * k;
		}
	}
	return true;
}

void Renderer::ScanRight(vec4 v1,vec4 v2,int* contourX )
{
	if(!clipY(v1,v2,0,m_height-1)) return;

	int yMin = min(v1.y,v2.y);
	int yMax = max(v1.y,v2.y);

	if(v1.x < 0 && v2.x < 0)
	{
		for( int i = yMin; i <= yMax; i++)
		{
			contourX[m_height+i] = -1;
		}
		return;
	}

	if(v1.x > m_height - 1 && v2.x > m_height - 1)
	{
		for( int i = yMin; i <= yMax; i++)
		{
			contourX[m_height+i] = m_height - 1;
		}
		return;
	}

	clipX(v1,v2,0,m_width);
	if( v1.y > v2.y)
		swap(v1,v2);

	int yMin2 = v1.y;
	int yMax2 = v2.y;

	if( yMax2 < yMax) 
	{
		int val = (int)v2.x;
		if( val <= 1)
			val = -1;
		else if ( val >= m_width -2)
			val = m_width - 1;
		else
			assert(0);
		for( int i = yMax2; i <= yMax; i++)
		{
			contourX[m_height+i] = val;
		}
	}

	if( yMin2 > yMin)
	{
		int val = (int)v1.x;
		if( val <= 1)
			val = -1;
		else if ( val >= m_width -2)
			val = m_width - 1;
		else
			assert(0);
		for( int i = yMin; i <= yMin2; i++)
		{
			contourX[m_height+i] = val;
		}
	}

	for( int i = yMin2; i <= yMax2; i++)
	{
		contourX[m_height+i] = 0;
	}

	int x1 = (int)v1.x;
	int x2 = (int)v2.x;
	int y1 = (int)v1.y;
	int y2 = (int)v2.y;


	bool steep = abs(y2 - y1) > abs(x2 - x1);

	if(steep)
	{
		swap(x1,y1);
		swap(x2,y2);
	}
	if(x1 > x2)
	{
		swap(x1,x2);
		swap(y1,y2);
	}

	int dx = x2 - x1;
	int dy = abs(y2 - y1);

	int e =  dx/2;
	int y = y1;
	int yStep = (y1 < y2) ? 1 : -1;

	for(int x = x1; x <= x2; ++x)
	{
		if(steep)
		{
			if( y > contourX[m_height+x]) 
				contourX[m_height+x] = y;
		}
		else
		{
			if( x > contourX[m_height+y]) 
				contourX[m_height+y] = x;
		}
		e -= dy;
		if(e < 0)
		{
			e += dx;
			y += yStep;
		}
	}
}

void Renderer::ScanLeft(vec4 v1,vec4 v2,int* contourX )
{
	if(!clipY(v1,v2,0,m_height-1)) return;

	int yMin = min(v1.y,v2.y);
	int yMax = max(v1.y,v2.y);

	if(v1.x < 0 && v2.x < 0)
	{
		for( int i = yMin; i <= yMax; i++)
		{
			contourX[i] = 0;
		}
		return;
	}

	if(v1.x > m_height - 1 && v2.x > m_height - 1)
	{
		for( int i = yMin; i <= yMax; i++)
		{
			contourX[i] = m_height - 1;
		}
		return;
	}

	clipX(v1,v2,0,m_width-1);
	if( v1.y > v2.y)
		swap(v1,v2);

	int yMin2 = v1.y;
	int yMax2 = v2.y;

	if( yMax2 < yMax) 
	{
		int val = (int)v1.x;
		for( int i = yMax2; i <= yMax; i++)
		{
			contourX[i] = val;
		}
	}

	if( yMin2 > yMin) 
	{
		int val = (int)v2.x;
		for( int i = yMin; i <= yMin2; i++)
		{
			contourX[i] = val;
		}
	}

	for( int i = yMin2; i <= yMax2; i++)
	{
		contourX[i] = m_height;
	}

	int x1 = (int)v1.x;
	int x2 = (int)v2.x;
	int y1 = (int)v1.y;
	int y2 = (int)v2.y;


	bool steep = abs(y2 - y1) > abs(x2 - x1);

	if(steep)
	{
		swap(x1,y1);
		swap(x2,y2);
	}
	if(x1 > x2)
	{
		swap(x1,x2);
		swap(y1,y2);
	}

	int dx = x2 - x1;
	int dy = abs(y2 - y1);

	int e =  dx/2;
	int y = y1;
	int yStep = (y1 < y2) ? 1 : -1;

	for(int x = x1; x <= x2; ++x)
	{
		if(steep)
		{
			if( y < contourX[x]) 
				contourX[x] = y;
		}
		else
		{
			if( x < contourX[y]) 
				contourX[y] = x;
		}
		e -= dy;
		if(e < 0)
		{
			e += dx;
			y += yStep;
		}
	}
}

void Renderer::RasterizeTriangle(vec4 v1, vec4 v2, vec4 v3)
{
	vec2 sf = ScaleFactor();
	mat4 ft = Scale( sf.x , sf.y, 1) * Translate(1,1,0)  * m_camera->Projection();
	v1 = ft * v1;
	v2 = ft * v2;
	v3 = ft * v3;

	v1 = v1 / v1.w;
	v2 = v2 / v2.w;
	v3 = v3 / v3.w;

	int xMin = min( min(v1.x, v2.x), v3.x);
	int xMax = max( max(v1.x, v2.x), v3.x);
	int yMin = min( min(v1.y, v2.y), v3.y);
	int yMax = max( max(v1.y, v2.y), v3.y);

	if( yMin > m_height || yMax < 0 || xMin > m_width || xMax < 0) return;

	xMin = max( xMin, 0);
	yMin = max( yMin, 0);
	xMax = min( xMax, m_width);
	yMax = min( yMax, m_height-1);

	foo(v1,v2,v3);

	vec3 crosProd = cross(vec3(v2.x,v2.y,0) - vec3(v1.x,v1.y,0), vec3(v3.x,v3.y,0) - vec3(v1.x,v1.y,0));

	if(crosProd.z < 0)
	{
		swap(v2,v3);
	}

	if(v2.y > v3.y)
	{
		ScanLeft(v1,v2,contourX);
		ScanLeft(v2,v3,contourX);
		ScanRight(v1,v3,contourX);
	}
	else
	{
		ScanLeft(v1,v2,contourX);
		ScanRight(v2,v3,contourX);
		ScanRight(v1,v3,contourX);
	}
	for( int i = yMin; i <= yMax; ++i)
	{
		int right = contourX[m_height + i];
		int left = contourX[i];
		if(right > m_width - 1 || left < 0 || left > right) 
			continue;
		for(int j = left; j <= right; j++)
			PlotPixel(j,i);
		;
	}
}

void Renderer::DrawTriangle(vec4& v1, vec4& v2, vec4& v3)
{
	float zHigh = -m_camera->ZNear();
	float zLow = -m_camera->ZFar();
	v1 = v1 / v1.w;
	v2 = v2 / v2.w;
	v3 = v3 / v3.w;
	int count = (v1.z < zLow) + (v2.z < zLow) + (v3.z < zLow);
	if( count == 3) return;
	else if( count == 2)
	{
		if(v3.z < zLow)
			swap(v1,v3);
		if(v3.z < zLow)
			swap(v2,v3);
		vec4 vv1 = v1;
		vec4 vv2 = v3;
		vec4 vv3 = v2;
		vec4 vv4 = v3;
		clipLowZ(vv1,vv2,zLow);
		clipLowZ(vv3,vv4,zLow);
		assert( vv2 == vv4);
		RasterizeTriangle(vv1,vv3,vv2);
	}
	else if( count == 1)
	{
		if(v3.z < zLow)
			swap(v1,v3);
		if(v2.z < zLow)
			swap(v2,v1);
		vec4 vv1 = v1;
		vec4 vv2 = v2;
		vec4 vv3 = v1;
		vec4 vv4 = v3;
		clipLowZ(vv1,vv2,zLow);
		clipLowZ(vv3,vv4,zLow);
		RasterizeTriangle(vv1,vv2,vv4);
		RasterizeTriangle(vv1,vv3,vv4);
	}
	else
	{
		RasterizeTriangle(v1,v2,v3);
	}
}

void Renderer::DrawTriangles(vector<Vertex>& vertices)
{
	mat4 view = m_camera->View();
	Vertex v1, v2, v3;
	for (int i = 3; i <= vertices.size(); i+=3 )
	{
		v1 = view * vertices[i-3];
		v2 = view * vertices[i-2];
		v3 = view * vertices[i-1];
		DrawTriangle(v1,v2,v3);
	}
}

void Renderer::DrawNgonsFast(vector<Vertex>& vertices, int n, Rgb color) 
{
	if (n < 2) return;
	mat4 projection = m_camera->Projection();
	mat4 view = m_camera->View();
	float zHigh = -m_camera->ZNear();
	float zLow = -m_camera->ZFar();
	Vertex p1, p2;
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i] = view * vertices[i];
		if ( (i+1) % n == 0 && (i+1) >= n) //draw srarting from current n vertices back
		{
			int startIdx = i-n+1;
			for (int j = 0; j < n; j++) {
				// cur and next are references to the actual array elements
				// these elements have allready in camera coordinates
				Vertex& cur = vertices[startIdx + j];
				Vertex& next = vertices[startIdx + ((j+1)%n)];
				// cur and next from here are in camera coordinates
				if ( clipZ(cur, next, zLow, zHigh) ) 
				{
					// clipZ (maybe) changed values of cur and next, now they are between zNear and zFar
					p1 = projection * cur;
					p2 = projection * next;
					// p1 and p2 are copies of cur and next after projection
					if( clip(p1, p2) )
					{
						// clip (maybe) changed values of cur and next, now they are entirely inside canonic view volume, we can draw them
						DrawLine(p1, p2, color);
					}
				}
			}
		}
	}

}

void Renderer::DrawLine3D(vec3 v1, vec3 v2, Rgb col) {
	mat4 fp = m_camera->Projection() * m_camera->View();
	vec4 p1 = fp * vec4(v1,1);
	vec4 p2 = fp * vec4(v2,1);
	if (abs(p1.w) < 0.001 || abs(p2.w) < 0.001)
		return;
	if (clip(p1, p2)) 
	{
		DrawLine(p1, p2, col);
	}

}
#pragma endregion