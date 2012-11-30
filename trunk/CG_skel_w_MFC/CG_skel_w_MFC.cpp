// CG_skel_w_MFC.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CG_skel_w_MFC.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

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

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

#define FILE_OPEN						1
#define MAIN_DEMO						1
#define MAIN_ABOUT						2
#define MAIN_TRY_DIALOG					3
#define MAIN_CLEAN_SCENE				4
#define MAIN_ADD_CAMERA					5
#define MAIN_ADD_MODEL					6
#define MAIN_RENDER_CAMERAS				7

#define MODEL_SHOW_VERTEX_NORMALS		20
#define MODEL_SHOW_FACE_NORMALS			21
#define MODEL_SHOW_BOUNDING_BOX			22
#define MODEL_SHOW_FRAME				23

#define CAMERA_SET_LOCATION				30
#define CAMERA_SET_FOV					31
#define CAMERA_SET_FOCUS_POINT			32
#define CAMERA_FOCUS_ON_ACTIVE_MODEL	33
#define CAMERA_SET_TYPE					34

//----------------------------------------------------------------------------
// Global variables
//----------------------------------------------------------------------------
Scene *scene;
Renderer *renderer;
int last_x,last_y;
bool lb_down, rb_down, mb_down, ctr_down, shift_down, alt_down;


//----------------------------------------------------------------------------
// Callbacks
//----------------------------------------------------------------------------
void display( void )
{
	scene->draw();
}

void reshape( int width, int height )
{
	renderer->CreateBuffers(width, height);
	Camera* ac = scene->ActiveCam();
	float windowAR = (float) width / (float) height;
	float x = abs(ac->Left()) + abs(ac->Right());
	float y = x / windowAR;
	//scene->ActiveCam()->Frustum(-x/2, x/2, -y/2, y/2, ac->ZNear(), ac->ZFar());
	glutPostRedisplay();
}

void keyboard( unsigned char key, int x, int y )
{
	switch ( key ) {
	case 033:
		exit( EXIT_SUCCESS );
		break;
	case 'm':
		scene->ToggleActiveModel();
		glutPostRedisplay();
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
				am->addLeftWorldTransformation(Scale(1/zoom_factor, 1/zoom_factor, 1/zoom_factor));
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
	if (NULL == ac || (dx ==0 && dy == 0) || am == NULL)
		return;
	vec3 eye = ac->Eye();
	vec3 up = ac->Up();
	vec3 at = ac->At();

	vec3 visionAxis = (at - eye);
	vec3 axis1 = normalize( cross( visionAxis, up ) );
	vec3 axis2 = normalize( cross( axis1, visionAxis ) );
	float smoothFactor = (ac->Right() - ac->Left()) / 50.0;
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
		vec3 modelOrigin = scene->ActiveModel()->origin();
		vec4 rotationAxisObjectSpace =  vec4(normalize(cross(visionAxis, dv)), 0);
		float angle = 5 * M_PI / 180.0; //converting to radian value
		float u = rotationAxisObjectSpace.x;
		float v = rotationAxisObjectSpace.y;
		float w = rotationAxisObjectSpace.z;
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

		am->addLeftWorldTransformation(Translate(-modelOrigin.x,-modelOrigin.y, -modelOrigin.z));
		am->addLeftWorldTransformation(rotationMatrix);
		am->addLeftWorldTransformation(Translate(modelOrigin.x,modelOrigin.y, modelOrigin.z));
		glutPostRedisplay();
	}
	if (alt_down && mb_down) {
		vec3 dv =  ( -dx * axis1 * smoothFactor) + (dy * axis2 * smoothFactor);
		mat4 tr = Translate( -dv.x * smoothFactor, -dv.y * smoothFactor, -dv.z * smoothFactor );
		am->addLeftWorldTransformation(tr);
		glutPostRedisplay();
	}
	
}

void fileMenu(int id)
{
	switch (id)
	{
	case FILE_OPEN:
		CFileDialog dlg(TRUE,_T(".obj"),NULL,NULL,_T("*.obj|*.*"));
		if(dlg.DoModal()==IDOK)
		{
			std::string s((LPCTSTR)dlg.GetPathName());
			scene->loadOBJModel((LPCTSTR)dlg.GetPathName());
		}
		break;
	}
}

void mainMenu(int id)
{
	CXyzDialog cameraEye("Please specify camera location in world cordinates");
	CXyzDialog cameraAt("Please specify camera target point in world cordinates");
	CXyzDialog cameraUp("Please specify camera UP vector in world cordinates");

	switch (id)
	{
	case MAIN_CLEAN_SCENE:
		scene->Clean();
		break;
	case MAIN_ADD_MODEL:
		{
			CFileDialog dlg(TRUE,_T(".obj"),NULL,NULL,_T("*.obj|*.*"));
			if(dlg.DoModal()==IDOK)
			{
				std::string s((LPCTSTR)dlg.GetPathName());
				scene->loadOBJModel((LPCTSTR)dlg.GetPathName());
				glutPostRedisplay();
			}
		}
		break;
	case MAIN_ADD_CAMERA:
		if (cameraEye.DoModal() == IDOK && cameraAt.DoModal() == IDOK && cameraUp.DoModal() == IDOK) {
			Camera c = Camera(*(scene->ActiveCam()));
			c.LookAt(cameraEye.GetXYZ(), cameraAt.GetXYZ(), cameraUp.GetXYZ());
			scene->AddCamera(c);
			glutPostRedisplay();
		}
		break;
	case MAIN_RENDER_CAMERAS:
		scene->ToggleShowCameras();
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
	}
	glutPostRedisplay();
}

void menuActiveCamera(int id)
{
	if (NULL == scene->ActiveCam())
		return;
	Camera* c = scene->ActiveCam();
	switch (id)
	{
	case CAMERA_SET_LOCATION:
		{
			CXyzDialog cameraEye("Please specify camera location in world cordinates");
			if (cameraEye.DoModal() == IDOK) 
			{
				c->LookAt(cameraEye.GetXYZ(), c->At(), c->Up());
			}
			break;
		}
	case CAMERA_SET_FOV:
		break;
	case CAMERA_SET_FOCUS_POINT:
		{
			CXyzDialog cameraAt("Please specify a location in world cordinates");
			if (cameraAt.DoModal() == IDOK) 
			{
				c->LookAt(c->Eye(), cameraAt.GetXYZ(), c->Up());
			}
			break;
		}
	case CAMERA_FOCUS_ON_ACTIVE_MODEL:
		if (NULL == scene->ActiveModel())
			break;
		c->LookAt(c->Eye(), scene->ActiveModel()->origin(), c->Up());
		break;
	case CAMERA_SET_TYPE:
		break;
	}


	glutPostRedisplay();
}

void initMenu()
{
	int activeModelMenuId = glutCreateMenu(menuActiveModel);
	glutAddMenuEntry("Show Normals per Vertex", MODEL_SHOW_VERTEX_NORMALS);
	glutAddMenuEntry("Show Normals per Face", MODEL_SHOW_FACE_NORMALS);
	glutAddMenuEntry("Show Bounding Box", MODEL_SHOW_BOUNDING_BOX);
	glutAddMenuEntry("Show Model Frame", MODEL_SHOW_FRAME);

	int activeCameraMenuId = glutCreateMenu(menuActiveCamera);
	glutAddMenuEntry("Set Camera Location", CAMERA_SET_LOCATION);
	glutAddMenuEntry("Set Camera FOV", CAMERA_SET_FOV);
	glutAddMenuEntry("Look At", CAMERA_SET_FOCUS_POINT);
	glutAddMenuEntry("Focus on Active Model", CAMERA_FOCUS_ON_ACTIVE_MODEL);
	

	glutCreateMenu(mainMenu);
	glutAddMenuEntry("Render Cameras",MAIN_RENDER_CAMERAS);
	glutAddMenuEntry("Clear Screen",MAIN_CLEAN_SCENE);
	glutAddMenuEntry("Add Model",MAIN_ADD_MODEL);
	glutAddSubMenu("Active Model", activeModelMenuId);

	glutAddMenuEntry("Add Camera",MAIN_ADD_CAMERA);
	glutAddSubMenu("Active Camera", activeCameraMenuId);


	

	//glutAddMenuEntry("Demo",MAIN_DEMO);
	//glutAddMenuEntry("Try Dialog",MAIN_TRY_DIALOG);
	//glutAddMenuEntry("About",MAIN_ABOUT);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
//----------------------------------------------------------------------------



int my_main( int argc, char **argv )
{
	//----------------------------------------------------------------------------
	// Initialize window
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
	glutInitWindowSize( 800, 800 );
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutCreateWindow( "The Awesome" );
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(1);
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));



	renderer = new Renderer(800,800);
	scene = new Scene(renderer);
	Camera c = Camera();
	float p = 3;
	
	vec3 eye = vec3(3,3,3);
	vec3 at = vec3(0,0,0);
	vec3 up = vec3(0,1,0);
	float leftView = -4;
	float rightView = 4;
	float top = 4;
	float bottom = -4;
	float zNear = 2;
	float zFar = 7;

	//c->Ortho(-p,p,-p,p,-p,p);
	//c->LookAt(vec4(3,3,4,0), vec4(3,0,4,0),vec4(0,1,0,0));
	//c->Frustum(-3,3,-3,3,2,8);
	scene->AddCamera(c);
	Camera* ac = scene->ActiveCam();
	if (ac != NULL) {
		ac->LookAt(eye, at, up);
		ac->Frustum(leftView, rightView, bottom, top, zNear, zFar);
		ac->Ortho(leftView, rightView, bottom, top, zNear, zFar);

	}
	//----------------------------------------------------------------------------
	// Initialize Callbacks
	//----------------------------------------------------------------------------
	glutDisplayFunc( display );
	glutKeyboardFunc( keyboard );
	glutMouseFunc( mouse );
	glutMotionFunc ( motion );
	glutReshapeFunc( reshape );
	initMenu();

	glutMainLoop();
	delete renderer;
	delete scene;
	return 0;
}

CWinApp theApp;

using namespace std;

//void foo () 
//{
//	mat2 a1(1, 2 ,3 ,4);
//	mat2 a2(1, 0 ,0 ,1);
//	cout << a1;
//	cout << a2;
//	cout <<  a1 * a2  << endl;
//
//	mat3 a3(1,2,3,4,5,6,7,8,9);
//
//	cout << a3 << endl;
//	cout << transpose(a3) << endl;
//	system("pause");
//}

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


