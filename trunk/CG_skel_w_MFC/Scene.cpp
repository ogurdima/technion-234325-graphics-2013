#include "stdafx.h"
#include "Scene.h"
#include "MeshModel.h"
#include <string>

using namespace std;

Scene::Scene() :
activeModel(-1),
activeLight(-1),
activeCamera(-1),
m_renderer(NULL),
drawCameras(false),
drawWorldFrame(false)
{

}

Scene::Scene(Renderer *renderer) : 
m_renderer(renderer),
activeModel(-1),
activeLight(-1),
activeCamera(-1),
drawCameras(false),
drawWorldFrame(false)
{

}

Scene::~Scene() 
{
	for (int i = 0; i < models.size(); i++) {
		if (NULL != models[i]) {
			delete models[i];
			models[i] = NULL;
		}
	}

	for (int i = 0; i < cameras.size(); i++) {
		if (NULL != cameras[i]) {
			delete cameras[i];
			cameras[i] = NULL;
		}
	}

	for (int i = 0; i < lights.size(); i++) {
		if (NULL != lights[i]) {
			delete lights[i];
			lights[i] = NULL;
		}
	}
}

void Scene::loadOBJModel(string fileName)
{
	MeshModel *model = new MeshModel(fileName);
	models.push_back(model);
	activeModel = models.size() - 1;
}

void Scene::draw()
{
	if(! isLegal())
		return;
	
	if(drawWorldFrame)
	{
		drawWorldAxes();
	}

	m_renderer->DrawVisibleBoundary();

	for (int i = 0; i < models.size(); i++) {
		Model* model = models[i];
		if (model == ActiveModel())
		{
			model->draw(m_renderer, Rgb(0,0.75,0));
		}
		else 
		{
			model->draw(m_renderer);
		}
	}

	if (drawCameras) {
		for (int i = 0; i < cameras.size(); i++) {
			Camera* c = cameras[i];
			if (c == ActiveCam()) continue;
			float factor = 0.2;
			Vertex cntr = Vertex(c->Eye(), 1);
			Vertex north = cntr + vec4(factor,0,0,0);
			Vertex south = cntr - vec4(factor,0,0,0);
			Vertex east = cntr + vec4(0,factor,0,0);
			Vertex west = cntr - vec4(0,factor,0,0);
			Vertex top = cntr + vec4(0,0,factor,0);
			Vertex bot = cntr - vec4(0,0,factor,0);

			vector<Vertex> segments;
			segments.push_back(north);
			segments.push_back(south);
			segments.push_back(east);
			segments.push_back(west);
			segments.push_back(top);
			segments.push_back(bot);

			m_renderer->DrawLineSegments(segments, Rgb(1, 0, 0));
		}
	}
	
	m_renderer->SwapBuffers();
}

void Scene::drawWorldAxes()
{
	m_renderer->DrawLine3D(vec3(0,0,0), vec3(3,0,0), Rgb(0.5, 0, 0) );
	m_renderer->DrawLine3D(vec3(0,0,0), vec3(0,3,0), Rgb(0, 0.5, 0) );
	m_renderer->DrawLine3D(vec3(0,0,0), vec3(0,0,3), Rgb(0, 0.5, 0.5) );
}

void Scene::drawModelAxes(Model* m)
{
	vector<vec3> modelAxes = m->coordinates();
	vec3 modelOrigin = m->origin();
	m_renderer->DrawLine3D(modelOrigin, modelAxes[0], Rgb(0.5,0.5,1));
	m_renderer->DrawLine3D(modelOrigin, modelAxes[1], Rgb(0.5,0.5,1));
	m_renderer->DrawLine3D(modelOrigin, modelAxes[2], Rgb(0.5,0.5,1));
}

void Scene::AddCamera(Camera c)
{
	Camera* cc = new Camera(c);
	cameras.push_back(cc);
	activeCamera = cameras.size() - 1;
	m_renderer->SetCamera(cameras[activeCamera]);
}

void Scene::Clean()
{
	activeCamera = -1;
	activeLight = -1;
	activeModel = -1;
	models.clear();
	cameras.clear();

	m_renderer->SwapBuffers();
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

inline MeshModel* Scene::ActiveModel()
{
	if(activeModel == -1  || models.size() <= activeModel)
		return NULL;
	MeshModel* mmodel = dynamic_cast<MeshModel*>( models[activeModel]);
	return mmodel;
}

void Scene::ToggleActiveModel()
{
	if (activeModel == -1)
		return;
	activeModel += 1;
	activeModel %= models.size();
}

void Scene::ToggleActiveCamera()
{
	if (activeCamera == -1)
		return;
	activeCamera += 1;
	activeCamera %= cameras.size();
	m_renderer->SetCamera(cameras[activeCamera]);
}

bool Scene::ToggleShowCameras()
{
	bool oldval = drawCameras;
	drawCameras = ! drawCameras;
	return oldval;
}

bool Scene::ToggleShowWorldFrame()
{
	bool oldval = drawWorldFrame;
	drawWorldFrame = ! drawWorldFrame;
	return oldval;
}