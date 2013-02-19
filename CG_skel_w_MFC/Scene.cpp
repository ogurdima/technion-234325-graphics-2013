#include "stdafx.h"
#include "Scene.h"
#include "MeshModel.h"
#include <string>

using namespace std;

Scene::Scene(Renderer *renderer) : 
renderer(renderer),
activeModel(-1),
activeLight(-1),
activeCamera(-1),
drawCameras(false),
drawWorldFrame(false),
drawLights(false)
{
	this->renderer->SetShading(PHONG);
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

void Scene::LoadOBJModel(string fileName)
{
	MeshModel *model = new MeshModel(fileName);
	model->BindToRenderer(renderer);
	models.push_back(model);
	activeModel = models.size() - 1;
}

void Scene::SetLights()
{
	vector<vec4> lightDirections;
	vector<vec3> parlightColors;
	vector<vec4> lightPositions;
	vector<vec3> ptlightColors;
	for (int i = 0; i < lights.size(); i++)
	{
		if (lights[i]->lightType == REGULAR_L && lights[i]->lightSource == PARALLEL_S)
		{
			lightDirections.push_back(cameras[activeCamera]->View() * lights[i]->direction);
			parlightColors.push_back(lights[i]->lightColor.toVec3());
			//renderer->DrawParallelSource(lights[i]->lightColor, lights[i]->direction, cameras[activeCamera]->Projection()*cameras[activeCamera]->View());
		}
		else if (lights[i]->lightType == REGULAR_L && lights[i]->lightSource == POINT_S)
		{
			lightPositions.push_back(cameras[activeCamera]->View() * lights[i]->location);
			ptlightColors.push_back(lights[i]->lightColor.toVec3());
			//renderer->DrawPointSource(lights[i]->lightColor);
		}
	}

	renderer->SetParallelLights(lightDirections, parlightColors);
	renderer->SetPointLights(lightPositions, ptlightColors);
}

void Scene::DrawWorldAxes()
{
	Camera* ac = ActiveCam();
	vector<vec4> lineEp;
	vector<vec3> clrs;
	lineEp.push_back(vec4(0,0,0,1));
	clrs.push_back(vec3(1,0,0));
	lineEp.push_back(vec4(10,0,0,1));
	clrs.push_back(vec3(1,0,0));
	lineEp.push_back(vec4(0,0,0,1));
	clrs.push_back(vec3(0,1,0));
	lineEp.push_back(vec4(0,10,0,1));
	clrs.push_back(vec3(0,1,0));
	lineEp.push_back(vec4(0,0,0,1));
	clrs.push_back(vec3(0,0,1));
	lineEp.push_back(vec4(0,0,10,1));
	clrs.push_back(vec3(1,0,1));

	renderer->SetShading(LINE);
	renderer->SetCamera(ac->View(), ac->Projection());
	renderer->DrawWFLines( lineEp, clrs);
	renderer->FinishShading();
}

void Scene::Draw()
{
	if(! IsLegal())
		return;
	cout << "Scene::Draw" << endl;
	Camera* ac = ActiveCam();
	ShadingType oldSt = renderer->Shading();

	renderer->InitDraw();
	DrawWorldAxes();

	if(1)
	{
		renderer->SetShading(SILHOUETTE);
		renderer->SetCamera(ac->View(), ac->Projection());
		renderer->EnableFrontFaceCull();
		for (int i = 0; i < models.size(); i++)
		{
			models[i]->QuickRebind(renderer);
			models[i]->Draw(renderer);
		}
		renderer->DisableFrontFaceCull();
		renderer->FinishShading();

		renderer->SetShading(oldSt);
		renderer->SetCamera(ac->View(), ac->Projection());
		SetLights();
		for (int i = 0; i < models.size(); i++)
		{
			models[i]->QuickRebind(renderer);
			models[i]->Draw(renderer);
		}
		renderer->FinishShading();
	}
	else
	{
		renderer->SetShading(oldSt);
		renderer->SetCamera(ac->View(), ac->Projection());
		SetLights();
		for (int i = 0; i < models.size(); i++)
		{
			cout << "Model[" << i << "]";
			models[i]->Draw(renderer);
		}
		renderer->FinishShading();
	}
	
	renderer->FinishDraw();
}

void Scene::AddCamera(Camera c)
{
	Camera* cc = new Camera(c);
	cameras.push_back(cc);
	activeCamera = cameras.size() - 1;
}

void Scene::AddLight(Light l)
{
	Light* ll = new Light(l);
	lights.push_back(ll);
	activeLight = lights.size() - 1;
}

void Scene::AddMeshModel(MeshModel m )
{
	MeshModel* mm = new MeshModel(m);
	models.push_back(mm);
	activeModel = models.size() - 1;
}

void Scene::RemoveLights()
{
	activeLight = -1;
	lights.clear();
}

void Scene::RemoveCameras()
{
	activeCamera = -1;
	cameras.clear();
}

void Scene::RemoveGeometry()
{
	activeModel = -1;
	models.clear();
}

bool Scene::IsLegal() {
	return (activeCamera != -1  && cameras.size() > activeCamera);
}

Camera* Scene::ActiveCam()
{
	if(activeCamera == -1 || cameras.size() <= activeCamera)
		return NULL;
	return cameras[activeCamera];
}

Light* Scene::ActiveLight()
{
	if(activeLight == -1 || lights.size() <= activeLight)
		return NULL;
	return lights[activeLight];
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
}

void Scene::ToggleActiveLight()
{
	if (activeLight == -1)
		return;
	activeLight += 1;
	activeLight %= lights.size();
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

void Scene::SetShading(ShadingType s)
{
	if (s != FLAT && s != GOURAUD && s != PHONG && s != TOON)
	{
		return;
	}

	renderer->SetShading(s);
	for (int i = 0; i < models.size(); i++)
	{
		models[i]->Unbind(renderer);
		models[i]->BindToRenderer(renderer);
	}
}