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
m_bg(bg)
{
	InitOpenGLRendering();
	CreateBuffers(512,512);
}

Renderer::Renderer(int width, int height, Rgb bg) :
m_width(width), 
m_height(m_height), 
m_outBuffer(NULL),
m_zbuffer(NULL),
m_bg(bg)
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
	FlushBuffer();
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

void Renderer::ClearDepthBuffer()
{
	// clear it
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
	if (clip(p1, p2)) 
	{
		DrawLine(p1, p2, col);
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
		if(v2.z > zHigh)
		{
			vec4 k = v2 - v1;
			k.w = 0;
			float t = ( zHigh - v1.z) / k. z;
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
	mat4 sp = Scale( sf.x , sf.y, 1)  * Translate(1,1,0) ;
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