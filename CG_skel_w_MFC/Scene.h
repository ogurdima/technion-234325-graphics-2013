#pragma once

#include "gl/glew.h"
#include <vector>
#include <string>
#include "Renderer.h"
#include "Camera.h"
#include "Model.h"
#include "MeshModel.h"
using namespace std;

class Light {};
 

class Scene 
{

protected:
	vector<Model*> models;
	vector<Light*> lights;
	vector<Camera*> cameras;
	Renderer* m_renderer;
	vector<vec3> anchored;
	int activeModel;
	int activeLight;
	int activeCamera;
	bool drawCameras;
	bool drawWorldFrame;

	

public:
	Scene();
	~Scene();
	Scene(Renderer *renderer);
	void loadOBJModel(string fileName);
	void Clean();
	void AddCamera(Camera c);
	void AddMeshModel(MeshModel m );
	
	void SetActiveModelAnchor();
	vector<vec3> getAnchoredModelCoordinates();

	void RotateActiveModel(mat4 rotMatrix);
	void TranslateActiveModel(mat4 transMatrix);
	void AddActiveModelTransform(mat4 trans);

	Camera* ActiveCam();
	MeshModel* ActiveModel();

	void draw();
	void drawWorldAxes();
	void drawModelAxes(Model* m);
	bool isLegal();
	void ToggleActiveModel();
	void ToggleActiveCamera();
	bool ToggleShowCameras();
	bool ToggleShowWorldFrame();

	
};