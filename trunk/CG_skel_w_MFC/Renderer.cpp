#include "StdAfx.h"
#include "Renderer.h"
#include "CG_skel_w_MFC.h"
#include "InitShader.h"
#include "GL\freeglut.h"

#define INDEX(width,x,y,c) (x+y*width)*3+c

Renderer::Renderer() :
m_width(512), 
m_height(512), 
m_outBuffer(NULL),
m_zbuffer(NULL)
{
	InitOpenGLRendering();
	CreateBuffers(512,512);
}

Renderer::Renderer(int width, int height) :
m_width(width), 
m_height(m_height), 
m_outBuffer(NULL),
m_zbuffer(NULL)
{
	InitOpenGLRendering();
	CreateBuffers(width,height);
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
}

void Renderer::FlushBuffer()
{
	ClearColorBuffer();
	//ClearDepthBuffer();
}

vec2 Renderer::ScaleFactor() 
{
	vec2 res;
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

inline mat4 Renderer::FinalProjection()
{
	//mat4 finalProjection = Scale( m_width/2, m_height/2, 0) * Translate(1,1,0) * m_camera->Projection() * m_camera->Transformation();
	vec2 sf = ScaleFactor();
	mat4 finalProjection =  Scale( sf.x , sf.y, 1)  * Translate(1,1,0) *  m_camera->Projection() * m_camera->Transformation();
	return finalProjection;
}

void Renderer::ClearColorBuffer() 
{
	for(int i=0; i<m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			m_outBuffer[INDEX(m_width,j,i,0)]=0;	m_outBuffer[INDEX(m_width,j,i,1)]=0;	m_outBuffer[INDEX(m_width,j,i,2)]=0;
		}
	}
}

void Renderer::ClearDepthBuffer()
{
	// clear it
}

void Renderer::DrawVisibleBoundary()
{
	vec2 sf = ScaleFactor();
	vec4 v1 = Scale( sf.x, sf.y, 0) * Translate(1,1,0) * vec4(-1,-1,0,1);
	vec4 v2 = Scale( sf.x, sf.y, 0) * Translate(1,1,0) * vec4(-1,1,0,1);
	vec4 v3 = Scale( sf.x, sf.y, 0) * Translate(1,1,0) * vec4(1,1,0,1);
	vec4 v4 = Scale( sf.x, sf.y, 0) * Translate(1,1,0) * vec4(1,-1,0,1);
	DrawLine(vec2(v1.x / v1.w, v1.y/ v1.w), vec2(v2.x / v2.w, v2.y/ v2.w) );
	DrawLine(vec2(v3.x / v3.w, v3.y/ v3.w), vec2(v2.x / v2.w, v2.y/ v2.w) );
	DrawLine(vec2(v3.x / v3.w, v3.y/ v3.w), vec2(v4.x / v4.w, v4.y/ v4.w) );
	DrawLine(vec2(v1.x / v1.w, v1.y/ v1.w), vec2(v4.x / v4.w, v4.y/ v4.w) );
}

void Renderer::Draw(vector<Vertex>& vertices, Rgb color)
{
	mat4 finalProjection = FinalProjection();
	vector<Vertex>::iterator it = vertices.begin();
	while (it != vertices.end())
	{
		Vertex v1 = finalProjection * ( *it++);
		Vertex v2 = finalProjection * ( *it++);
		Vertex v3 = finalProjection * ( *it++);
		
		vec3 p1 = vec3(v1.x/v1.w, v1.y/v1.w, v1.z/v1.w);
		vec3 p2 = vec3(v2.x/v2.w, v2.y/v2.w, v2.z/v2.w);
		if (clip(p1, p2)) {
			DrawLine(p1, p2, color);
		}
		p1 = vec3(v1.x/v1.w, v1.y/v1.w, v1.z/v1.w);
		p2 = vec3(v3.x/v3.w, v3.y/v3.w, v3.z/v3.w);
		if (clip(p1, p2)) {
			DrawLine(p1, p2, color);
		}
		p1 = vec3(v2.x/v2.w, v2.y/v2.w, v2.z/v2.w);
		p2 = vec3(v3.x/v3.w, v3.y/v3.w, v3.z/v3.w);
		if (clip(p1, p2)) {
			DrawLine(p1, p2, color);
		}

	}

	for(int i = 0; i+3 <= vertices.size()  ; i+=3)
	{
		Vertex v1 = vertices.at(i);
		Vertex v2 = vertices.at(i+1);
		Vertex v3 = vertices.at(i+2);
		
		DrawTriangle2D(	vec2(v1.x/v1.w,v1.y/v1.w), vec2(v2.x/v2.w,v2.y/v2.w), vec2(v3.x/v3.w,v3.y/v3.w), color);
	}
}

bool Renderer::clip(vec3& v1, vec3& v2)
{


	return true;
}

void Renderer::DrawLineSegments(vector<vec4>& segmentList, Rgb color)
{
	mat4 finalProjection = FinalProjection();
	for(vector<vec4>::iterator it = segmentList.begin();   it != segmentList.end(); it++) 
	{
		vec4 v = finalProjection * (*it);
		*it = v;
	}
	for(int i = 0; i+2 <= segmentList.size(); i+=2)
	{
		vec4 p1 = segmentList.at(i);
		vec4 p2 =  segmentList.at(i+1);
		DrawLine( vec2(p1.x/p1.w,p1.y/p1.w), vec2(p2.x/p2.w,p2.y/p2.w), color );
		bool OK = true;
	}
}

void Renderer::DrawPolyline(vector<Vertex>& vertices, Rgb color)
{
	mat4 finalProjection =FinalProjection();
	for(vector<Vertex>::iterator it = vertices.begin();   it != vertices.end(); it++) 
	{
		vec4 v = finalProjection * (*it);
		*it = v;
	}
	for(int i = 0; i+1 < vertices.size(); i++)
	{
		Vertex v1 = vertices.at(i);
		Vertex v2 = vertices.at(i+1);
		DrawLine(vec2(v1.x/v1.w,v1.y/v1.w), vec2(v2.x/v2.w,v2.y/v2.w), color);
	}
}

void Renderer::DrawLine3D(vec3 v1, vec3 v2, Rgb col) {
	vec2 p1 = ProjectPoint( vec4(v1, 1) );
	vec2 p2 = ProjectPoint( vec4(v2, 1) );
	DrawLine(p1, p2, col);
}

inline void Renderer::DrawTriangle2D(vec2 v1, vec2 v2, vec2 v3, Rgb col)
{
	DrawLine(v1,v2, col);
	DrawLine(v1,v3, col);
	DrawLine(v2,v3, col);
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

void Renderer::DrawLine(vec3 p1, vec3 p2, Rgb col)
{
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

//--------------------------------------------------------------------------
// Transformation stuff
//--------------------------------------------------------------------------

void Renderer::SetCamera(Camera* c)
{
	m_camera = c;
}

inline vec2 Renderer::ProjectPoint(vec3 p)
{
	return ProjectPoint(vec4(p, 1));
}

inline vec2 Renderer::ProjectPoint(vec4 p)
{
	if (NULL == m_camera)
		return vec2(0,0);
	mat4 finalProjection = FinalProjection();
	vec4 projected = finalProjection * p;
	return vec2(projected.x/projected.w, projected.y/projected.w);
}

vec3 Renderer::ObjectToCamera(vec4 p) {
	if (NULL == m_camera)
		return vec3(0,0,0);
	mat4 cameraTransformation = m_camera->Transformation();
	vec4 inCameraCoords = cameraTransformation * p;
	return vec3(inCameraCoords.x/inCameraCoords.w, inCameraCoords.y/inCameraCoords.w, inCameraCoords.z/inCameraCoords.w);
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