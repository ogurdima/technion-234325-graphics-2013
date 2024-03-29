#pragma once

#include "vec.h"
#include "mat.h"

enum LensMode { ORTHO, PERSPECTIVE, FRUSTUM };

class Camera
{
	LensMode lensMode;

	vec4 eye, at, up;
	float right, left, top, bottom, zNear, zFar, fovy,aspect;  

	void LookAt(const vec4& eye, const vec4& at, const vec4& up );

public:
	Camera();

	inline void LookAt(const vec3& eye, const vec3& at, const vec3& up ) {
		LookAt(vec4(eye, 0), vec4(at, 0), vec4(up,0) );
	}
	void Ortho( const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar );
	void Frustum( const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar );
	void Perspective( const float fovy, const float aspect,
		const float zNear, const float zFar);
	void Zoom(float zoomFactor);
	mat4 Projection();
	mat4 View();

	inline LensMode getLensMode() { return lensMode; }
	inline vec3 Eye() { return vec3(eye.x, eye.y, eye.z); }
	inline vec3 At() { return vec3(at.x, at.y, at.z); }
	inline vec3 Up() { return vec3(up.x, up.y, up.z); }
	inline float Right() { return right; }
	inline float Left() { return left; }
	inline float Top() { return top; }
	inline float Bottom() { return bottom; }
	inline float ZNear() { return zNear; }
	inline float ZFar() { return zFar; }
	inline float Fovy() { return fovy; }
	inline float Aspect() { return aspect; }

};

