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
 

class Scene {
protected:
	vector<Model*> models;
	vector<Light*> lights;
	vector<Camera*> cameras;
	Renderer *m_renderer;
	vector<vec3> anchored;

	void AddActiveModelTransform(mat4 trans);

public:
	Scene() {};
	Scene(Renderer *renderer) : m_renderer(renderer) {};
	void loadOBJModel(string fileName);
	void Clean();
	void AddCamera(Camera * c);
	void RotateActiveModel(mat4 rotMatrix);
	void SetActiveModelAnchor();
	void SetView(float leftView, float rightView, float zNear, float zFar, float top, float bottom, vec3 eye, vec3 up, vec3 at);
	vector<vec3> getAnchoredModelCoordinates();
	void TranslateActiveModel(mat4 transMatrix);

	Camera* ActiveCam();
	Model* ActiveModel();

	void draw();
	void drawDemo();
	void drawWorldAxes();
	void drawModelAxes(Model* m);
	bool isLegal();
	void ToggleActiveModel();
	
	int activeModel;
	int activeLight;
	int activeCamera;
};