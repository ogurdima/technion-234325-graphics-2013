#include "StdAfx.h"
#include "Renderer.h"
#include "CG_skel_w_MFC.h"
#include "InitShader.h"
#include "GL\freeglut.h"

#define INDEX(width,x,y,c) (x+y*width)*3+c

Renderer::Renderer() :m_width(512), m_height(512)
{
	InitOpenGLRendering();
	CreateBuffers(512,512);
}

Renderer::Renderer(int width, int height) :m_width(width), m_height(m_height)
{
	InitOpenGLRendering();
	CreateBuffers(width,height);
}

Renderer::~Renderer(void)
{
}

void Renderer::CreateBuffers(int width, int height)
{
	m_width=width;
	m_height=height;	
	CreateOpenGLBuffer(); //Do not remove this line.
	m_outBuffer = new float[3*m_width*m_height];
}

void Renderer::FlushBuffer()
{
	for(int i=0; i<m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			m_outBuffer[INDEX(m_width,j,i,0)]=0;	m_outBuffer[INDEX(m_width,j,i,1)]=0;	m_outBuffer[INDEX(m_width,j,i,2)]=0;
		}
	}
}

void Renderer::SetDemoBuffer()
{
	////vertical line
	//for(int i=0; i<m_height; i++)
	//{
	//	m_outBuffer[INDEX(m_width,256,i,0)]=1;	m_outBuffer[INDEX(m_width,256,i,1)]=0;	m_outBuffer[INDEX(m_width,256,i,2)]=0;
	//}
	////horizontal line
	//for(int i=0; i<m_width; i++)
	//{
	//	m_outBuffer[INDEX(m_width,i,256,0)]=1;	m_outBuffer[INDEX(m_width,i,256,1)]=0;	m_outBuffer[INDEX(m_width,i,256,2)]=1;
	//}

	//// diagonal line
	//for(int i=0; i<m_width; i++)
	//{
	//	m_outBuffer[INDEX(m_width,i,i,0)]=0;	m_outBuffer[INDEX(m_width,i,i,1)]=1;	m_outBuffer[INDEX(m_width,i,i,2)]=0;
	//}

	//DrawLine(vec2(20,20), vec2(200,200));
	//DrawLine(vec2(20,200), vec2(200,20));
	//DrawLine(vec2(256,256), vec2(256,300));
	//DrawLine(vec2(50,5), vec2(35,120));

	vec2 p1, p2;
	vec2 center(m_width / 2, m_height / 2);
	float r = m_width / 2;
	float a = 0;
	for (int i = 0; i < 32; i++){
		p1.x = center.x - r * cos(a);
		p1.y = center.y - r * sin(a);
		p2.x = center.x + r * cos(a);
		p2.y = center.y + r * sin(a);

		DrawLine(p1, p2);

		a += 3.14159265358979323846 / 16; /* 2PI/32 */
	}
}

void Renderer::Draw(vector<Vertex> vertices)
{
	mat4 finalProjection = Scale( m_width/2, m_height/2, 0) * Translate(1,1,0) * m_camera->Projection() * m_camera->Transformation();
	for(vector<Vertex>::iterator it = vertices.begin();   it != vertices.end(); it++) 
	{
		vec4 v = finalProjection * (*it);
		*it = v;
	}
	for(int i = 0; i+3 <= vertices.size() ; i+=3)
	{
		Vertex v1 = vertices.at(i);
		Vertex v2 = vertices.at(i+1);
		Vertex v3 = vertices.at(i+2);
		DrawTriangle2D(	vec2(v1.x/v1.w,v1.y/v1.w), vec2(v2.x/v2.w,v2.y/v2.w), vec2(v3.x/v3.w,v3.y/v3.w));
	}
}

void Renderer::DrawLine3D(vec4 v1, vec4 v2) {
	vec2 p1 = ProjectPoint(v1);
	vec2 p2 = ProjectPoint(v2);
	DrawLine(p1, p2);
}

void Renderer::DrawTriangle2D(vec2 v1, vec2 v2, vec2 v3)
{
	DrawLine(v1,v2);
	DrawLine(v1,v3);
	DrawLine(v2,v3);
}

void Renderer::SetCamera(Camera* c)
{
	m_camera = c;
}

void Renderer::plotPixel(int x, int y)
{
	if (x < 0 || x >= m_width || y < 0 || y >= m_height)
		return;
	m_outBuffer[INDEX(m_width,x,y,0)]=1;	m_outBuffer[INDEX(m_width,x,y,1)]=1;	m_outBuffer[INDEX(m_width,x,y,2)]=1;
}

void Renderer::DrawLine(vec2 p1, vec2 p2)
{
	int x1 = (int)p1.x;
	int x2 = (int)p2.x;
	int y1 = (int)p1.y;
	int y2 = (int)p2.y;
	bool steep = abs(y2 - y1) > abs(x2 - x1);

	if(steep)
	{
		swap(x1,y1);
		swap(x2,y2);
	}
	if(x1 > x1)
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
			plotPixel(y,x);
		}
		else
		{
			plotPixel(x,y);
		}
		e -= dy;
		if(e < 0)
		{
			e += dx;
			y += yStep;
		}
	}
}

inline vec2 Renderer::ProjectPoint(vec3 p)
{
	return ProjectPoint(vec4(p));
}

inline vec2 Renderer::ProjectPoint(vec4 p)
{
	if (NULL == m_camera)
		return vec2(0,0);
	mat4 finalProjection = Scale( m_width/2, m_height/2, 0) * Translate(1,1,0) * m_camera->Projection() * m_camera->Transformation();
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