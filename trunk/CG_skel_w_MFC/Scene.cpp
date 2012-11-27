#include "stdafx.h"
#include "Scene.h"
#include "MeshModel.h"
#include <string>

using namespace std;

void Scene::loadOBJModel(string fileName)
{
	MeshModel *model = new MeshModel(fileName);
	models.push_back(model);
	activeModel = models.size() - 1;
}

void Scene::draw()
{
	// 1. Send the renderer the current camera transform and the projection
	// 2. Tell all models to draw themselves
	if(! isLegal())
		return;
	Model* model = models[activeModel];
	model->draw(m_renderer);
	m_renderer->SwapBuffers();
}


void Scene::AddCamera(Camera * c)
{
	cameras.push_back(c);
	activeCamera = cameras.size() - 1;
	m_renderer->SetCamera(cameras[activeCamera]);
}

void Scene::SetView(float leftView, float rightView, float zNear, float zFar, float top, float bottom, vec3 eye, vec3 up, vec3 at)
{
	cameras[activeCamera]->LookAt(vec4(eye, 0), vec4(at, 0), vec4(up, 0));
	cameras[activeCamera]->Frustum(leftView, rightView, bottom, top, zNear, zFar);
}

void Scene::RotateModel(mat4 rotMatrix)
{
	if (models.size() == 0)
		return;
	Model* model = models[activeModel];
	MeshModel* mmodel = dynamic_cast<MeshModel*>(model);
	if (mmodel == NULL)
		return;
	mmodel->addLeftWorldTransformation(rotMatrix);
}

vector<vec4> Scene::getModelCoordinates() { 
	Model* model = models[activeModel];
	MeshModel* mmodel = dynamic_cast<MeshModel*>(model);
	return mmodel->coordinates();
}

void Scene::Clean()
{
	activeCamera = -1;
	activeLight = -1;
	activeModel = -1;
	models.clear();
	cameras.clear();
}

bool Scene::isLegal() {
	return (activeModel != -1 && activeCamera != -1 && models.size() > activeModel && cameras.size() > activeCamera);
}