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
drawSilhouette(false)
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
	//model->BindToRenderer(renderer);
	renderer->BindModel(model);
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

void Scene::BindReflectionMaps()
{
	Camera* ac = ActiveCam();
	for (int i = 0; i < models.size(); i++)
	{
		if(!models[i]->GetDrawEnvMap())
		{
			continue;
		}
		GLint viewport[4];
		glGetIntegerv( GL_VIEWPORT, viewport );
		glViewport(0,0,256,256);
		bool aaWasEnabled = glIsEnabled( GL_MULTISAMPLE );
		glDisable( GL_MULTISAMPLE );
		vec4 bbc = models[i]->Transformation() * vec4( models[i]->BoundingBoxCenter(), 1);
		vec3 bbCenter = vec3( bbc.x/bbc.w , bbc.y/ bbc.w, bbc.z/bbc.w);
		vec3 v = bbCenter - ac->Eye();
		vec3 uppp = cross( cross( ac->At() - ac->Eye(), ac->Up()), ac->At() - ac->Eye());
		vec3 xxx = normalize(cross(v, uppp));
		vec3 yyy = normalize(cross(xxx, v));
		vec3 zzz = normalize(cross(xxx, yyy));

		AddReflectionTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, LookAtMat(bbCenter, bbCenter-xxx, -yyy), PerspectiveMat(72.3,1, 0.01, 2* ac->ZFar()), models[i]);
		AddReflectionTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, LookAtMat(bbCenter, bbCenter+yyy, zzz),  PerspectiveMat(72.3,1, 0.01, 2* ac->ZFar()), models[i]);
		AddReflectionTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, LookAtMat(bbCenter, bbCenter-yyy, -zzz), PerspectiveMat(72.3,1, 0.01, 2* ac->ZFar()), models[i]);
		AddReflectionTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, LookAtMat(bbCenter, bbCenter+zzz, -yyy), PerspectiveMat(72.3,1, 0.01, 2* ac->ZFar()), models[i]);
		AddReflectionTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, LookAtMat(bbCenter, bbCenter-zzz, -yyy), PerspectiveMat(72.3,1, 0.01, 2* ac->ZFar()), models[i]);
		AddReflectionTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X, LookAtMat(bbCenter, bbCenter+xxx, -yyy), PerspectiveMat(72.3,1, 0.01, 2* ac->ZFar()), models[i]);
		
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		if (aaWasEnabled) {
			glEnable(GL_MULTISAMPLE);
		}
	}
	

}

void Scene::AddReflectionTexture(GLenum dir, mat4 view, mat4 projection, MeshModel* m)
{
	renderer->InitDraw();
	renderer->SetCamera(view, projection);
	SetLights();
	for (int j = 0; j < models.size(); j++)
	{
		if( models[j] == m)
			continue;
		renderer->DrawModel(models[j]);
	}
	renderer->CopyFrameToTexture(dir, m);
}

void Scene::Animation()
{
	for(int i = 0; i < models.size(); ++i)
	{
		//models[i]->ChangeColorAnimationParam(0.02);
		models[i]->Animation();
	}
}

void Scene::Draw()
{
	if(! IsLegal())
		return;
	//cout << "Scene::Draw" << endl;
	Camera* ac = ActiveCam();
	ShadingType oldSt = renderer->Shading();


	BindReflectionMaps();

	renderer->InitDraw();
	DrawWorldAxes();
	if(drawSilhouette)
	{
		renderer->SetShading(SILHOUETTE);
		renderer->SetCamera(ac->View(), ac->Projection());
		renderer->EnableFrontFaceCull();
		for (int i = 0; i < models.size(); i++)
		{
			renderer->DrawModel(models[i]);
		}
		renderer->DisableFrontFaceCull();
		renderer->FinishShading();

	}
	
	renderer->SetShading(oldSt);
	renderer->SetCamera(ac->View(), ac->Projection());
	SetLights();
	for (int i = 0; i < models.size(); i++)
	{
		if(!models[i]->GetDrawEnvMap())
			renderer->DrawModel(models[i]);
	}
	renderer->FinishShading();

	renderer->SetShading(ENV);
	renderer->SetCamera(ac->View(), ac->Projection());
	SetLights();
	for (int i = 0; i < models.size(); i++)
	{
		if(models[i]->GetDrawEnvMap())
			renderer->DrawModel(models[i]);
	}
	renderer->FinishShading();

	renderer->SetShading(oldSt);
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

bool Scene::ToggleShowSilhouette()
{
	bool oldval = drawSilhouette;
	drawSilhouette = !drawSilhouette;
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
//		models[i]->Unbind(renderer);
//		models[i]->BindToRenderer(renderer);
	}
}