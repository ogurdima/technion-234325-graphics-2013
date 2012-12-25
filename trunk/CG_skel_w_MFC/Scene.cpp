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
drawWorldFrame(false),
drawLights(false)
{

}

Scene::Scene(Renderer *renderer) : 
m_renderer(renderer),
activeModel(-1),
activeLight(-1),
activeCamera(-1),
drawCameras(false),
drawWorldFrame(false),
drawLights(false)
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

	m_renderer->SetLights(lights);
	for (int i = 0; i < models.size(); i++) {
		Model* model = models[i];
		model->draw(m_renderer);
	}

	if (drawLights)
	{
		DrawLights();
	}
	if (drawCameras) 
	{
		DrawCameras();
	}
	
	m_renderer->SwapBuffers();
}

void Scene::DrawSnowflake(vec4 at, float len, Rgb col)
{
	Vertex cntr = at;
	Vertex north = cntr + vec4(len,0,0,0);
	Vertex south = cntr - vec4(len,0,0,0);
	Vertex east = cntr + vec4(0,len,0,0);
	Vertex west = cntr - vec4(0,len,0,0);
	Vertex top = cntr + vec4(0,0,len,0);
	Vertex bot = cntr - vec4(0,0,len,0);

	vector<Vertex> segments;
	segments.push_back(north);
	segments.push_back(south);
	segments.push_back(east);
	segments.push_back(west);
	segments.push_back(top);
	segments.push_back(bot);

	m_renderer->DrawNgons(segments, 2, col);
}

void Scene::DrawCameras()
{
	for (int i = 0; i < cameras.size(); i++) {
			Camera* c = cameras[i];
			if (c == ActiveCam()) continue;
			float factor = 0.3;
			Vertex cntr = Vertex(c->Eye(), 1);
			Vertex focusPt = Vertex(c->At(), 1);
			DrawSnowflake(cntr, factor, Rgb(0,0,1));

			vector<Vertex> segments;
			segments.push_back(cntr);
			segments.push_back((focusPt - cntr) * 3 * factor);
			//m_renderer->DrawNgons(segments, 2, Rgb(0, 0, 1));
		}
}

void Scene::DrawLights()
{
	for (int i = 0; i < lights.size(); i++) {
		Light* l = lights[i];
		if (l->lightType != REGULAR_L || l->lightSource != POINT_S) {
			continue;
		}
		float factor = 0.4;
		Vertex cntr = l->location;
		cntr.w = 1;
		DrawSnowflake(cntr, factor, Rgb(1,0.5,0));
		
		// Ray from each point ligt source to the camera focus point
		vector<Vertex> segments;
		segments.push_back(cntr);
		segments.push_back(Vertex(ActiveCam()->At(), 1));
		m_renderer->DrawNgons(segments, 2, Rgb(1, 0.5, 0));
	}
}

void Scene::drawWorldAxes()
{
	vector<Vertex> axeses;
	axeses.push_back(vec4(0,0,0,1));
	axeses.push_back(vec4(10,0,0,1));
	m_renderer->DrawNgons(axeses,2,Rgb(1,0,0));
	axeses.clear();
	axeses.push_back(vec4(0,0,0,1));
	axeses.push_back(vec4(0,10,0,1));
	m_renderer->DrawNgons(axeses,2,Rgb(0,1,0));
	axeses.clear();
	axeses.push_back(vec4(0,0,0,1));
	axeses.push_back(vec4(0,0,10,1));
	m_renderer->DrawNgons(axeses,2,Rgb(0,0,1));	
}

void Scene::drawModelAxes(Model* m)
{
	vector<vec3> modelAxes = m->coordinates();
	vec3 modelOrigin = m->origin();
	vector<vec4> axeses;
	axeses.push_back(vec4(modelOrigin,1));
	axeses.push_back(vec4(modelAxes[0],1));
	m_renderer->DrawNgons(axeses,2,Rgb(1,0,0));
	axeses.clear();
	axeses.push_back(vec4(modelOrigin,1));
	axeses.push_back(vec4(modelAxes[1],1));
	m_renderer->DrawNgons(axeses,2,Rgb(0,1,0));
	axeses.clear();
	axeses.push_back(Vertex(modelOrigin,1));
	axeses.push_back(Vertex(modelAxes[2],1));
	m_renderer->DrawNgons(axeses,2,Rgb(0,0,1));
}

void Scene::AddCamera(Camera c)
{
	Camera* cc = new Camera(c);
	cameras.push_back(cc);
	activeCamera = cameras.size() - 1;
	m_renderer->SetCamera(cameras[activeCamera]);
}

void Scene::AddLight(Light l)
{
	Light* ll = new Light(l);
	lights.push_back(ll);
}

void Scene::RemoveLights()
{
	activeLight = -1;
	lights.clear();
	m_renderer->SwapBuffers();
}

void Scene::RemoveCameras()
{
	activeCamera = -1;
	cameras.clear();
	m_renderer->SwapBuffers();
}

void Scene::RemoveGeometry()
{
	activeModel = -1;
	models.clear();
	m_renderer->SwapBuffers();
}

bool Scene::isLegal() {
	return (activeCamera != -1  && cameras.size() > activeCamera);
}

Camera* Scene::ActiveCam()
{
	if(activeCamera == -1 || cameras.size() <= activeCamera)
		return NULL;
	return cameras[activeCamera];
}

MeshModel* Scene::ActiveModel()
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

bool Scene::ToggleShowLights()
{
	bool oldval = drawLights;
	drawLights = !drawLights;
	return oldval;
}

void Scene::AddMeshModel(MeshModel m )
{
	MeshModel* mm = new MeshModel(m);
	models.push_back(mm);
	activeModel = models.size() - 1;
}
