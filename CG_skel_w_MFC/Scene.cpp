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
	if(models.size() == 0 || cameras.size() == 0 )
		return;

	Model* model = models[activeModel];
	model->draw(m_renderer);
	m_renderer->SwapBuffers();
}

void Scene::drawDemo()
{
	m_renderer->SetDemoBuffer();
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

	if(models.size() == 0 || cameras.size() == 0 )
		return;
	draw();
	//m_renderer->DrawLine3D(eye, at);
	vec3 cameraEye = m_renderer->ObjectToCamera(eye);
	vec3 cameraAt = m_renderer->ObjectToCamera(at);
	m_renderer->DrawLine3D(cameraEye, cameraAt); 
	//m_renderer->DrawLine3D(vec4(0,0,0,1), vec4(3,0,0,1));
}