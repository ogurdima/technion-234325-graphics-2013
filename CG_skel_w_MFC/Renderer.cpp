#include "StdAfx.h"
#include "Renderer.h"
#include "InitShader.h"

//#pragma region Ctor Dtor
void checkOpenGLerror()
{
  GLenum errCode;
  if(( errCode = glGetError()) != GL_NO_ERROR)
    cout << "OpenGl error! - " << gluErrorString(errCode) << endl;;
}

Renderer::Renderer(int _w, int _h) :
deviceW(_w),
deviceH(_h)
{
	srand(time(0));
	InitShaders();
	//cout << "Renderer::Renderer: " << deviceW << "x" << deviceH << endl;
	glViewport(0,0, deviceW, deviceH);
}

Renderer::~Renderer(void)
{
}

void Renderer::InitShaders()
{
	oglPrograms[FLAT] = InitShader("Shaders/lighting_vs.glsl", "Shaders/basic_fs.glsl");
	oglPrograms[GOURAUD] = InitShader("Shaders/lighting_vs.glsl", "Shaders/basic_fs.glsl");
	oglPrograms[PHONG] = InitShader("Shaders/vnt_vs.glsl", "Shaders/phong_fs.glsl");
	oglPrograms[TOON] = InitShader("Shaders/vnt_vs.glsl", "Shaders/toon_fs.glsl");
	oglPrograms[SILHOUETTE] = InitShader("Shaders/silhouette_vs.glsl", "Shaders/basic_fs.glsl");
	oglPrograms[LINE] = InitShader("Shaders/line_vs.glsl", "Shaders/basic_fs.glsl");
	oglPrograms[ENV] = InitShader("Shaders/vnt_vs.glsl", "Shaders/env_fs.glsl");
}
//#pragma endregion

//#pragma region Draw pipeline
void Renderer::InitDraw()
{
	// clear buffer 
	// TODO: clear color to backgroundColor
	
	glClearColor(1,1,1,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void Renderer::FinishDraw()
{
	checkOpenGLerror();
	glutSwapBuffers();
}

void Renderer::SaveUniformLocations()
{
	uLoc.viewLoc =					glGetUniformLocation(oglPrograms[shading], "view");
	uLoc.projectionLoc =			glGetUniformLocation(oglPrograms[shading], "projection");
	uLoc.modelLoc =					glGetUniformLocation(oglPrograms[shading], "model");
	uLoc.normalTransformLoc =		glGetUniformLocation(oglPrograms[shading], "normalTransform");
	uLoc.emissiveLoc =				glGetUniformLocation(oglPrograms[shading], "emissive");
	uLoc.diffuseLoc =				glGetUniformLocation(oglPrograms[shading], "diffuse");
	uLoc.ambientLoc =				glGetUniformLocation(oglPrograms[shading], "ambient");
	uLoc.specularLoc =				glGetUniformLocation(oglPrograms[shading], "specular");
	uLoc.shininessLoc =				glGetUniformLocation(oglPrograms[shading], "shininess");
	uLoc.samplerLoc =				glGetUniformLocation(oglPrograms[shading], "texMap");
	uLoc.useTexLoc =				glGetUniformLocation(oglPrograms[shading], "useTex");
	uLoc.envCubeMapLoc =			glGetUniformLocation(oglPrograms[shading], "envCubeMap");
	uLoc.useNormalMapLoc =			glGetUniformLocation(oglPrograms[shading], "useNormalMap");
	uLoc.normalMapLoc =				glGetUniformLocation(oglPrograms[shading], "normalMap");
	uLoc.useVertexAnimation =		glGetUniformLocation(oglPrograms[shading], "useVertexAnimation");
	uLoc.vertexAnimationParamLoc =	glGetUniformLocation(oglPrograms[shading], "vertexAnimationParam");
}

void Renderer::SetShading(ShadingType _type)
{
	shading = _type;
	glUseProgram(oglPrograms[shading]);
	SaveUniformLocations();
}
ShadingType Renderer::Shading()
{
	return shading;
}
void Renderer::FinishShading()
{
	checkOpenGLerror();
	glUseProgram(0);
}

void Renderer::EnableFrontFaceCull()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
}
void Renderer::DisableFrontFaceCull()
{
	glDisable(GL_CULL_FACE);
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
//#pragma endregion

//#pragma region Camera
void Renderer::SetCamera(mat4 view, mat4 projection)
{
	glUniformMatrix4fv( uLoc.viewLoc, 1, GL_TRUE, view);
	glUniformMatrix4fv( uLoc.projectionLoc, 1, GL_TRUE, projection);
}
//#pragma endregion

//#pragma region Lights
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

		//cout << "Renderer::SetParallelLights: (" << lightDirections[i].x << ", " << lightDirections[i].y << ", " << lightDirections[i].z << ")";
		//cout << " RGB: [" << lightColors[i].x << ", " << lightColors[i].y << ", " << lightColors[i].z << "]" << endl;
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

		//cout << "Renderer::SetPointLights: (" << lightPositions[i].x << ", " << lightPositions[i].y << ", " << lightPositions[i].z << ")" << endl;
		//cout << " RGB: [" << lightColors[i].x << ", " << lightColors[i].y << ", " << lightColors[i].z << "]" << endl;
	}
	int pointLightNumLoc = glGetUniformLocation(program, "pointLightNum");
	glUniform1i(pointLightNumLoc, bound);
}
//#pragma endregion

//#pragma region Model
void Renderer::BindModel(MeshModel* model)
{
	ModelBind b;
	b.size = 8;
	glGenTextures(1, &(b.texture));
	glGenTextures(1, &(b.envTexture));
	glGenTextures(1, &(b.normalTexture));

	vector<vec4> pts = model->Triangles();
	vector<vec4> vnormals = model->VertexNormals(); 
	vector<vec4> fnormals = model->FaceNormals();
	vector<vec2> textures = model->TextureCoords();
	vector<vec3> tangents;
	vector<vec3> bitangents;
	model->TangentBitangent(tangents, bitangents);
	vector<vec4> avgnormals = model->AverageVertexNormals();
	vector<vec3> vrands = model->Randoms();

	if( 0 >= pts.size())
	{
		model->_oglBind = b;
		return;
	}

	b.buffers = new GLuint[b.size];
	glGenVertexArrays(1, &(b.vao));
	glBindVertexArray(b.vao);
	glGenBuffers(b.size, b.buffers);

	glBindBuffer(GL_ARRAY_BUFFER, b.buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * pts.size(), &pts[0], GL_STATIC_DRAW);

	if( vnormals.size()  < pts.size())
	{
		glDeleteBuffers(1, (&b.buffers[1]) );
		b.buffers[1] = -1;
	}
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, b.buffers[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * vnormals.size(), &vnormals[0], GL_STATIC_DRAW);
	}

	glBindBuffer(GL_ARRAY_BUFFER, b.buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * fnormals.size(), &fnormals[0], GL_STATIC_DRAW);
	
	if( textures.size()  < pts.size())
	{
		glDeleteBuffers(1, (&b.buffers[3]) );
		b.buffers[3] = -1;
	}
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, b.buffers[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * textures.size(), &textures[0], GL_STATIC_DRAW);
	}

	if( tangents.size()  < pts.size() || bitangents.size() < pts.size())
	{
		glDeleteBuffers(1, (&b.buffers[4]) );
		b.buffers[4] = -1;
		glDeleteBuffers(1, (&b.buffers[5]) );
		b.buffers[5] = -1;
	}
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, b.buffers[4]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * tangents.size(), &tangents[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, b.buffers[5]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * bitangents.size(), &bitangents[0], GL_STATIC_DRAW);
	}

	if( avgnormals.size() < pts.size())
	{
		glDeleteBuffers(1, (&b.buffers[6]) );
		b.buffers[6] = -1;
	}
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, b.buffers[6]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * avgnormals.size(), &avgnormals[0], GL_STATIC_DRAW);
	}
	if( vrands.size() < pts.size())
	{
		glDeleteBuffers(1, (&b.buffers[7]) );
		b.buffers[7] = -1;
	}
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, b.buffers[7]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vrands.size(), &vrands[0], GL_STATIC_DRAW);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	model->_oglBind = b;
}

void Renderer::BindTexture(MeshModel* m , Texture& t)
{
	glActiveTexture(GL_TEXTURE0);
	if( -1 != m->_oglBind.texture)
	{
		glGenTextures(1, &(m->_oglBind.texture));
	}
	glBindTexture(GL_TEXTURE_2D, m->_oglBind.texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t.width, t.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(t.img[0]));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D );
	glBindTexture(GL_TEXTURE_2D, 0);
	checkOpenGLerror();
}

void Renderer::BindNormalTexture(MeshModel* m , Texture& t)
{
	glActiveTexture(GL_TEXTURE2);
	if( -1 == m->_oglBind.normalTexture)
	{
		glGenTextures(1, &(m->_oglBind.normalTexture));
	}
	glBindTexture(GL_TEXTURE_2D, m->_oglBind.normalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t.width, t.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(t.img[0]));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D );
	glBindTexture(GL_TEXTURE_2D, 0);
	checkOpenGLerror();
}

void Renderer::CopyFrameToTexture(GLenum dir, MeshModel* m)
{
	glActiveTexture(GL_TEXTURE1);
	if( -1 == m->_oglBind.envTexture)
	{
		glGenTextures(1, &(m->_oglBind.envTexture));
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, m->_oglBind.envTexture);

	GLint m_viewport[4];
	glGetIntegerv( GL_VIEWPORT, m_viewport );
	int side = min(m_viewport[3],m_viewport[2]);
	int size = side*side * 4;
	if(size> 0)
	{
		glCopyTexImage2D(dir, 0, GL_RGB, 0, 0, side, side, 0);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP );
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Renderer::UnbindModel(MeshModel* mb)
{
	glDeleteTextures(1, &(mb->_oglBind.texture));
	glDeleteTextures(1, &(mb->_oglBind.envTexture));
	glDeleteTextures(1, &(mb->_oglBind.normalTexture));
	for( int i = 0; i < mb->_oglBind.size; i++)
	{
		if( -1 != mb->_oglBind.buffers[i])
			glDeleteBuffers(1, &(mb->_oglBind.buffers[i]));
	}
	mb->_oglBind.size = 0;
	delete[] mb->_oglBind.buffers;
	mb->_oglBind.buffers = NULL;
	glDeleteVertexArrays(1, &(mb->_oglBind.vao));
	mb->_oglBind.vao = 0;
}


void Renderer::SetModelUniforms(MeshModel* m)
{
	glUniformMatrix4fv(uLoc.modelLoc, 1, GL_TRUE, m->Transformation());
	glUniformMatrix4fv(uLoc.normalTransformLoc, 1, GL_TRUE, m->NormalTransformation());
	
	MaterialColor mc = m->GetDefaultColor();
	glUniform3f(uLoc.ambientLoc, mc.ambient.r, mc.ambient.g, mc.ambient.b);
	glUniform3f(uLoc.diffuseLoc, mc.diffuse.r, mc.diffuse.g, mc.diffuse.b);
	glUniform3f(uLoc.emissiveLoc, mc.emissive.r, mc.emissive.g, mc.emissive.b);
	glUniform3f(uLoc.specularLoc, mc.specular.r, mc.specular.g, mc.specular.b);
	glUniform1f(uLoc.shininessLoc, mc.shininess);
	
	
	if(m->GetDrawTexture() && -1 != m->_oglBind.buffers[3])
	{
		glUniform1i(uLoc.useTexLoc, m->GetDrawTexture());
		glBindTexture( GL_TEXTURE_2D, m->_oglBind.texture);
		glUniform1i(uLoc.samplerLoc, 0);
		checkOpenGLerror();
	}
	else
	{
		glUniform1i(uLoc.useTexLoc, false);
	}
	
	if ( m->GetDrawEnvMap())
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture( GL_TEXTURE_CUBE_MAP, m->_oglBind.envTexture);
		glUniform1i(uLoc.envCubeMapLoc, 1);
	}

	
	if(m->GetNormalMap() && -1 != m->_oglBind.buffers[3] && -1 != m->_oglBind.normalTexture)
	{
		glUniform1i(uLoc.useNormalMapLoc, m->GetNormalMap());
		glActiveTexture(GL_TEXTURE2);
		glBindTexture( GL_TEXTURE_2D, m->_oglBind.normalTexture);
		glUniform1i(uLoc.normalMapLoc , 2);
	}
	else
	{
		glUniform1i(uLoc.useNormalMapLoc, false);
	}

	bool anim = m->GetVertexAnimation();
	glUniform1i(uLoc.useVertexAnimation, m->GetVertexAnimation());
	vec3 param = normalize( vec3( rand(), rand(), rand()));
	glUniform3f(uLoc.vertexAnimationParamLoc, param.x, param.y, param.z);

}

void Renderer::SetModelvao(MeshModel* m)
{
	glBindVertexArray(m->_oglBind.vao);

	GLuint vPositionLoc = glGetAttribLocation(oglPrograms[shading], "vPosition");
	glBindBuffer(GL_ARRAY_BUFFER, m->_oglBind.buffers[0]);
	glEnableVertexAttribArray(vPositionLoc);
	glVertexAttribPointer(vPositionLoc, 4, GL_FLOAT, 0, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER,0);

	GLuint avgNormalLoc = glGetAttribLocation(oglPrograms[shading], "avgNormal");
	if( -1 != avgNormalLoc && -1 != m->_oglBind.buffers[6])
	{
		glBindBuffer(GL_ARRAY_BUFFER, m->_oglBind.buffers[6]);
		glEnableVertexAttribArray(avgNormalLoc);
		glVertexAttribPointer(avgNormalLoc, 4, GL_FLOAT, 0, 0, 0);
	}

	GLuint vNormalLoc = glGetAttribLocation(oglPrograms[shading], "vNormal");
	if( vNormalLoc != -1 && FLAT != shading && -1 != m->_oglBind.buffers[1])
	{
		glBindBuffer(GL_ARRAY_BUFFER, m->_oglBind.buffers[1]);
		glEnableVertexAttribArray(vNormalLoc);
		glVertexAttribPointer(vNormalLoc, 4, GL_FLOAT, 0, 0, 0);
	} else if( -1 != m->_oglBind.buffers[2]  && vNormalLoc != -1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m->_oglBind.buffers[2]);
		glEnableVertexAttribArray(vNormalLoc);
		glVertexAttribPointer(vNormalLoc, 4, GL_FLOAT, 0, 0, 0);
	}
	
	GLuint vTexlLoc = glGetAttribLocation(oglPrograms[shading], "vTex");
	if (vTexlLoc != -1 && -1 != m->_oglBind.buffers[3]) {
		glBindBuffer(GL_ARRAY_BUFFER, m->_oglBind.buffers[3]);
		glEnableVertexAttribArray(vTexlLoc);
		glVertexAttribPointer(vTexlLoc, 2, GL_FLOAT, 0, 0, 0);
	}

	GLuint vTanlLoc = glGetAttribLocation(oglPrograms[shading], "vTan");
	GLuint vBitanlLoc = glGetAttribLocation(oglPrograms[shading], "vBitan");
	if (vTanlLoc != -1 && vBitanlLoc != -1 && -1 != m->_oglBind.buffers[4] && -1 != m->_oglBind.buffers[5]) {
		glBindBuffer(GL_ARRAY_BUFFER, m->_oglBind.buffers[4]);
		glEnableVertexAttribArray(vTanlLoc);
		glVertexAttribPointer(vTanlLoc, 3, GL_FLOAT, 0, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, m->_oglBind.buffers[5]);
		glEnableVertexAttribArray(vBitanlLoc);
		glVertexAttribPointer(vBitanlLoc, 3, GL_FLOAT, 0, 0, 0);
	}

	GLuint vRandlLoc = glGetAttribLocation(oglPrograms[shading], "vRand");
	if (vRandlLoc != -1 && -1 != m->_oglBind.buffers[7]) {
		glBindBuffer(GL_ARRAY_BUFFER, m->_oglBind.buffers[7]);
		glEnableVertexAttribArray(vRandlLoc);
		glVertexAttribPointer(vRandlLoc, 3, GL_FLOAT, 0, 0, 0);
	}
}

void Renderer::DrawModel(MeshModel* m)
{
	GLuint program = oglPrograms[shading];
	if (program < 0)
	{
		throw std::exception("DrawTriangles called with no program set");
	}

	SetModelUniforms(m);
	SetModelvao(m);
	
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, m->FaceCount() * 3);
	glBindVertexArray(0);

}
//#pragma endregion

//#pragma region Lines
GLuint Renderer::BindLineBuffer(vector<vec4> verteces, vector<vec3> colors)
{
	GLuint oglLineProgram = oglPrograms[LINE];

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
	GLuint oglLineProgram = oglPrograms[LINE];
	if (oglLineProgram < 0)
	{
		throw std::exception("DrawWFLines program is invalid");
	}
	GLuint vao = BindLineBuffer(verteces, colors);
	
	glUseProgram(oglLineProgram);
	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(vao );
	glLineWidth(2);
	glDrawArrays(GL_LINES, 0, min(verteces.size(), colors.size()));
	glUseProgram(0);
	glDeleteVertexArrays(1, &vao);
	return;
}
//#pragma endregion

//#pragma region Deprecated
//void Renderer::SetUniformMatrices(vector<GLuint> handles, vector<mat4> values)
//{
//	GLuint program = oglPrograms[shading];
//	if (program < 0 || handles.size() != values.size() || handles.size() == 0)
//	{
//		throw std::exception("Bad arguments in SetUniformMatrices");
//	}
//	for (unsigned int i = 0; i < handles.size(); i++)
//	{
//		glUniformMatrix4fv(handles[i], 1, GL_TRUE, values[i]);
//	}
//}
//
//void Renderer::SetUniformVec3(GLuint handle, vec3 val)
//{
//	GLuint program = oglPrograms[shading];
//	if (program < 0)
//	{
//		throw std::exception("Bad arguments in SetUniformMatrices");
//	}
//	glUniform3f(handle, val.x, val.y, val.z);
//}
//
//void Renderer::SetUniformVec2(GLuint handle, vec2 val)
//{
//	GLuint program = oglPrograms[shading];
//	if (program < 0)
//	{
//		throw std::exception("Bad arguments in SetUniformMatrices");
//	}
//	glUniform2f(handle, val.x, val.y);
//}
//
//void Renderer::SetUniform1b(GLuint handle, bool val)
//{
//	GLuint program = oglPrograms[shading];
//	if (program < 0)
//	{
//		throw std::exception("Bad arguments in SetUniformMatrices");
//	}
//	glUniform1i(handle, val);
//}
//
//void Renderer::SetUniform1i(GLuint handle, int val)
//{
//	GLuint program = oglPrograms[shading];
//	if (program < 0)
//	{
//		throw std::exception("Bad arguments in SetUniformMatrices");
//	}
//	glUniform1i(handle, val);
//
//}
//
//void Renderer::SetUniform(GLuint handle, float val)
//{
//	GLuint program = oglPrograms[shading];
//	if (program < 0)
//	{
//		throw std::exception("Bad arguments in SetUniformMatrices");
//	}
//	glUniform1f(handle, val);
//}
//
//void Renderer::SetUniformMatrix(GLuint handle, mat4 val)
//{
//	GLuint program = oglPrograms[shading];
//	if (program < 0)
//	{
//		throw std::exception("Bad arguments in SetUniformMatrix");
//	}
//	glUniformMatrix4fv(handle, 1, GL_TRUE, val);
//}
//
//void Renderer::DrawTriangles(GLuint vao, int count)
//{
//	glEnable(GL_DEPTH_TEST);
//	glBindVertexArray(vao);
//	glDrawArrays(GL_TRIANGLES, 0, count);
//}

//#pragma endregion