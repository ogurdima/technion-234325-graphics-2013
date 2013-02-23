#include "stdafx.h"
#include "CG_skel_w_MFC.h"
#include <sstream>
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GL/freeglut_ext.h"
#include "vec.h"
#include "mat.h"
#include "InitShader.h"
#include "Scene.h"
#include "Renderer.h"
#include <string>
#include "InputDialog.h"
#include "PrimMeshModel.h"
#include "MColorDialog.h"
#include "MaterialColor.h"
#include "ColorSelector.h"
#include "lodepng.h"
#include <ctime>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef enum { MODEL_MODE = 0, CAMERA_MODE, LIGHT_MODE} ActiveMode;
typedef enum { M_WORLD_FRAME = 0, M_MODEL_FRAME } ModelActiveFrame;
typedef enum { C_WORLD_FRAME = 0, C_VIEW_FRAME } CameraActiveFrame;
typedef enum { T_ROTATION = 0, T_TRANSLATION } ActiveTransformation;


#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

#define MAIN_SHOW_WORLD_FRAME					1
#define MAIN_RENDER_CAMERAS						2
#define MAIN_RENDER_LIGHTS						3
#define MAIN_REMOVE_GEOMETRY					4
#define MAIN_REMOVE_CAMERAS						5
#define MAIN_REMOVE_LIGHTS						6
#define MAIN_ADD_CAMERA							7
#define MAIN_ADD_MODEL							8
#define MAIN_ADD_PRIMITIVE						9						
#define MAIN_SET_BACKGROUND_COLOR				10
#define MAIN_RENDER_SILHOUETTE					11


#define MODEL_SHOW_VERTEX_NORMALS				200
#define MODEL_SHOW_FACE_NORMALS					201
#define MODEL_SHOW_BOUNDING_BOX					202
#define MODEL_SHOW_FRAME						203
#define MODEL_NON_UNIFORM_SCALE					204
#define MODEL_SET_MONOTON_COLOR					205
#define MODEL_SET_TEXTURE						206
#define MODEL_ENABLE_ENV_MAP					207
#define MODEL_DISABLE_ENV_MAP					208
#define MODEL_ENABLE_SPHERICAL_TEX_COORD		209
#define MODEL_ENABLE_NORMAL_MAP					210
#define MODEL_DISABLE_NORMAL_MAP				211
#define MODEL_ENABLE_VERTEX_ANIM				212
#define MODEL_DISABLE_VERTEX_ANIM				213
#define MODEL_ENABLE_COLOR_ANIM					214
#define MODEL_DISABLE_COLOR_ANIM				215

#define CAMERA_SET_LOCATION						30
#define CAMERA_SET_FOV							31
#define CAMERA_SET_FOCUS_POINT					32
#define CAMERA_FOCUS_ON_ACTIVE_MODEL			33
#define CAMERA_SET_TYPE							34

#define LENS_ORTHO								341
#define LENS_PERSPECTIVE						342
#define LENS_FRUSTUM							343

#define RENDERER_SHADING_WIREFRAME				401
#define RENDERER_SHADING_FLAT					402
#define RENDERER_SHADING_GOURAUD				403
#define RENDERER_SHADING_PHONG					404
#define RENDERER_SET_ANTIALIASING				405
#define RENDERER_TOGGLE_FOG						406
#define RENDERER_SET_FOG_COLOR					407
#define RENDERER_SHADING_TOON					408

#define LIGHT_AMBIENT							50
#define LIGHT_POINT_SOURCE						51
#define LIGHT_PARALLEL_SOURCE					52

using namespace std;
//--------------Global variables-----------------------------------------------
CWinApp theApp;
Scene *scene;
Renderer *renderer;
int last_x,last_y;
bool lb_down, rb_down, mb_down, ctr_down, shift_down, alt_down;
float smoothFactor = 2;
int timerInterval = 100;

ActiveMode activeMode = MODEL_MODE;
ModelActiveFrame modelActiveFrame = M_WORLD_FRAME;
CameraActiveFrame cameraActiveFrame = C_WORLD_FRAME;
ActiveTransformation activeTransformation  = T_TRANSLATION;

void parseElem(string cmd, float& zNear, float& zFar, float& top, float& bot, float& left, float& right, float& fovy, float& aspect) 
{
	if (cmd.empty())
		return;
	int eqPos = cmd.find_first_of("=");
	if (eqPos == -1)
		return;
	string operand = cmd.substr(0, eqPos);
	string val = cmd.substr(eqPos + 1, cmd.size());
	float fval;
	std::stringstream iss(val);
	iss >> fval;

	if (operand == "n") 
	{
		zNear = fval;
	}
	else if (operand == "f")
	{
		 zFar = fval;
	}
	else if (operand == "l")
	{
		 left = fval;
	}
	else if (operand == "r")
	{
		 right = fval;
	}
	else if (operand == "t")
	{
		 top = fval;
	}
	else if (operand == "b")
	{
		 bot = fval;
	}
	else if (operand == "fovy")
	{
		 fovy = fval;
	}
	else if (operand == "aspect")
	{
		 aspect = fval;
	}
}

void parseOrthoElem(string cmd)
{
	float zNear = scene->ActiveCam()->ZNear();
	float zFar = scene->ActiveCam()->ZFar();
	float top = scene->ActiveCam()->Top();
	float bot = scene->ActiveCam()->Bottom();
	float left = scene->ActiveCam()->Left();
	float right = scene->ActiveCam()->Right();
	float fovy = scene->ActiveCam()->Fovy();
	float aspect = scene->ActiveCam()->Aspect();

	parseElem(cmd, zNear, zFar, top, bot, left, right, fovy, aspect);
	scene->ActiveCam()->Ortho(left, right, bot, top, zNear, zFar);
}

void parseOrthoCmd(string cmd)
{
	if (cmd.empty())
		return;
	int commaPos = cmd.find_first_of(",");
	if (commaPos == -1)
	{
		parseOrthoElem(cmd);
		return;
	}
	string before = cmd.substr(0, commaPos);
	parseOrthoElem(before);
	string after = cmd.substr(commaPos + 1, cmd.size());
	parseOrthoCmd(after);
}

void parsePerspectiveElem(string cmd)
{
	float zNear = scene->ActiveCam()->ZNear();
	float zFar = scene->ActiveCam()->ZFar();
	float top = scene->ActiveCam()->Top();
	float bot = scene->ActiveCam()->Bottom();
	float left = scene->ActiveCam()->Left();
	float right = scene->ActiveCam()->Right();
	float fovy = scene->ActiveCam()->Fovy();
	float aspect = scene->ActiveCam()->Aspect();

	parseElem(cmd, zNear, zFar, top, bot, left, right, fovy, aspect);
	scene->ActiveCam()->Perspective(fovy, aspect,zNear, zFar);
}

void parsePerspectiveCmd(string cmd)
{
	if (cmd.empty())
		return;
	int commaPos = cmd.find_first_of(",");
	if (commaPos == -1)
	{
		parsePerspectiveElem(cmd);
		return;
	}
	string before = cmd.substr(0, commaPos);
	parsePerspectiveElem(before);
	string after = cmd.substr(commaPos + 1, cmd.size());
	parsePerspectiveCmd(after);
}

void parseFrustumElem(string cmd)
{
	float zNear = scene->ActiveCam()->ZNear();
	float zFar = scene->ActiveCam()->ZFar();
	float top = scene->ActiveCam()->Top();
	float bot = scene->ActiveCam()->Bottom();
	float left = scene->ActiveCam()->Left();
	float right = scene->ActiveCam()->Right();
	float fovy = scene->ActiveCam()->Fovy();
	float aspect = scene->ActiveCam()->Aspect();

	parseElem(cmd, zNear, zFar, top, bot, left, right, fovy, aspect);
	scene->ActiveCam()->Frustum(left, right, bot, top, zNear, zFar);
}

void parseFrustumCmd(string cmd)
{
	if (cmd.empty())
		return;
	int commaPos = cmd.find_first_of(",");
	if (commaPos == -1)
	{
		parseFrustumElem(cmd);
		return;
	}
	string before = cmd.substr(0, commaPos);
	parseFrustumElem(before);
	string after = cmd.substr(commaPos + 1, cmd.size());
	parseFrustumCmd(after);
}

// callbacks
void display( void )
{
	scene->Draw();
}

void reshape( int width, int height )
{
	// renderer->CreateBuffers(width, height);
	Camera* ac = scene->ActiveCam();
	float windowAR = (float) width / (float) height;
	float x = abs(ac->Left()) + abs(ac->Right());
	float y = x / windowAR;
	switch (ac->getLensMode())
	{
		case ORTHO:
			ac->Ortho(-x/2, x/2, -y/2, y/2, ac->ZNear(), ac->ZFar());
			break;
		case PERSPECTIVE:
		case FRUSTUM:
			ac->Frustum(-x/2, x/2, -y/2, y/2, ac->ZNear(), ac->ZFar());
			break;
	}
	
	glutPostRedisplay();
}

mat4 customRotationMatrix(vec3 rotationAxis, double factor)
{
	rotationAxis = normalize(rotationAxis);
	float angle = 5 * factor * M_PI / 180.0; //converting to radian value
	float u = rotationAxis.x;
	float v = rotationAxis.y;
	float w = rotationAxis.z;
	float u2 = u * u;
	float v2 = v * v;
	float w2 = w * w;
	float L = (u*u + v * v + w * w);

	mat4 rotationMatrix;
	rotationMatrix[0][0] = (u2 + (v2 + w2) * cos(angle)) / L;
	rotationMatrix[0][1] = (u * v * (1 - cos(angle)) - w * sqrt(L) * sin(angle)) / L;
	rotationMatrix[0][2] = (u * w * (1 - cos(angle)) + v * sqrt(L) * sin(angle)) / L;
	rotationMatrix[0][3] = 0.0; 
 
	rotationMatrix[1][0] = (u * v * (1 - cos(angle)) + w * sqrt(L) * sin(angle)) / L;
	rotationMatrix[1][1] = (v2 + (u2 + w2) * cos(angle)) / L;
	rotationMatrix[1][2] = (v * w * (1 - cos(angle)) - u * sqrt(L) * sin(angle)) / L;
	rotationMatrix[1][3] = 0.0; 
 
	rotationMatrix[2][0] = (u * w * (1 - cos(angle)) - v * sqrt(L) * sin(angle)) / L;
	rotationMatrix[2][1] = (v * w * (1 - cos(angle)) + u * sqrt(L) * sin(angle)) / L;
	rotationMatrix[2][2] = (w2 + (u2 + v2) * cos(angle)) / L;
	rotationMatrix[2][3] = 0.0; 
 
	rotationMatrix[3][0] = 0.0;
	rotationMatrix[3][1] = 0.0;
	rotationMatrix[3][2] = 0.0;
	rotationMatrix[3][3] = 1.0;
	
	return rotationMatrix;
}
void customWFRotateModel(MeshModel* am, mat4 rotationMatrix)
{
	vec3 orig = am->Origin();
	am->WFTranslate(Translate(-orig));
	am->WFRotate(rotationMatrix);
	am->WFTranslate(Translate(orig));
}
bool tryGeneralKeyboardCommands(unsigned char key)
{
	switch ( key ) {
	case 'm':
		activeMode = MODEL_MODE;
		return true;
	case 'l':
		activeMode = LIGHT_MODE;
		return true;
	case 'c':
		activeMode = CAMERA_MODE;
		return true;
	case 'r':
		activeTransformation = T_ROTATION;
		return true;
	case 't':
		activeTransformation = T_TRANSLATION;
		return true;
	case ',':
		if( smoothFactor <= 0)
			smoothFactor = 1;
		smoothFactor /= 2;
		return true;
	case '.':
		if( smoothFactor <= 0)
			smoothFactor = 1;
		smoothFactor *= 2;
		return true;
	}
	return false;
}
void modelWFTranslation(unsigned char key)
{
	MeshModel* mm = scene->ActiveModel();
	Camera* c = scene->ActiveCam();
	if( NULL == mm || NULL == c)	return;

	vec3 forwardDirection = normalize( c->At() - c->Eye());
	vec3 rightDirection = normalize( cross( forwardDirection, c->Up()));
	vec3 upDirection = normalize( cross( rightDirection, forwardDirection));
	switch (key)
	{
	case 'w':
		mm->WFTranslate( Translate( forwardDirection * (smoothFactor)));
		return;
	case 's':
		mm->WFTranslate( Translate(forwardDirection * (-smoothFactor)));
		return;
	case 'a':
		mm->WFTranslate( Translate(rightDirection * (-smoothFactor)));
		return;
	case 'd':
		mm->WFTranslate( Translate(rightDirection * (smoothFactor)));
		return;
	case 'e':
		mm->WFTranslate( Translate(upDirection * (smoothFactor)));
		return;
	case 'q':
		mm->WFTranslate( Translate(upDirection * (-smoothFactor)));
		return;
	}
}
void modelWFRotation(unsigned char key)
{
	MeshModel* mm = scene->ActiveModel();
	Camera* c = scene->ActiveCam();
	if( NULL == mm || NULL == c)	return;

	vec3 forwardDirection = normalize( c->At() - c->Eye());
	vec3 rightDirection = normalize( cross( forwardDirection, c->Up()));
	vec3 upDirection = normalize( cross( rightDirection, forwardDirection));
	mat4 custRotation;
	switch (key)
	{
	case 'w':
		custRotation = customRotationMatrix(rightDirection, -smoothFactor);
		break;
	case 's':
		custRotation = customRotationMatrix(rightDirection, smoothFactor);
		break;
	case 'a':
		custRotation = customRotationMatrix(upDirection, -smoothFactor);
		break;
	case 'd':
		custRotation = customRotationMatrix(upDirection, smoothFactor);
		break;
	case 'e':
		custRotation = customRotationMatrix(forwardDirection, smoothFactor);
		break;
	case 'q':
		custRotation = customRotationMatrix(forwardDirection, -smoothFactor);
		break;
	default:
		return;
	}
	customWFRotateModel(mm, custRotation);
}
void modelMFTranslation(unsigned char key)
{
	MeshModel* mm = scene->ActiveModel();
	if( NULL == mm)	return;
	switch (key)
	{
	case 'w':
		mm->MFTranslate( Translate(0,0, -smoothFactor));
		return;
	case 's':
		mm->MFTranslate( Translate(0,0, smoothFactor));
		return;
	case 'a':
		mm->MFTranslate( Translate(-smoothFactor, 0,0));
		return;
	case 'd':
		mm->MFTranslate( Translate(smoothFactor, 0,0));
		return;
	case 'e':
		mm->MFTranslate( Translate(0, -smoothFactor, 0));
		return;
	case 'q':
		mm->MFTranslate( Translate(0, smoothFactor, 0));
		return;
	}
}
void modelMFRotation(unsigned char key)
{
	MeshModel* mm = scene->ActiveModel();
	if( NULL == mm)	return;
	float angle = 5 * smoothFactor * M_PI / 180.0;
	switch (key)
	{
	case 'w':
		mm->MFRotate( RotateX( -angle));
		return;
	case 's':
		mm->MFRotate( RotateX( angle));
		return;
	case 'a':
		mm->MFRotate( RotateY( angle));
		return;
	case 'd':
		mm->MFRotate( RotateY( -angle));
		return;
	case 'e':
		mm->MFRotate( RotateZ( -angle));
		return;
	case 'q':
		mm->MFRotate( RotateZ( angle));
		return;
	}
}
void modelKeyboard(unsigned char key)
{
	switch ( key ) {
	case '[':
		modelActiveFrame = M_WORLD_FRAME;
		return;
	case ']':
		modelActiveFrame = M_MODEL_FRAME;
		return;
	case '\t':
		scene->ToggleActiveModel();
		return;
	}
	if( M_WORLD_FRAME == modelActiveFrame)
	{
		if(T_ROTATION == activeTransformation)
			modelWFRotation(key);
		else if ( T_TRANSLATION == activeTransformation)
			modelWFTranslation(key);
	}
	else if( M_MODEL_FRAME == modelActiveFrame)
	{
		if(T_ROTATION == activeTransformation)
			modelMFRotation(key);
		else if ( T_TRANSLATION == activeTransformation)
			modelMFTranslation(key);
	}
}

void cameraWFRotation(unsigned char key)
{

}
void cameraWFTranslation(unsigned char key)
{
	Camera* c = scene->ActiveCam();
	if( NULL == c)	return;

	vec3 to;
	switch (key)
	{
	case 'w':
		to = vec3(0,0, -smoothFactor);
		break;
	case 's':
		to = vec3(0,0, smoothFactor);
		break;
	case 'a':
		to = vec3(-smoothFactor, 0, 0);
		break;
	case 'd':
		to = vec3(smoothFactor, 0, 0);
		break;
	case 'e':
		to = vec3(0, smoothFactor, 0);
		break;
	case 'q':
		to = vec3(0, -smoothFactor, 0);
		break;
	default:
		return;
	}
	c->LookAt( c->Eye() + to, c->At() + to, c->Up());
}
void cameraVFRotation(unsigned char key)
{
	Camera* c = scene->ActiveCam();
	if( NULL == c)	return;

	vec3 eye = c->Eye();
	vec3 at = c->At();
	vec3 fwd = normalize( at - eye);
	vec3 right = normalize(cross( fwd, c->Up()));
	vec3 up = normalize( cross( right, fwd));
	

	switch (key)
	{
	case 'w':
		at = eye + ( normalize( fwd + (up * smoothFactor)) * length(at - eye));
		up = normalize(cross( right, at - eye ));
		break;
	case 's':
		at = eye + ( normalize( fwd + (up * -smoothFactor)) * length(at - eye));
		up = normalize(cross( right, at - eye ));
		break;
	case 'a':
		at = eye + ( normalize( fwd + (right * -smoothFactor)) * length(at - eye));
		break;
	case 'd':
		at = eye + ( normalize( fwd + (right * smoothFactor)) * length(at - eye));
		break;
	case 'e':
		up = normalize( up + (right * smoothFactor));
		break;
	case 'q':
		up = normalize( up + (right * -smoothFactor));
		break;
	default:
		return;
	}
	c->LookAt( eye, at, up);
}
void cameraVFTranslation(unsigned char key)
{
	Camera* c = scene->ActiveCam();
	if( NULL == c)	return;

	vec3 forwardDirection = normalize( c->At() - c->Eye());
	vec3 rightDirection = normalize( cross( forwardDirection, c->Up()));
	vec3 upDirection = normalize( cross( rightDirection, forwardDirection));
	vec3 to;
	switch (key)
	{
	case 'w':
		to = forwardDirection * smoothFactor;
		break;
	case 's':
		to = forwardDirection * -smoothFactor;
		break;
	case 'a':
		to = rightDirection * -smoothFactor;
		break;
	case 'd':
		to = rightDirection * smoothFactor;
		break;
	case 'e':
		to = upDirection * smoothFactor;
		break;
	case 'q':
		to = upDirection * -smoothFactor;
		break;
	default:
		return;
	}
	c->LookAt( c->Eye() + to, c->At() + to, c->Up());
}
void cameraKeyboard(unsigned char key)
{
	switch ( key ) {
	case '[':
		cameraActiveFrame = C_WORLD_FRAME;
		return;
	case ']':
		cameraActiveFrame = C_VIEW_FRAME;
		return;
	case '\t':
		scene->ToggleActiveCamera();
		return;
	}
	if( C_WORLD_FRAME == cameraActiveFrame)
	{
		if(T_ROTATION == activeTransformation)
			cameraWFRotation(key);
		else if ( T_TRANSLATION == activeTransformation)
			cameraWFTranslation(key);
	}
	else if( C_VIEW_FRAME == cameraActiveFrame)
	{
		if(T_ROTATION == activeTransformation)
			cameraVFRotation(key);
		else if ( T_TRANSLATION == activeTransformation)
			cameraVFTranslation(key);
	}
}


void handleParallelLightRotation(unsigned char key)
{
	Light* al = scene->ActiveLight();
	Camera* c = scene->ActiveCam();
	if( NULL == al || NULL == c || PARALLEL_S != al->lightSource) return;

	vec3 forwardDirection = normalize( c->At() - c->Eye());
	vec3 rightDirection = normalize( cross( forwardDirection, c->Up()));
	vec3 upDirection = normalize( cross( rightDirection, forwardDirection));
	switch (key)
	{
	case 'w':
		al->direction = customRotationMatrix( rightDirection, -smoothFactor) * al->direction;
		break;
	case 's':
		al->direction = customRotationMatrix( rightDirection, smoothFactor) * al->direction;
		break;
	case 'a':
		al->direction = customRotationMatrix( upDirection, smoothFactor) * al->direction;
		break;
	case 'd':
		al->direction = customRotationMatrix( upDirection, -smoothFactor) * al->direction;
		break;
	case 'e':
		al->direction = customRotationMatrix( forwardDirection, smoothFactor) * al->direction;
		break;
	case 'q':
		al->direction = customRotationMatrix( forwardDirection, -smoothFactor) * al->direction;
		break;
	default:
		return;
	}
}
void handlePointLightTranslation(unsigned char key)
{
	Light* al = scene->ActiveLight();
	Camera* c = scene->ActiveCam();
	if( NULL == al || NULL == c || POINT_S != al->lightSource) return;

	vec3 forwardDirection = normalize( c->At() - c->Eye());
	vec3 rightDirection = normalize( cross( forwardDirection, c->Up()));
	vec3 upDirection = normalize( cross( rightDirection, forwardDirection));

	switch (key)
	{
	case 'w':
		al->location += vec4(forwardDirection * smoothFactor, 0);
		break;
	case 's':
		al->location += vec4(forwardDirection * -smoothFactor, 0);
		break;
	case 'a':
		al->location += vec4(rightDirection * -smoothFactor, 0);
		break;
	case 'd':
		al->location += vec4(rightDirection * smoothFactor, 0);
		break;
	case 'e':
		al->location += vec4(upDirection * smoothFactor, 0);
		break;
	case 'q':
		al->location += vec4(upDirection * -smoothFactor, 0);
		break;
	default:
		return;
	}
}
void lightKeyboard(unsigned char key)
{
	switch ( key ) {
	case '\t':
		scene->ToggleActiveLight();
		return;
	}
	Light* al = scene->ActiveLight();
	if( NULL == al) return;
	if( AMBIENT_L == al->lightType ) return;
	if( PARALLEL_S == al->lightSource)
		handleParallelLightRotation(key);
	else if( POINT_S == al->lightSource)
		handlePointLightTranslation(key);
}

void keyboard( unsigned char key, int x, int y )
{
	if( 033 == key)
		exit( EXIT_SUCCESS );
	if( tryGeneralKeyboardCommands( key) )
		return;

	switch ( activeMode ) {
	case MODEL_MODE:
		modelKeyboard(key);
		break;
	case CAMERA_MODE:
		cameraKeyboard(key);
		break;
	case LIGHT_MODE:
		lightKeyboard(key);
		break;
	default:
		throw exception("Active mode is not leagal!!!");
		break;
	}
	glutPostRedisplay();
	return;

	Camera* ac = scene->ActiveCam();
	switch ( key ) {
	case 033:
		exit( EXIT_SUCCESS );
		break;
	case 'm':
		scene->ToggleActiveModel();
		break;
	case 'c':
		scene->ToggleActiveCamera();
		//smoothFactor = (scene->ActiveCam()->Right() - scene->ActiveCam()->Left()) / 10;
		break;
	case 'w': //nearer 
		{
			if (ac == NULL) break;
			vec3 diff = ac->At() - ac->Eye();
			if (length(diff) < 0.05) break;
			vec3 newEye = ac->Eye() + 0.05 * diff;
			vec3 newAt = ac->At() + 0.05 * diff;
			ac->LookAt(newEye, newAt, ac->Up());
		}
		break;
	case 's': //farther
		{
			if (ac == NULL) break;
			vec3 diff = ac->At() - ac->Eye();
			vec3 newEye = ac->Eye() - 0.05 * diff;
			vec3 newAt = ac->At() - 0.05 * diff;
			ac->LookAt(newEye, newAt, ac->Up());
		}
		break;
	case 'a': // cursor left
		{
			if (ac == NULL) break;
			vec3 diff = ac->At() - ac->Eye();
			vec3 viewRight = cross(diff, ac->Up());
			vec3 delta = - 0.05 * viewRight;
			ac->LookAt(ac->Eye() + delta, ac->At() + delta, ac->Up());
		}
		break;
	case 'd': // cursor right
		{
			if (ac == NULL) break;
			vec3 diff = ac->At() - ac->Eye();
			vec3 viewRight = cross(diff, ac->Up());
			vec3 delta = 0.05 * viewRight;
			ac->LookAt(ac->Eye() + delta, ac->At() + delta, ac->Up());
		}
		break;
	case 'y':
		break;
	}
	
}

void mouse(int button, int state, int x, int y)
{
	//button = {GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON}
	//state = {GLUT_DOWN,GLUT_UP}
	const int wheel_up = 3;
	const int wheel_down = 4;

	ctr_down = (glutGetModifiers() & GLUT_ACTIVE_CTRL) ? true : false;
	shift_down = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) ? true : false;
	alt_down = (glutGetModifiers() & GLUT_ACTIVE_ALT) ? true : false;

	float zoom_factor = 1.1;
	//set down flags
	switch(button) {
	case GLUT_LEFT_BUTTON:
		lb_down = (state==GLUT_UP)?0:1;
		break;
	case GLUT_RIGHT_BUTTON:
		rb_down = (state==GLUT_UP)?0:1;
		break;
	case GLUT_MIDDLE_BUTTON:
		mb_down = (state==GLUT_UP)?0:1;	
		break;
	case wheel_up:
		zoom_factor = 0.9;
	case wheel_down:
		if (state==GLUT_DOWN) 
		{
			if (alt_down) {
				MeshModel* am = scene->ActiveModel();
				if (NULL == am)
					break;
				am->WFScale(Scale(1/zoom_factor, 1/zoom_factor, 1/zoom_factor));
				glutPostRedisplay();
				break;
			}
			Camera* ac = scene->ActiveCam();
			if (NULL == ac)
				return;
			ac->Zoom(zoom_factor);
			glutPostRedisplay();
		}
		break;
	}

	if (lb_down || mb_down || rb_down) {
		last_x=x;
		last_y=y;
	}
}

void motion(int x, int y)
{
	// calc difference in mouse movement
	int dx = x - last_x;
	int dy = y - last_y;
	// update last x,y
	last_x = x;
	last_y = y;
	Camera* ac = scene->ActiveCam();
	MeshModel* am = scene->ActiveModel();

	if (NULL == ac || (dx ==0 && dy == 0) || (am == NULL && alt_down))
		return;
	vec3 eye = ac->Eye();
	vec3 up = ac->Up();
	vec3 at = ac->At();

	vec3 visionAxis = (at - eye);
	vec3 axis1 = normalize( cross( visionAxis, up ) );
	vec3 axis2 = normalize( cross( axis1, visionAxis ) );
	if (ctr_down && lb_down) {
		float len = length(visionAxis);
		eye += (-(dx * axis1) * smoothFactor);
		eye += ((dy * axis2) * smoothFactor);
		eye = at + len * normalize( eye - at );
		up = normalize( cross( cross( (at - eye), up ), (at - eye) ) );
		ac->LookAt(eye, at, up);
		glutPostRedisplay();
	}
	if (ctr_down && mb_down) {
		vec3 vdxx = -(dx * axis1 * smoothFactor); 
		vec3 vdyy = (dy * axis2 * smoothFactor); 
		eye +=  vdxx;
		eye += vdyy;
		at += vdxx;
		at += vdyy;
		ac->LookAt(eye, at, up);
		glutPostRedisplay();
	}
	if (alt_down && lb_down) {
		vec3 dv =  ( -dx * axis1 * smoothFactor) + (dy * axis2 * smoothFactor);
		vec3 modelOrigin = scene->ActiveModel()->Origin();

		am->WFTranslate(Translate(-modelOrigin));
		am->WFRotate(customRotationMatrix( normalize(cross(visionAxis, dv)) , smoothFactor));
		am->WFTranslate(Translate(modelOrigin));
		glutPostRedisplay();
	}
	if (alt_down && mb_down) {
		vec3 dv =  ( -dx * axis1 * smoothFactor) + (dy * axis2 * smoothFactor);
		mat4 tr = Translate( -dv.x * smoothFactor, -dv.y * smoothFactor, -dv.z * smoothFactor );
		am->WFTranslate(tr);
		glutPostRedisplay();
	}
	
}

void addModel()
{
	CFileDialog dlg(TRUE,_T(".obj"),NULL,NULL,_T("*.obj|*.*"));
	if(dlg.DoModal()==IDOK)
	{
		std::string s((LPCTSTR)dlg.GetPathName());
		scene->LoadOBJModel((LPCTSTR)dlg.GetPathName());
	}
}
void addCamera()
{
	CXyzDialog cameraEye("Please specify camera location in world cordinates");
	CXyzDialog cameraAt("Please specify camera target point in world cordinates");
	CXyzDialog cameraUp("Please specify camera UP vector in world cordinates");
	if (cameraEye.DoModal() == IDOK && cameraAt.DoModal() == IDOK && cameraUp.DoModal() == IDOK) {
		Camera c;
		// TODO: what if 'eye' and 'at' at the same place? what if 'up' bad ? 
		c.LookAt(cameraEye.GetXYZ(), cameraAt.GetXYZ(), cameraUp.GetXYZ());
		scene->AddCamera(c);
	}
}
void setBackground()
{
	ColorSelector dlg;
	dlg.SetColor(Rgb(0,0,0));
	if (IDOK == dlg.DoModal())
	{
	}
}

void mainMenu(int id)
{
	switch (id)
	{
	case MAIN_REMOVE_GEOMETRY:
		scene->RemoveGeometry();
		break;
	case MAIN_ADD_MODEL:
		addModel();
		break;
	case MAIN_ADD_CAMERA:
		addCamera();
		break;
	case MAIN_RENDER_CAMERAS:
		scene->ToggleShowCameras();
		break;
	case MAIN_SHOW_WORLD_FRAME:
		scene->ToggleShowWorldFrame();
		break;
	case MAIN_ADD_PRIMITIVE:
		scene->AddMeshModel( PrimMeshModel());
		break;
	case MAIN_RENDER_LIGHTS:
		scene->ToggleShowLights();
		break;
	case MAIN_REMOVE_CAMERAS:
		scene->RemoveCameras();
		break;
	case MAIN_REMOVE_LIGHTS:
		scene->RemoveLights();
		break;
	case MAIN_SET_BACKGROUND_COLOR:
		setBackground();
		break;
	case MAIN_RENDER_SILHOUETTE:
		scene->ToggleShowSilhouette();
		break;
	}
	glutPostRedisplay();

}

void modelNonUniformScale()
{
	CXyzDialog dialog;
	if(IDOK ==  dialog.DoModal())
	{
		vec3 scl = dialog.GetXYZ();
		scene->ActiveModel()->WFScale(Scale(scl));
	}
}
void setMonotonColor()
{
	MeshModel* m = scene->ActiveModel();
	if( NULL == m) return;

	MaterialColor cp = m->GetDefaultColor();
	MColorDialog d(cp.emissive, cp.diffuse, cp.specular, cp.ambient);

	if(IDOK == d.DoModal())
	{
		cp.diffuse = d.m_clr_diffuse;
		cp.emissive = d.m_clr_emissive;
		cp.specular = d.m_clr_specular;
		cp.ambient = d.m_clr_ambient;
		m->SetDefaultColor(cp);
		m->SetDrawTexture(false);
	}
}
void loadTexture()
{
	MeshModel* am = scene->ActiveModel();
	if(NULL == am)
		return;
	CFileDialog dlg(TRUE,_T(".png"),NULL,NULL,_T("*.png|*.*"));
	if(dlg.DoModal()==IDOK)
	{
		string s((LPCTSTR)dlg.GetPathName());
		Texture t;
		lodepng::decode(t.img, t.width, t.height, s);
		renderer->BindTexture(am, t);
		am->SetDrawTexture(true);
	}
}

void loadNormalTexture()
{
	MeshModel* am = scene->ActiveModel();
	if(NULL == am)
		return;
	CFileDialog dlg(TRUE,_T(".png"),NULL,NULL,_T("*.png|*.*"));
	if(dlg.DoModal()==IDOK)
	{
		string s((LPCTSTR)dlg.GetPathName());
		Texture t;
		lodepng::decode(t.img, t.width, t.height, s);
		renderer->BindNormalTexture(am, t);
		am->SetNormalMap(true);
	}
}

void menuActiveModel(int id)
{
	if (NULL == scene->ActiveModel())
		return;
	MeshModel* m = scene->ActiveModel();
	switch (id)
	{
	case MODEL_SHOW_VERTEX_NORMALS:
		m->ToggleShowVertexNormals();
		break;
	case MODEL_SHOW_FACE_NORMALS: 
		m->ToggleShowFaceNormals();
		break;
	case MODEL_SHOW_BOUNDING_BOX:
		m->ToggleShowBoundingBox();
		break;
	case MODEL_SHOW_FRAME:
		m->ToggleShowModelFrame();
		break;
	case MODEL_NON_UNIFORM_SCALE:
		modelNonUniformScale();
		break;
	case MODEL_SET_MONOTON_COLOR:
		setMonotonColor();
		break;
	case MODEL_SET_TEXTURE:
		loadTexture();
		break;
	case MODEL_ENABLE_ENV_MAP:
		m->SetDrawEnvMap(true);
		break;
	case MODEL_DISABLE_ENV_MAP:
		m->SetDrawEnvMap(false);
		break;
	case MODEL_ENABLE_SPHERICAL_TEX_COORD:
		m->SetTextureCoordinatesSource(SPHERICAL);
		break;
	case MODEL_ENABLE_NORMAL_MAP:
		loadNormalTexture();
		break;
	case MODEL_DISABLE_NORMAL_MAP:
		m->SetNormalMap(false);
		break;
	case MODEL_ENABLE_VERTEX_ANIM:
		m->SetVertexAnimation(true);
		break;
	case MODEL_DISABLE_VERTEX_ANIM:
		m->SetVertexAnimation(false);
		break;
	case MODEL_ENABLE_COLOR_ANIM:
		m->SetColorAnimation(true);
		break;
	case MODEL_DISABLE_COLOR_ANIM:
		m->SetColorAnimation(false);
		break;
	}
	glutPostRedisplay();
}

void setOrthoLens()
{
	CCmdDialog cid;
	if(IDOK ==  cid.DoModal())
	{
		string cmd = cid.GetCmd();
		parseOrthoCmd(cmd);
	}
}
void setFrustumLens()
{
	CCmdDialog cid;
	if(IDOK ==  cid.DoModal())
	{
		string cmd = cid.GetCmd();
		parseFrustumCmd(cmd);
	}
}
void setPerspectiveLens()
{
	CCmdDialog cid;
	if(IDOK ==  cid.DoModal())
	{
		string cmd = cid.GetCmd();
		parsePerspectiveCmd(cmd);
	}
}

void menuLens(int id)
{
	Camera* cam = scene->ActiveCam();
	if (NULL == cam) return;
	
	switch (id)
	{
	case LENS_ORTHO:
		setOrthoLens();
		break;
	case LENS_FRUSTUM:
		setFrustumLens();
		break;
	case LENS_PERSPECTIVE:
		setPerspectiveLens();
		break;
	}
	glutPostRedisplay();
}

void setCamLocation(Camera* c)
{

	CXyzDialog cameraEye("Please specify camera location in world cordinates");
	if (cameraEye.DoModal() == IDOK) 
	{
		c->LookAt(cameraEye.GetXYZ(), c->At(), c->Up());
	}
}
void setCamFocusPoint(Camera* c)
{
	CXyzDialog cameraAt("Please specify a location in world cordinates");
	if (cameraAt.DoModal() == IDOK) 
	{
		c->LookAt(c->Eye(), cameraAt.GetXYZ(), c->Up());
	}
}

void menuActiveCamera(int id)
{
	if (NULL == scene->ActiveCam())
		return;
	Camera* c = scene->ActiveCam();
	switch (id)
	{
	case CAMERA_SET_LOCATION:
		setCamLocation(c);
		break;
	case CAMERA_SET_FOCUS_POINT:
		setCamFocusPoint(c);
		break;
	case CAMERA_FOCUS_ON_ACTIVE_MODEL:
		if (NULL == scene->ActiveModel())
			break;
		c->LookAt(c->Eye(), scene->ActiveModel()->Origin(), c->Up());
		break;
	case CAMERA_SET_TYPE:
		break;
	}
	glutPostRedisplay();
}

void addAmbientLight()
{
	ColorSelector dlg;
	dlg.SetColor(Rgb(1,1,1));
	if (IDOK == dlg.DoModal())
	{
		scene->AddLight( Light(AMBIENT_L, PARALLEL_S, vec4(0,0,0,0), dlg.GetColor(), vec4(0,0,0,0)));
	}
}
void addParallelLight()
{
	CXyzDialog direction("Please specify a direction");
	if (direction.DoModal() == IDOK) 
	{
		ColorSelector dlg;
		dlg.SetColor(Rgb(1,1,1));
		if (IDOK == dlg.DoModal())
		{
			scene->AddLight( Light(REGULAR_L, PARALLEL_S, vec4(0,0,0,0), dlg.GetColor(), vec4(direction.GetXYZ(),0) ));
		}
	}
}
void addPointLight()
{
	CXyzDialog position("Please specify a position");
	if (position.DoModal() == IDOK) 
	{
		ColorSelector dlg;
		dlg.SetColor(Rgb(1,1,1));
		if (IDOK == dlg.DoModal())
		{
			scene->AddLight( Light(REGULAR_L, POINT_S, vec4(position.GetXYZ(),1), dlg.GetColor(), vec4(0,0,0,0) ));
		}
	}
}

void menuLight(int id)
{
	if (NULL == scene->ActiveCam())
		return;
	
	switch (id)
	{
	case LIGHT_AMBIENT:
		addAmbientLight();
		break;
	case LIGHT_PARALLEL_SOURCE:
		addParallelLight();
		break;
	case LIGHT_POINT_SOURCE:
		addPointLight();
		break;
	}
	glutPostRedisplay();
}

void menuRenderer(int id)
{
	switch (id)
	{
	case RENDERER_SHADING_WIREFRAME:
		break;

	case RENDERER_SHADING_FLAT:
		scene->SetShading(FLAT);
		break;

	case RENDERER_SHADING_GOURAUD:
		scene->SetShading(GOURAUD);
		break;

	case RENDERER_SHADING_PHONG:
		scene->SetShading(PHONG);
		break;

	case RENDERER_SHADING_TOON:
		scene->SetShading(TOON);
		break;

	case RENDERER_SET_ANTIALIASING:
		renderer->ToggleAntialiasing();
		break;

	case RENDERER_TOGGLE_FOG:
		break;

	case RENDERER_SET_FOG_COLOR:
		{
			ColorSelector dlg;
			dlg.SetColor(Rgb(0,0,0));
			if (IDOK == dlg.DoModal())
			{
			}
		}
		break;
	}
	glutPostRedisplay();
}

void initMenu()
{
	int activeModelMenuId = glutCreateMenu(menuActiveModel);
	//glutAddMenuEntry("Show Bounding Box",			MODEL_SHOW_BOUNDING_BOX);
	glutAddMenuEntry("Set Monotone Color",			MODEL_SET_MONOTON_COLOR);
	glutAddMenuEntry("Set Texture",					MODEL_SET_TEXTURE);
	//glutAddMenuEntry("Show Model Frame",			MODEL_SHOW_FRAME);
	//glutAddMenuEntry("Show Normals per Vertex",		MODEL_SHOW_VERTEX_NORMALS);
	//glutAddMenuEntry("Show Normals per Face",		MODEL_SHOW_FACE_NORMALS);
	glutAddMenuEntry("Nonuniform Scale",			MODEL_NON_UNIFORM_SCALE);
	glutAddMenuEntry("Enable environment map",		MODEL_ENABLE_ENV_MAP);
	glutAddMenuEntry("Disable environment map",		MODEL_DISABLE_ENV_MAP);
	glutAddMenuEntry("Spherical texture coords",	MODEL_ENABLE_SPHERICAL_TEX_COORD);
	glutAddMenuEntry("Set normal mapping texture",	MODEL_ENABLE_NORMAL_MAP);
	glutAddMenuEntry("Disable normal mapping",		MODEL_DISABLE_NORMAL_MAP);
	glutAddMenuEntry("On vertex animation",			MODEL_ENABLE_VERTEX_ANIM);
	glutAddMenuEntry("Off vertex animation",		MODEL_DISABLE_VERTEX_ANIM);
	glutAddMenuEntry("On color animation",			MODEL_ENABLE_COLOR_ANIM);
	glutAddMenuEntry("Off color animation",			MODEL_DISABLE_COLOR_ANIM);

	int lensMenu = glutCreateMenu(menuLens);
	glutAddMenuEntry("Ortho",			LENS_ORTHO);
	glutAddMenuEntry("Perspective",		LENS_PERSPECTIVE);
	glutAddMenuEntry("Frustum",			LENS_FRUSTUM);

	int activeCameraMenuId = glutCreateMenu(menuActiveCamera);
	glutAddMenuEntry("Focus on Active Model",		CAMERA_FOCUS_ON_ACTIVE_MODEL);
	glutAddMenuEntry("Set Camera Location",			CAMERA_SET_LOCATION);
	glutAddMenuEntry("Look At",						CAMERA_SET_FOCUS_POINT);	
	glutAddSubMenu("Set lens type", lensMenu);

	int rendererMenuId = glutCreateMenu(menuRenderer);
	//glutAddMenuEntry("Wireframe",		RENDERER_SHADING_WIREFRAME);
	glutAddMenuEntry("Flat",			RENDERER_SHADING_FLAT);
	glutAddMenuEntry("Gouraud",			RENDERER_SHADING_GOURAUD);
	glutAddMenuEntry("Phong",			RENDERER_SHADING_PHONG);
	glutAddMenuEntry("Toon",			RENDERER_SHADING_TOON);
	glutAddMenuEntry("Antialiasing",	RENDERER_SET_ANTIALIASING);
	//glutAddMenuEntry("Toggle Fog",		RENDERER_TOGGLE_FOG);
	//glutAddMenuEntry("Set Fog Color",	RENDERER_SET_FOG_COLOR);
	
	int lightMenuId = glutCreateMenu(menuLight);
	//glutAddMenuEntry("Ambient",				LIGHT_AMBIENT);
	glutAddMenuEntry("Point Source",		LIGHT_POINT_SOURCE);
	glutAddMenuEntry("Parallel Source",		LIGHT_PARALLEL_SOURCE);

	glutCreateMenu(mainMenu);
	glutAddMenuEntry("Add Model",			MAIN_ADD_MODEL);
	glutAddSubMenu("Active Model",			activeModelMenuId);
	glutAddMenuEntry("Add Camera",			MAIN_ADD_CAMERA);
	glutAddSubMenu("Active Camera",			activeCameraMenuId);
	glutAddSubMenu("Add Light",				lightMenuId);
	glutAddSubMenu("Renderer",				rendererMenuId);

	//glutAddMenuEntry("Add Primitive",		MAIN_ADD_PRIMITIVE);
	glutAddMenuEntry("Remove all Models",	MAIN_REMOVE_GEOMETRY);
	glutAddMenuEntry("Remove all Cameras",	MAIN_REMOVE_CAMERAS);
	glutAddMenuEntry("Remove all Lights",	MAIN_REMOVE_LIGHTS);

	glutAddMenuEntry("Show silhouette",		MAIN_RENDER_SILHOUETTE);
	//glutAddMenuEntry("Show world frame",	MAIN_SHOW_WORLD_FRAME);
	//glutAddMenuEntry("Show Cameras",		MAIN_RENDER_CAMERAS);
	//glutAddMenuEntry("Show Lights",			MAIN_RENDER_LIGHTS);
	//glutAddMenuEntry("Set Background Color",MAIN_SET_BACKGROUND_COLOR);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
//----------------------------------------------------------------------------

void onDestruction()
{
	delete scene;
	delete renderer;
}

void onTimer(int val) {
	bool OK = true;
	scene->Animation();
	glutPostRedisplay();
	glutTimerFunc(timerInterval, &onTimer, 0);
}

int my_main( int argc, char **argv )
{
	//----------------------------------------------------------------------------
	// Initialize window
	//----------------------------------------------------------------------------
	glutInit( &argc, argv );
	glutInitDisplayMode(	GLUT_DEPTH |
							GLUT_DOUBLE | 
							GLUT_RGBA |
							GLUT_MULTISAMPLE );

	glutInitWindowSize( 800, 800 );
	glutInitContextVersion( 3, 3 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutCreateWindow( "The Awesome" );
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		throw std::exception("Failed in glewInit");
		exit(1);
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	
	glEnable( GL_TEXTURE_2D);



	//----------------------------------------------------------------------------
	// Initialize Callbacks
	//----------------------------------------------------------------------------
	glutDisplayFunc( display );
	glutKeyboardFunc( keyboard );
	glutMouseFunc( mouse );
	glutMotionFunc ( motion );
	glutReshapeFunc( reshape );
	glutTimerFunc(timerInterval, &onTimer, 0);
	glutCloseFunc( onDestruction ); 	

	initMenu();

	//----------------------------------------------------------------------------
	// Create initial renderer
	//----------------------------------------------------------------------------
	renderer = new Renderer(800, 800);
	
	//----------------------------------------------------------------------------
	// Create initial cameras
	//----------------------------------------------------------------------------
	Camera c1 = Camera(); // The default camera
	c1.LookAt(vec3(0,15,0) , vec3(0,0,0) , vec3(0,0,1) );
	c1.Perspective(60, 1, 1, 200);
	Camera c2 = Camera(); // One more
	c2.LookAt(vec3(5,5,5) , vec3(0,0,0) , vec3(0,0,1) );
	c2.Perspective(30, 4/3, 0.5, 20);
	smoothFactor = (c1.Right() - c1.Left()) / 10.0;

	//----------------------------------------------------------------------------
	// Create initial scene
	//----------------------------------------------------------------------------
	scene = new Scene(renderer);
	scene->AddCamera(c1);
	scene->AddLight(Light(REGULAR_L, PARALLEL_S, vec4(0,0,0,0), Rgb(0.5,0.5,0.5), vec4(0,0,-1, 0)));
	scene->AddLight(Light(REGULAR_L, POINT_S, vec4(0,5,-5,1), Rgb(0.5,0.5,0.5), vec4(0,0,0, 0)));



	glutMainLoop();
	delete renderer;
	delete scene;
	return 0;
}

int main( int argc, char **argv )
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		my_main(argc, argv );
	}

	return nRetCode;
}
