// CG_skel_w_MFC.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CG_skel_w_MFC.h"
#include <sstream>


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
#include "PrimMeshModel.h"
#include "MColorDialog.h"
#include "MaterialColor.h"

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

#define FILE_OPEN						1
#define MAIN_DEMO						1
#define MAIN_ABOUT						2
#define MAIN_TRY_DIALOG					3
#define MAIN_REMOVE_GEOMETRY			4
#define MAIN_ADD_CAMERA					5
#define MAIN_ADD_MODEL					6
#define MAIN_RENDER_CAMERAS				7
#define MAIN_SHOW_WORLD_FRAME			8
#define MAIN_ADD_PRIMITIVE				9
#define MAIN_RENDER_LIGHTS				10


#define MODEL_SHOW_VERTEX_NORMALS		20
#define MODEL_SHOW_FACE_NORMALS			21
#define MODEL_SHOW_BOUNDING_BOX			22
#define MODEL_SHOW_FRAME				23
#define MODEL_NON_UNIFORM_SCALE			24
#define MODEL_SET_DEFAULT_COLOR			25

#define CAMERA_SET_LOCATION				30
#define CAMERA_SET_FOV					31
#define CAMERA_SET_FOCUS_POINT			32
#define CAMERA_FOCUS_ON_ACTIVE_MODEL	33
#define CAMERA_SET_TYPE					34

#define LENS_ORTHO						341
#define LENS_PERSPECTIVE				342
#define LENS_FRUSTUM					343

#define MENU_SHADING_WIREFRAME			401
#define MENU_SHADING_FLAT				402
#define MENU_SHADING_GOURAUD			403
#define MENU_SHADING_PHONG				404


//----------------------------------------------------------------------------
// Global variables
//----------------------------------------------------------------------------
Scene *scene;
Renderer *renderer;
int last_x,last_y;
bool lb_down, rb_down, mb_down, ctr_down, shift_down, alt_down;
float smoothFactor;


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
	switch (ac->getLensMode())
	{
		case ORTHO:
			ac->Frustum(-x/2, x/2, -y/2, y/2, ac->ZNear(), ac->ZFar());
			break;
		case PERSPECTIVE:
		case FRUSTUM:
			ac->Frustum(-x/2, x/2, -y/2, y/2, ac->ZNear(), ac->ZFar());
			break;
	}
	
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
		break;
	case 'c':
		scene->ToggleActiveCamera();
		smoothFactor = (scene->ActiveCam()->Right() - scene->ActiveCam()->Left()) / 10;
		glutPostRedisplay();
		break;
	case ',':
		smoothFactor /= 2;
		break;
	case '.':
		smoothFactor *= 2;
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
				am->Scale(Scale(1/zoom_factor, 1/zoom_factor, 1/zoom_factor));
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

		am->Translate(Translate(-modelOrigin.x,-modelOrigin.y, -modelOrigin.z));
		am->Rotate(rotationMatrix);
		am->Translate(Translate(modelOrigin.x,modelOrigin.y, modelOrigin.z));
		glutPostRedisplay();
	}
	if (alt_down && mb_down) {
		vec3 dv =  ( -dx * axis1 * smoothFactor) + (dy * axis2 * smoothFactor);
		mat4 tr = Translate( -dv.x * smoothFactor, -dv.y * smoothFactor, -dv.z * smoothFactor );
		am->Translate(tr);
		glutPostRedisplay();
	}
	
}

void mainMenu(int id)
{
	CXyzDialog cameraEye("Please specify camera location in world cordinates");
	CXyzDialog cameraAt("Please specify camera target point in world cordinates");
	CXyzDialog cameraUp("Please specify camera UP vector in world cordinates");

	switch (id)
	{
	case MAIN_REMOVE_GEOMETRY:
		scene->RemoveGeometry();
		glutPostRedisplay();
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
			Camera c;
			c.LookAt(cameraEye.GetXYZ(), cameraAt.GetXYZ(), cameraUp.GetXYZ());
			scene->AddCamera(c);
			glutPostRedisplay();
		}
		break;
	case MAIN_RENDER_CAMERAS:
		scene->ToggleShowCameras();
		glutPostRedisplay();
		break;
	case MAIN_SHOW_WORLD_FRAME:
		scene->ToggleShowWorldFrame();
		glutPostRedisplay();
		break;
	case MAIN_ADD_PRIMITIVE:
		scene->AddMeshModel( PrimMeshModel());
		glutPostRedisplay();
		break;
	case MAIN_RENDER_LIGHTS:
		scene->ToggleShowLights();
		break;
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
		{
			CXyzDialog dialog;
			if(IDOK ==  dialog.DoModal())
			{
				vec3 scl = dialog.GetXYZ();
				scene->ActiveModel()->Scale(Scale(scl));
			}
		}
		break;
	case MODEL_SET_DEFAULT_COLOR:
		{
			MColorDialog d;
			MaterialColor cp = m->GetDefaultColor();
			d.m_clr_diffuse = cp.diffuse;
			d.m_clr_emissive = cp.emissive;
			d.m_clr_specular = cp.specular;
			d.m_clr_ambient = cp.ambient;

			if(IDOK == d.DoModal())
			{
				cp.diffuse = d.m_clr_diffuse;
				cp.emissive = d.m_clr_emissive;
				cp.specular = d.m_clr_specular;
				cp.ambient = d.m_clr_ambient;
				m->SetDefaultColor(cp);
			}
		}
	}
	glutPostRedisplay();
}

void menuLens(int id)
{
	Camera* cam = scene->ActiveCam();
	if (NULL == cam) return;
	
	switch (id)
	{
	case LENS_ORTHO:
		{
			CCmdDialog cid;
			if(IDOK ==  cid.DoModal())
			{
				string cmd = cid.GetCmd();
				parseOrthoCmd(cmd);
			}
		}
		break;
	case LENS_FRUSTUM:
		{
			CCmdDialog cid;
			if(IDOK ==  cid.DoModal())
			{
				string cmd = cid.GetCmd();
				parseFrustumCmd(cmd);
			}
		}
		break;
	case LENS_PERSPECTIVE:
		{
			CCmdDialog cid;
			if(IDOK ==  cid.DoModal())
			{
				string cmd = cid.GetCmd();
				parsePerspectiveCmd(cmd);
			}
		}
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

void menuRenderer(int id)
{
	switch (id)
	{
	case MENU_SHADING_WIREFRAME:
		renderer->SetShading(SHADING_WIREFRAME);
		break;

	case MENU_SHADING_FLAT:
		renderer->SetShading(SHADING_FLAT);
		break;

	case MENU_SHADING_GOURAUD:
		renderer->SetShading(SHADING_GOURARD);
		break;

	case MENU_SHADING_PHONG:
		renderer->SetShading(SHADING_PHONG);
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
	glutAddMenuEntry("Nonuniform Scale", MODEL_NON_UNIFORM_SCALE);
	glutAddMenuEntry("Set Model Color", MODEL_SET_DEFAULT_COLOR);
	

	int lensMenu = glutCreateMenu(menuLens);
	glutAddMenuEntry("Ortho", LENS_ORTHO);
	glutAddMenuEntry("Perspective", LENS_PERSPECTIVE);
	glutAddMenuEntry("Frustum", LENS_FRUSTUM);

	int activeCameraMenuId = glutCreateMenu(menuActiveCamera);
	glutAddMenuEntry("Set Camera Location", CAMERA_SET_LOCATION);
	glutAddMenuEntry("Look At", CAMERA_SET_FOCUS_POINT);
	glutAddMenuEntry("Focus on Active Model", CAMERA_FOCUS_ON_ACTIVE_MODEL);
	glutAddSubMenu("Set lens type", lensMenu);

	int rendererMenuId = glutCreateMenu(menuRenderer);
	glutAddMenuEntry("Wireframe", MENU_SHADING_WIREFRAME);
	glutAddMenuEntry("Flat", MENU_SHADING_FLAT);
	glutAddMenuEntry("Gouraud", MENU_SHADING_GOURAUD);
	glutAddMenuEntry("Phong", MENU_SHADING_PHONG);
	
	glutCreateMenu(mainMenu);
	glutAddMenuEntry("Show world frame",MAIN_SHOW_WORLD_FRAME);
	glutAddMenuEntry("Render Cameras",MAIN_RENDER_CAMERAS);
	glutAddMenuEntry("Render Lights",MAIN_RENDER_LIGHTS);
	glutAddMenuEntry("Remove all Models",MAIN_REMOVE_GEOMETRY);
	glutAddMenuEntry("Add Model",MAIN_ADD_MODEL);
	glutAddSubMenu("Active Model", activeModelMenuId);

	glutAddMenuEntry("Add Camera",MAIN_ADD_CAMERA);
	glutAddSubMenu("Active Camera", activeCameraMenuId);
	glutAddMenuEntry("Add primitive", MAIN_ADD_PRIMITIVE);
	glutAddSubMenu("Renderer", rendererMenuId);

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
	glutInitContextVersion( 2, 1 );
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


	//----------------------------------------------------------------------------
	// Create initial renderer
	//----------------------------------------------------------------------------
	renderer = new Renderer(800,800, Rgb(0.95, 0.95, 0.95));
	renderer->SetShading(SHADING_GOURARD);
	
	//----------------------------------------------------------------------------
	// Create initial cameras
	//----------------------------------------------------------------------------
	Camera c1 = Camera(); // The default camera
	c1.LookAt(vec3(0,5,0) , vec3(0,0,0) , vec3(0,0,1) );
	c1.Ortho(-4, 4, -3, 3, 0.5, 20);
	Camera c2 = Camera(); // One more
	c2.LookAt(vec3(5,5,5) , vec3(0,0,0) , vec3(0,0,1) );
	c2.Perspective(30, 4/3, 0.5, 20);

	smoothFactor = (c1.Right() - c1.Left()) / 10.0;

	//----------------------------------------------------------------------------
	// Create initial scene
	//----------------------------------------------------------------------------
	scene = new Scene(renderer);

	scene->AddCamera(c2);
	scene->AddCamera(c1);
	scene->AddLight(Light(REGULAR_L, POINT_S, vec4(7,7,7,0), Rgb(1,1,1)));

	scene->ToggleShowCameras();
	scene->ToggleShowLights();
	scene->ToggleShowWorldFrame();

	
	
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


