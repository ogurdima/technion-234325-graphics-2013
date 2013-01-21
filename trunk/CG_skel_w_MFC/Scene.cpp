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
drawLights(false),
shading(FLAT)
{
	initShaders();
	renderer->SetShadingProgram(oglPrograms[FLAT]);
	oglCameraBind = renderer->BindCamera();
}

void Scene::initShaders()
{
	oglPrograms[FLAT] = InitShader("Shaders/flat_vshader.glsl", "Shaders/flat_fshader.glsl");
	oglPrograms[GOURAUD] = InitShader("Shaders/flat_vshader.glsl", "Shaders/flat_fshader.glsl");
	oglPrograms[PHONG] = InitShader("Shaders/phong_vshader.glsl", "Shaders/phong_fshader.glsl");
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
	model->BindToRenderer(renderer, shading);
	models.push_back(model);
	activeModel = models.size() - 1;
}

void Scene::draw()
{
	if(! isLegal())
		return;

	cout << "Scene::Draw" << endl;

	vector<GLuint> h;
	vector<mat4> v;

	h.push_back(oglCameraBind.viewHnd);
	v.push_back(cameras[activeCamera]->View());
	h.push_back(oglCameraBind.projectHnd);
	v.push_back(cameras[activeCamera]->Projection());

	renderer->SetUniformMatrices(h, v);

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
		}
		else if (lights[i]->lightType == REGULAR_L && lights[i]->lightSource == POINT_S)
		{
			lightPositions.push_back(cameras[activeCamera]->View() * lights[i]->location);
			ptlightColors.push_back(lights[i]->lightColor.toVec3());
		}

	}
	renderer->SetParallelLights(lightDirections, parlightColors);
	renderer->SetPointLights(lightPositions, ptlightColors);

	for (int i = 0; i < models.size(); i++)
	{
		cout << "Model[" << i << "]";
		models[i]->draw(renderer);
	}
	renderer->SwapBuffers();
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


void Scene::SetShading(ShadingType s)
{
	if (s != FLAT && s != GOURAUD && s != PHONG)
	{
		return;
	}
	shading = s;
	renderer->SetShadingProgram(oglPrograms[shading]);
	for (int i = 0; i < models.size(); i++)
	{
		models[i]->BindToRenderer(renderer, shading);
	}
}