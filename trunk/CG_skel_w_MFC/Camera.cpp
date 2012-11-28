#include "StdAfx.h"
#include "Camera.h"

Camera::Camera()
{
	cTransform = Identity4();
	lensMode = FRUSTUM;
}

void Camera::setTransformation(const mat4& transform)
{
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
}

void Camera::Perspective(	const float fovy, const float aspect,
							const float zNear, const float zFar)
{
	lensMode = PERSPECTIVE;
	this->fovy = fovy;
	this->aspect = aspect;
	this->zNear = zNear;
	this->zFar = zFar;
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
}

mat4 Camera::Projection()
{
	switch (lensMode)
	{
	case ORTHO:
		return Scale(2/(right - left), 2/(top - bottom), 2/(zNear - zFar)) *  Translate(-(right+left)/2,-(top  + bottom)/2, (zFar + zNear)/2);
	case FRUSTUM:
		return mat4(	(2*zNear/(right - left)), 0, (right + left)/(right - left), 0,
						0, 2*zNear/(top - bottom), (top + bottom)/(top - bottom),0,
						0,0, -(zFar + zNear)/(zFar - zNear), -2*zFar*zNear/(zFar - zNear),
						0,0,-1,0);
	case PERSPECTIVE:
		return mat4();
	}
}

mat4 Camera::Transformation()
{
	vec4 n = normalize( eye - at);
	vec4 u = vec4(normalize(cross(up,n)), 1);
    vec4 v = vec4(normalize(cross(n,u)), 1);
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
