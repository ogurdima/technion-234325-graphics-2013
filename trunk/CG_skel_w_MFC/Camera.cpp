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
	vec4 n = normalize( eye - at);
	vec4 u = normalize(cross(up,n));
    vec4 v = normalize(cross(n,u));
    vec4 t = vec4(0, 0, 0, 1);
	cTransform = mat4(u, v, n, t) * Translate( -eye );
}

void Camera::Frustum(	const float left, const float right,
						const float bottom, const float top,
						const float zNear, const float zFar )
{
	projection = mat4(	(2*zNear/(right - left)), 0, (right + left)/(right - left), 0,
						0, 2*zNear/(top - bottom), (top + bottom)/(top - bottom),0,
						0,0, -(zFar + zNear)/(zFar - zNear), -2*zFar*zNear/(zFar - zNear),
						0,0,-1,0);
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

mat4 Camera::Transformation()
{
	return cTransform;
}


