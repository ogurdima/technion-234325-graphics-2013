#pragma once

#include "gl/glew.h"
#include <vector>
#include <string>
#include "Renderer.h"
#include "Camera.h"
using namespace std;

class Model {
protected:
	virtual ~Model() {}
public:
	void virtual draw(Renderer * r = NULL)=0;
};


class Light {

};
 

class Scene {
	vector<Model*> models;
	vector<Light*> lights;
	vector<Camera*> cameras;
	Renderer *m_renderer;

public:
	Scene() {};
	Scene(Renderer *renderer) : m_renderer(renderer) {};
	void loadOBJModel(string fileName);

	void AddCamera(Camera * c);
	void SetView(float leftView, float rightView, float zNear, float zFar, float top, float bottom, vec3 eye, vec3 up, vec3 at);

	void draw();
	void drawDemo();
	
	int activeModel;
	int activeLight;
	int activeCamera;
};