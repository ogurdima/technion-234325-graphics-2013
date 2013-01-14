#include "StdAfx.h"
#include "Renderer.h"


Renderer::Renderer(int _w, int _h) :
deviceW(_w),
deviceH(_h),
program(-1)
{
	cout << "Renderer::Renderer: " << deviceW << "x" << deviceH << endl;
}


Renderer::~Renderer(void)
{
}


void Renderer::SetShadingProgram(GLuint _program)
{
	program = _program;
	glUseProgram(program);
}

ModelBind Renderer::BindModel(vector<vec4> pts, vector<vec4> normals /*also textures*/)
{
	if (program < 0)
	{
		throw std::exception("Binding model without program set");
	}

	GLuint buffers[1];
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, buffers);

	GLuint vPositionLoc = glGetAttribLocation(program, "vPosition");
	GLuint modelLoc = glGetUniformLocation(program, "model");

	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * pts.size(), &pts[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(vPositionLoc);
	glVertexAttribPointer(vPositionLoc, 4, GL_FLOAT, 0, 0, 0);

	ModelBind b;
	b.modelHnd = modelLoc;
	b.ptsHnd = vPositionLoc;
	b.vao = vao;
	return b;
}

CameraBind Renderer::BindCamera()
{
	if (program < 0)
	{
		throw std::exception("Binding camera without program set");
	}

	GLuint viewLoc = glGetUniformLocation(program, "view");
	GLuint projectLoc = glGetUniformLocation(program, "projection");

	CameraBind b;
	b.projectHnd = projectLoc;
	b.viewHnd = viewLoc;
	return b;
}

void Renderer::SetUniformMatrices(vector<GLuint> handles, vector<mat4> values)
{
	if (program < 0 || handles.size() != values.size() || handles.size() == 0)
	{
		throw std::exception("Bad arguments in SetUniformMatrices");
	}
	for (int i = 0; i < handles.size(); i++)
	{
		glUniformMatrix4fv(handles[i], 1, GL_TRUE, values[i]);
	}
}

void Renderer::SetUniformMatrix(GLuint handle, mat4 val)
{
	if (program < 0)
	{
		throw std::exception("Bad arguments in SetUniformMatrix");
	}
	glUniformMatrix4fv(handle, 1, GL_TRUE, val);
}

void Renderer::DrawTriangles(GLuint vao, int count)
{
	if (program < 0)
	{
		throw std::exception("DrawTriangles called with not program set");
	}
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, count);
}

void Renderer::SwapBuffers()
{
	glutSwapBuffers();
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}