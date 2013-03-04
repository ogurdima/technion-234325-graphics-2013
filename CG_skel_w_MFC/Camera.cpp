#include "StdAfx.h"
#include "Camera.h"

Camera::Camera() :
	eye(vec4(-1,0,0,0)),
	at(vec4(0,0,0,0)),
	up(vec4(0,0,1,0)),
	aspect(4/3),
	fovy(M_PI/6),
	zNear(0.5),
	zFar(100)
{
	LookAt(eye, at, up);
	Perspective(fovy, aspect, zNear, zFar);
}

void Camera::LookAt(const vec4& eye, const vec4& at, const vec4& up )
{
	this->eye = eye;
	this->at = at;
	this->up = normalize(up);
}

void Camera::Frustum(	const float left, const float right,
						const float bottom, const float top,
						const float zNear, const float zFar )
{
	lensMode = FRUSTUM;
	this->left = left;
	this->right = right;
	this->bottom = bottom;
	this->top = top;
	this->zNear = zNear;
	this->zFar = zFar;
	this->aspect = (right - left) / (top - bottom);
}

void Camera::Perspective(	const float fovy, const float aspect,
							const float zNear, const float zFar)
{
	float rady = ( fovy/ 180.0) * M_PI;
	this->zNear = zNear;
	this->zFar = zFar;
	this->fovy = fovy;
	this->aspect = aspect;

	float x = tan(rady/2);
	this->left = -x/aspect;
	this->right = -left;
	this->bottom = -x;
	this->top = x;
	lensMode = PERSPECTIVE;
}

void Camera::Ortho( const float left, const float right,
					const float bottom, const float top,
					const float zNear, const float zFar )
{
	lensMode = ORTHO;
	this->left = left;
	this->right = right;
	this->bottom = bottom;
	this->top = top;
	this->zNear = zNear;
	this->zFar = zFar;
	this->aspect = (right - left) / (top - bottom);
}

mat4 Camera::Projection()
{
	switch (lensMode)
	{
	case ORTHO:
		return Scale(2/(right - left), 2/(top - bottom), 2/(zNear - zFar)) *  Translate(-(right+left)/2,-(top  + bottom)/2, (zFar + zNear)/2);
	case FRUSTUM:
	case PERSPECTIVE:
		return mat4(	(2*zNear/(right - left)), 0, (right + left)/(right - left), 0,
						0, 2*zNear/(top - bottom), (top + bottom)/(top - bottom),0,
						0,0, -(zFar + zNear)/(zFar - zNear), -2*zFar*zNear/(zFar - zNear),
						0,0,-1,0);
	}
}

mat4 Camera::View()
{
	return LookAtMat(eye, at, up);

	vec4 n = normalize( eye - at);
	vec4 u = vec4(normalize(cross(up,n)), 0);
    vec4 v = vec4(cross(n,u), 0);
    vec4 t = vec4(0, 0, 0, 1);
	return mat4(u, v, n, t) * Translate( -eye );
}

void Camera::Zoom(float zoomFactor)
{
	right *= zoomFactor;
	left *= zoomFactor;
	top *= zoomFactor;
	bottom *= zoomFactor;
}
