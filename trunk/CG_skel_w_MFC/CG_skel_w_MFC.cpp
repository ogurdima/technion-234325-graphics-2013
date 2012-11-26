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

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

#define FILE_OPEN 1
#define MAIN_DEMO 1
#define MAIN_ABOUT 2

Scene *scene;
Renderer *renderer;

int last_x,last_y;
bool lb_down,rb_down,mb_down;
bool ctr_down;
bool shift_down;

float leftView, rightView, zNear, zFar, top, bottom;
vec3 eye, up, at;

//----------------------------------------------------------------------------
// Callbacks

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
	}
	//case :
		//ctr_down = 
	
}



void mouse(int button, int state, int x, int y)
{
	//button = {GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON}
	//state = {GLUT_DOWN,GLUT_UP}
	const int wheel_up = 3;
	const int wheel_down = 4;

	ctr_down = (glutGetModifiers() & GLUT_ACTIVE_CTRL) ? true : false;
	shift_down = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) ? true : false;

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
			leftView *= zoom_factor;
			rightView*=zoom_factor;
			top*= zoom_factor;
			bottom*=zoom_factor;

		}
		break;

	}

	if ((ctr_down || shift_down) && lb_down) {
		last_x=x;
		last_y=y;
	}

	scene->SetView(leftView, rightView, zNear, zFar, top, bottom, eye, up, at);
	// add your code
}

void motion(int x, int y)
{
	// calc difference in mouse movement
	int dx=x-last_x;
	int dy=last_y - y;
	// update last x,y
	last_x=x;
	last_y=y;
	if (ctr_down && lb_down) {
		float len = length(at - eye);
		//vec3 direction = 
		vec3 axis1 = normalize(cross((at - eye), up));
		vec3 axis2 = normalize(cross( axis1,(at - eye)));
		eye += (dx * axis1 / 50);
		eye += (dy * axis2 / 50);
		eye = at + len * normalize(eye - at);
		up =  normalize( cross( cross( (at - eye), top), (at - eye)));
	}
	if (shift_down && lb_down) {
		vec3 axis1 = normalize(cross((at - eye), up));
		vec3 axis2 = normalize(cross( axis1,(at - eye)));
		vec3 vdxx = (dx * axis1 / 50); 
		vec3 vdyy = (dy * axis2 / 50); 
		eye +=  vdxx;
		eye += vdyy;
		at += vdxx;
		at += vdyy;

	}
	scene->SetView(leftView, rightView, zNear, zFar, top, bottom, eye, up, at);
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
	switch (id)
	{
	case MAIN_DEMO:
		scene->drawDemo();
		break;
	case MAIN_ABOUT:
		AfxMessageBox(_T("Computer Graphics"));
		break;
	}
}

void initMenu()
{

	int menuFile = glutCreateMenu(fileMenu);
	glutAddMenuEntry("Open..",FILE_OPEN);
	glutCreateMenu(mainMenu);
	glutAddSubMenu("File",menuFile);
	glutAddMenuEntry("Demo",MAIN_DEMO);
	glutAddMenuEntry("About",MAIN_ABOUT);
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
	
	eye = vec3(0,0,7);
	at = vec3(0,0,0);
	up = vec3(0,1,0);
	leftView = -3;
	rightView = 3;
	top = 3;
	bottom = -3;
	zNear = 2;
	zFar = 10;

	//c->Ortho(-p,p,-p,p,-p,p);
	//c->LookAt(vec4(3,3,4,0), vec4(3,0,4,0),vec4(0,1,0,0));
	//c->Frustum(-3,3,-3,3,2,8);
	scene->AddCamera(c);
	scene->SetView(leftView, rightView, zNear, zFar, top, bottom, eye, up, at);
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


