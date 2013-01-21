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

ModelBind Renderer::BindModel(vector<vec4> pts, vector<vec4> normals /*also textures*/, MaterialColor c)
{
	if (program < 0)
	{
		throw std::exception("Binding model without program set");
	}

	GLuint buffers[2];
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(2, buffers);

	GLuint vPositionLoc = glGetAttribLocation(program, "vPosition");
	GLuint vNormalLoc = glGetAttribLocation(program, "vNormal");
	GLuint modelLoc = glGetUniformLocation(program, "model");
	GLuint normalTransformLoc = glGetUniformLocation(program, "normalTransform");
	GLuint emissiveLoc = glGetUniformLocation(program, "emissive");
	GLuint diffuseLoc = glGetUniformLocation(program, "diffuse");
	GLuint ambientLoc = glGetUniformLocation(program, "ambient");
	GLuint specularLoc = glGetUniformLocation(program, "specular");
	GLuint shininessLoc = glGetUniformLocation(program, "shininess");

	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * pts.size(), &pts[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(vPositionLoc);
	glVertexAttribPointer(vPositionLoc, 4, GL_FLOAT, 0, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * normals.size(), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(vNormalLoc);
	glVertexAttribPointer(vNormalLoc, 4, GL_FLOAT, 0, 0, 0);

	glUniform3f(emissiveLoc, c.emissive.r, c.emissive.g, c.emissive.b);
	glUniform3f(ambientLoc, c.ambient.r, c.ambient.g, c.ambient.b);
	glUniform3f(specularLoc, c.specular.r, c.specular.g, c.specular.b);
	glUniform3f(diffuseLoc, c.diffuse.r, c.diffuse.g, c.diffuse.b);
	glUniform1f(shininessLoc, 4.0);

	ModelBind b;
	b.pointMat = modelLoc;
	b.normMat = normalTransformLoc;
	b.ptsHnd = vPositionLoc; // not sure we need this
	b.nrmHnd = vNormalLoc; // not sure we need this
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

void Renderer::SetParallelLights(vector<vec4> lightDirections, vector<vec3> lightColors)
{
	if (program < 0)
	{
		throw std::exception("Binding camera without program set");
	}
	int bound = min(lightDirections.size(), MAX_LIGHTS);
	char name[512];

	for (int i = 0; i < bound; i++)
	{
		sprintf(name, "lightDir[%d]", i);
		int lightDirLoc = glGetUniformLocation(program, name);
		glUniform3f(lightDirLoc, lightDirections[i].x, lightDirections[i].y, lightDirections[i].z);

		sprintf(name, "parlightColor[%d]", i);
		int parlightColorLoc = glGetUniformLocation(program, name);
		glUniform3f(parlightColorLoc, lightColors[i].x, lightColors[i].y, lightColors[i].z);

		cout << "Renderer::SetParallelLights: (" << lightDirections[i].x << ", " << lightDirections[i].y << ", " << lightDirections[i].z << ")";
		cout << " RGB: [" << lightColors[i].x << ", " << lightColors[i].y << ", " << lightColors[i].z << "]" << endl;
	}
	int parallelLightNumLoc = glGetUniformLocation(program, "parallelLightNum");
	glUniform1i(parallelLightNumLoc, bound);
}

void Renderer::SetPointLights(vector<vec4> lightPositions, vector<vec3> lightColors)
{
	if (program < 0)
	{
		throw std::exception("Binding camera without program set");
	}
	int bound = min(lightPositions.size(), MAX_LIGHTS);
	char name[512];

	for (int i = 0; i < bound; i++)
	{
		sprintf(name, "lightPos[%d]", i);
		int lightPosLoc = glGetUniformLocation(program, name);
		glUniform3f(lightPosLoc, lightPositions[i].x, lightPositions[i].y, lightPositions[i].z);

		sprintf(name, "ptlightColor[%d]", i);
		int ptlightColorLoc = glGetUniformLocation(program, name);
		glUniform3f(ptlightColorLoc, lightColors[i].x, lightColors[i].y, lightColors[i].z);

		cout << "Renderer::SetPointLights: (" << lightPositions[i].x << ", " << lightPositions[i].y << ", " << lightPositions[i].z << ")" << endl;
		cout << " RGB: [" << lightColors[i].x << ", " << lightColors[i].y << ", " << lightColors[i].z << "]" << endl;
	}
	int pointLightNumLoc = glGetUniformLocation(program, "pointLightNum");
	glUniform1i(pointLightNumLoc, bound);
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
	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, count);
}

void Renderer::SwapBuffers()
{
	glutSwapBuffers();
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}