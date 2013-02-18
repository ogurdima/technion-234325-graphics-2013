#pragma once

#include <vector>
#include <string>
#include "Renderer.h"
#include "Camera.h"
#include "Model.h"
#include "MeshModel.h"
#include "Rgb.h"
#include "Light.h"


using namespace std;
 


class Scene 
{

protected:
	vector<MeshModel*>		models;
	vector<Light*>			lights;
	vector<Camera*>			cameras;
	Renderer*				renderer;

	int						activeModel;
	int						activeLight;
	int						activeCamera;

	bool					drawCameras;
	bool					drawWorldFrame;
	bool					drawLights;

	//ShadingType				shading;
	//GLuint					oglPrograms[3];
	//GLuint					oglLineProgram;
	//CameraBind				oglCameraBind;
	//
	//void			InitShaders();
	//void			SetCameraMatices();
	void			SetLights();
	//GLuint			BindLineBuffer(vector<vec4> verteces, vector<vec3> colors);


public:
	
	Scene(Renderer *renderer);
	~Scene();

	void LoadOBJModel(string fileName);
	void RemoveGeometry();
	void RemoveCameras();
	void RemoveLights();
	void AddCamera(Camera c);
	void AddMeshModel(MeshModel m );
	void AddLight(Light l);

	Camera* ActiveCam();
	Light* ActiveLight();
	MeshModel* ActiveModel();

	bool IsLegal();

	void SetShading(ShadingType s);

	void Draw();

	void ToggleActiveModel();
	void ToggleActiveCamera();
	void ToggleActiveLight();
	bool ToggleShowCameras();
	bool ToggleShowWorldFrame();
	bool ToggleShowLights();

	void DrawWorldAxes();
	//void DrawModelAxes(Model* m);
	//void DrawSnowflake(vec4 at, float len, Rgb col = Rgb(0,0,0));
	//void DrawLights();
	//void DrawCameras();

	//void SetActiveModelAnchor();
	//vector<vec3> getAnchoredModelCoordinates();
	//void RotateActiveModel(mat4 rotMatrix);
	//void TranslateActiveModel(mat4 transMatrix);
	//void AddActiveModelTransform(mat4 trans);
	
};