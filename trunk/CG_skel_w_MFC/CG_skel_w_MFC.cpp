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

#define FILE_OPEN 1
#define MAIN_DEMO 1
#define MAIN_ABOUT 2
#define MAIN_TRY_DIALOG 3
#define MAIN_CLEAN_SCENE 4
#define MAIN_ADD_CAMERA 5
#define MAIN_ADD_MODEL 6


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
	//Call the scene and ask it to draw itself
	scene->draw();
}

void reshape( int width, int height )
{
	//update the renderer's buffers
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
			Camera* ac = scene->ActiveCam();
			if (NULL == ac)
				return;
			ac->Zoom(zoom_factor);
			glutPostRedisplay();
		}
		break;

	}

	if (lb_down) {
		last_x=x;
		last_y=y;
		if (alt_down) {
			scene->SetActiveModelAnchor();
		}
		//glutPostRedisplay();
	}

	//scene->SetView(leftView, rightView, zNear, zFar, top, bottom, eye, up, at);
	
	// add your code
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
	if (NULL == ac)
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
	if (shift_down && lb_down) {
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
		vec4 rotationAxisObjectSpace = vec4(normalize(cross(visionAxis, dv)), 0);
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

		vector<vec4> modelCoords = scene->getAnchoredModelCoordinates();
		mat4 rotX = RotateX( dot(modelCoords[0], rotationAxisObjectSpace) ); 
		mat4 rotY = RotateY( dot(modelCoords[1], rotationAxisObjectSpace) ); 
		mat4 rotZ = RotateZ( dot(modelCoords[2], rotationAxisObjectSpace) ); 
		vec4 p1 = rotationAxisObjectSpace * 50;
		p1.w = 1;
		vec4 p2 = -rotationAxisObjectSpace * 50;
		p2.w = 1;
		//renderer->DrawLine3D( p1, p2 , Rgb(1,1,0));
		//scene->RotateModel(rotX*rotY*rotZ);
		scene->RotateModel(rotationMatrix);
		glutPostRedisplay();
	}
	//scene->SetView(leftView, rightView, zNear, zFar, top, bottom, eye, up, at);
	
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
			
		}
		break;
	/*case MAIN_TRY_DIALOG:
		if (IDOK == cdialog.DoModal()) {
			vec3 v = cdialog.GetXYZ();
			cout << "X="<< v.x << " Y=" << v.y << " Z=" << v.z << std::endl;
		}*/
		break;
	}
}

void initMenu()
{

	//int menuFile = glutCreateMenu(fileMenu);
	//glutAddMenuEntry("Open..",FILE_OPEN);
	glutCreateMenu(mainMenu);
	glutAddMenuEntry("Clear Screen",MAIN_CLEAN_SCENE);
	glutAddMenuEntry("Add Model",MAIN_ADD_MODEL);
	glutAddMenuEntry("Add Camera",MAIN_ADD_CAMERA);
	//glutAddSubMenu("File",menuFile);
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
	glutInitDisplayMode( GLUT_RGBA| GLUT_DOUBLE);
	glutInitWindowSize( 800, 800 );
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutCreateWindow( "CG" );
	glewExperimental = GL_TRUE;
	//glewInit();
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		/*		...*/
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));



	renderer = new Renderer(800,800);
	scene = new Scene(renderer);
	Camera* c = new Camera();
	float p = 3;
	
	vec3 eye = vec3(3,3,3);
	vec3 at = vec3(0,0,0);
	vec3 up = vec3(0,1,0);
	float leftView = -3;
	float rightView = 3;
	float top = 3;
	float bottom = -3;
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
	}
	//----------------------------------------------------------------------------
	// Initialize Callbacks

	glutDisplayFunc( display );
	glutKeyboardFunc( keyboard );
	glutMouseFunc( mouse );
	glutMotionFunc ( motion );
	glutReshapeFunc( reshape );
	initMenu();

	glutMainLoop();
	delete scene;
	delete renderer;
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


