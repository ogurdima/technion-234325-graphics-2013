#include "StdAfx.h"
#include "Camera.h"

Camera::Camera()
{
	cTransform = Identity4();
}

void Camera::setTransformation(const mat4& transform)
{
}

void Camera::LookAt(const vec4& eye, const vec4& at, const vec4& up )
{
}

void Camera::Frustum(	const float left, const float right,
						const float bottom, const float top,
						const float zNear, const float zFar )
{
}
void Camera::Perspective(	const float fovy, const float aspect,
							const float zNear, const float zFar)
{
}

void Camera::Ortho( const float left, const float right,
					const float bottom, const float top,
					const float zNear, const float zFar )
{
	projection = Scale(2/(right - left), 2/(top - bottom), 2/(zNear - zFar)) * 
		Translate(-(right+left)/2,-(top  + bottom)/2, (zFar + zNear)/2);
}

mat4 Camera::Projection()
{
	return projection;
}
