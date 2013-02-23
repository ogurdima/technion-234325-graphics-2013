#pragma once
#include "gl/glew.h"
#include "gl/glut.h"


class ModelBind
{
public:
	GLuint		vao;
	int			size;
	GLuint*		buffers;
	GLuint		texture;
	GLuint		envTexture;
	GLuint		normalTexture;

	ModelBind():
	vao(-1),
	size(0),
	buffers(NULL),
	texture(-1),
	envTexture(-1),
	normalTexture(-1)
	{}
};