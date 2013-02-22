#pragma once
#include "gl/glew.h"
#include "gl/glut.h"


typedef struct
{
	GLuint		vao;
	int			size;
	GLuint*		buffers;
	GLuint		texture;
	GLuint		envTexture;
	GLuint		normalTexture;
} ModelBind;