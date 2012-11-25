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

	m_renderer->SetCamera(cameras[activeCamera]);

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
}