#include "StdAfx.h"
#include "Renderer.h"
#include "InitShader.h"

Renderer::Renderer(int _w, int _h) :
deviceW(_w),
deviceH(_h)
{
	InitShaders();
	cout << "Renderer::Renderer: " << deviceW << "x" << deviceH << endl;
}

Renderer::~Renderer(void)
{
}

void Renderer::InitShaders()
{
	oglPrograms[FLAT] = InitShader("Shaders/flat_vshader.glsl", "Shaders/flat_fshader.glsl");
	oglPrograms[GOURAUD] = InitShader("Shaders/flat_vshader.glsl", "Shaders/flat_fshader.glsl");
	oglPrograms[PHONG] = InitShader("Shaders/phong_vshader.glsl", "Shaders/phong_fshader.glsl");
	oglPrograms[TOON] = InitShader("Shaders/toon_vshader.glsl", "Shaders/toon_fshader.glsl");
	oglPrograms[SILHOUETTE] = InitShader("Shaders/silhouette_vshader.glsl", "Shaders/silhouette_fshader.glsl");
	oglLineProgram = InitShader("Shaders/line_vshader.glsl", "Shaders/line_fshader.glsl");
}

void Renderer::SetShading(ShadingType _type)
{
	shading = _type;
	glUseProgram(oglPrograms[shading]);
}

ShadingType Renderer::Shading()
{
	return shading;
}

ModelBind Renderer::BindModel(vector<vec4> pts, vector<vec4> normals)
{
	GLuint program = oglPrograms[shading];
	if (program < 0)
	{
		throw std::exception("Binding model without program set");
	}

	ModelBind b;
	RebindModelUniforms(&b);

	GLuint* buffers = new GLuint[2];
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(2, buffers);

	GLuint vPositionLoc = glGetAttribLocation(program, "vPosition");
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * pts.size(), &pts[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(vPositionLoc);
	glVertexAttribPointer(vPositionLoc, 4, GL_FLOAT, 0, 0, 0);
	
	GLuint vNormalLoc = glGetAttribLocation(program, "vNormal");
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * normals.size(), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(vNormalLoc);
	glVertexAttribPointer(vNormalLoc, 4, GL_FLOAT, 0, 0, 0);

	b.vao = vao;
	b.size = 2;
	b.buffers = buffers;
	return b;
}

void Renderer::RebindModelUniforms( ModelBind* mb)
{
	GLuint program = oglPrograms[shading];
	if (program < 0)
	{
		throw std::exception("Binding model without program set");
	}
	
	mb->modelLoc = glGetUniformLocation(program, "model");
	mb->normalTransformLoc = glGetUniformLocation(program, "normalTransform");
	mb->emissiveLoc = glGetUniformLocation(program, "emissive");
	mb->diffuseLoc = glGetUniformLocation(program, "diffuse");
	mb->ambientLoc = glGetUniformLocation(program, "ambient");
	mb->specularLoc = glGetUniformLocation(program, "specular");
	mb->shininessLoc = glGetUniformLocation(program, "shininess");
}

void Renderer::SetParallelLights(vector<vec4> lightDirections, vector<vec3> lightColors)
{
	GLuint program = oglPrograms[shading];
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
	GLuint program = oglPrograms[shading];
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
	GLuint program = oglPrograms[shading];
	if (program < 0 || handles.size() != values.size() || handles.size() == 0)
	{
		throw std::exception("Bad arguments in SetUniformMatrices");
	}
	for (unsigned int i = 0; i < handles.size(); i++)
	{
		glUniformMatrix4fv(handles[i], 1, GL_TRUE, values[i]);
	}
}

void Renderer::SetUniformVec3(GLuint handle, vec3 val)
{
	GLuint program = oglPrograms[shading];
	if (program < 0)
	{
		throw std::exception("Bad arguments in SetUniformMatrices");
	}
	glUniform3f(handle, val.x, val.y, val.z);
}

void Renderer::SetUniform(GLuint handle, float val)
{
	GLuint program = oglPrograms[shading];
	if (program < 0)
	{
		throw std::exception("Bad arguments in SetUniformMatrices");
	}
	glUniform1f(handle, val);
}

void Renderer::SetUniformMatrix(GLuint handle, mat4 val)
{
	GLuint program = oglPrograms[shading];
	if (program < 0)
	{
		throw std::exception("Bad arguments in SetUniformMatrix");
	}
	glUniformMatrix4fv(handle, 1, GL_TRUE, val);
}

void Vertex3f(vec4 v)
{
	glVertex3f( v.x/ v.w, v.y/ v.w, v.z/ v.w);
}

//void Renderer::DrawWorldAxes(mat4 toCam, GLfloat len)
//{
//	vec4 s = toCam * vec4(0,0,0,1);
//	vec4 x = toCam * vec4(len,0,0,1);
//	vec4 y = toCam * vec4(0,len,0,1);
//	vec4 z = toCam * vec4(0,0,len,1);
//
//	glBegin( GL_LINES );
//	glLineWidth((GLfloat)2);
//	glClearColor(0,0,0,0);
//	glColor3f( 1.0f, 0.0f, 0.0f );
//	glVertex3f( 1.0f, 1.0f, 0.0f);
//	glVertex3f( -1.0f, -1.0f, 0.0f);
//	glEnd();
//}

void Renderer::DrawTriangles(GLuint vao, int count)
{
	GLuint program = oglPrograms[shading];
	if (program < 0)
	{
		throw std::exception("DrawTriangles called with no program set");
	}
	
	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(vao);

	glDrawArrays(GL_TRIANGLES, 0, count);
}

void Renderer::DrawSilhouette(GLuint vao, int count)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	DrawTriangles(vao, count);
	glDisable(GL_CULL_FACE);
}


GLuint Renderer::BindLineBuffer(vector<vec4> verteces, vector<vec3> colors)
{
	if (oglLineProgram < 0)
	{
		throw std::exception("Problem with line shader");
	}

	GLuint buffers[2];
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(2, buffers);

	GLuint vPositionLoc = glGetAttribLocation(oglLineProgram, "vPosition");
	GLuint vColorLoc = glGetAttribLocation(oglLineProgram, "vColor");



	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * verteces.size(), &verteces[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(vPositionLoc);
	glVertexAttribPointer(vPositionLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * colors.size(), &colors[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(vColorLoc);
	glVertexAttribPointer(vColorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	return vao;
}

void Renderer::DrawWFLines(vector<vec4> verteces, vector<vec3> colors)
{
	if (oglLineProgram < 0)
	{
		throw std::exception("DrawWFLines program is invalid");
	}
	GLuint vao = BindLineBuffer(verteces, colors);
	
	glUseProgram(oglLineProgram);
	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(vao );
	// TODO: delete verteces
	glLineWidth( 2);
	glDrawArrays(GL_LINES, 0, min(verteces.size(), colors.size()));
	glUseProgram(oglPrograms[shading]);
	return;
}

//void Renderer::DrawParallelSource(Rgb col, vec4 dir, mat4 toScreen)
//{
//	glColor3f(col.r, col.g, col.b);
//
//	glBegin(GL_LINES);
//
//	vec4 origin = toScreen * vec4(0,0,0,1);
//	dir = toScreen * dir;
//	
//	glVertex2f(origin.x, origin.y);
//	glVertex2f(dir.x, dir.y);
//
//	glEnd();
//}

void Renderer::SwapBuffers()
{
	glutSwapBuffers();
}

void Renderer::ToggleAntialiasing()
{
	if(glIsEnabled( GL_MULTISAMPLE) )
	{
		glDisable( GL_MULTISAMPLE);
		return;
	}
	glEnable( GL_MULTISAMPLE);
}

void assignCameraTransformations(GLuint program, mat4 view, mat4 projection)
{
	GLuint viewLoc = glGetUniformLocation(program, "view");
	glProgramUniformMatrix4fv( program, viewLoc, 1, GL_TRUE, view);
	GLuint projectLoc = glGetUniformLocation(program, "projection");
	glProgramUniformMatrix4fv( program, projectLoc, 1, GL_TRUE, projection);
}

void Renderer::InitDraw(mat4 view, mat4 projection)
{
	// clear buffer 
	// TODO: clear color to backgroundColor
	glClearColor(1,1,1,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enabling camera's transformations
	// for line program
	assignCameraTransformations(oglLineProgram, view, projection);
	// for current program
	assignCameraTransformations(oglPrograms[shading], view, projection);
	// Enable current program
	glUseProgram(oglPrograms[shading]);
}

