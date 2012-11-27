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
	
	
	drawWorldAxes();

	for (int i = 0; i < models.size(); i++) {
		Model* model = models[i];
		if (model == ActiveModel())
		{
			drawModelAxes(model);
			model->draw(m_renderer, Rgb(1,0,0));
		}
		else {
			model->draw(m_renderer);
		}
	}
	
	m_renderer->SwapBuffers();
}

void Scene::drawWorldAxes()
{
	m_renderer->DrawLine3D(vec4(0,0,0,1), vec4(0.5,0,0,0), Rgb(1,0,0));
	m_renderer->DrawLine3D(vec4(0,0,0,1), vec4(0,0.5,0,0), Rgb(0,1,0));
	m_renderer->DrawLine3D(vec4(0,0,0,1), vec4(0,0,0.5,0), Rgb(0.2,0.2, 1));
}

void Scene::drawModelAxes(Model* m)
{
	vector<vec4> modelAxes = m->coordinates();
	vec4 modelOrigin = m->origin();
	m_renderer->DrawLine3D(modelOrigin, 0.2*normalize(modelAxes[0]), Rgb(0.5,0.5,0));
	m_renderer->DrawLine3D(modelOrigin, 0.2*normalize(modelAxes[1]), Rgb(0.5,0.5,0));
	m_renderer->DrawLine3D(modelOrigin, 0.2*normalize(modelAxes[2]), Rgb(0.5,0.5,0));
}

void Scene::AddCamera(Camera * c)
{
	cameras.push_back(c);
	activeCamera = cameras.size() - 1;
	m_renderer->SetCamera(cameras[activeCamera]);
}

void Scene::SetView(float leftView, float rightView, float zNear, float zFar, float top, float bottom, vec3 eye, vec3 up, vec3 at)
{
	cameras[activeCamera]->LookAt(eye, at, up);
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

void Scene::SetActiveModelAnchor()
{
	anchored = ActiveModel()->coordinates();
}

vector<vec4> Scene::getAnchoredModelCoordinates() { 
	return anchored;
}

void Scene::Clean()
{
	//activeCamera = -1;
	activeLight = -1;
	activeModel = -1;
	models.clear();
	//cameras.clear();
}

bool Scene::isLegal() {
	return (activeModel != -1 && activeCamera != -1 && models.size() > activeModel && cameras.size() > activeCamera);
}

Camera* Scene::ActiveCam()
{
	if(activeCamera == -1 || cameras.size() <= activeCamera)
		return NULL;
	return cameras[activeCamera];
}

Model* Scene::ActiveModel()
{
	if(activeModel == -1  || models.size() <= activeModel)
		return NULL;
	return models[activeModel];
}

void Scene::ToggleActiveModel()
{
	if (activeModel == -1)
		return;
	activeModel += 1;
	activeModel %= models.size();
}