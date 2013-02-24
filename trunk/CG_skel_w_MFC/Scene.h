#pragma once

#include <vector>
#include <string>
#include "Renderer.h"
#include "Camera.h"
#include "Model.h"
#include "MeshModel.h"
#include "Rgb.h"
#include "Light.h"

#define NOISE_WIDTH		128
#define NOISE_HEIGHT	128


using namespace std;

class Scene 
{

protected:
	double noise[NOISE_WIDTH][NOISE_HEIGHT]; 

	vector<MeshModel*>		models;
	vector<Light*>			lights;
	vector<Camera*>			cameras;
	Renderer*				renderer;

	int						activeModel;
	int						activeLight;
	int						activeCamera;

	bool					drawCameras;
	bool					drawWorldFrame;
	bool					drawLights;
	bool					drawSilhouette;


	void					SetLights();
	void					GenerateNoise();
	double					SmoothNoise(double x, double y);
	double					Turbulence(double x, double y, double size);
public:
	
	Scene(Renderer *renderer);
	~Scene();

	void LoadOBJModel(string fileName);
	void RemoveGeometry();
	void RemoveCameras();
	void RemoveLights();
	void AddCamera(Camera c);
	void AddMeshModel(MeshModel m );
	void AddLight(Light l);

	Camera* ActiveCam();
	Light* ActiveLight();
	MeshModel* ActiveModel();

	bool IsLegal();
	void Animation();

	void SetShading(ShadingType s);

	void Draw();

	void ToggleActiveModel();
	void ToggleActiveCamera();
	void ToggleActiveLight();
	bool ToggleShowCameras();
	bool ToggleShowWorldFrame();
	bool ToggleShowLights();
	bool ToggleShowSilhouette();

	void DrawWorldAxes();

	void AddReflectionTexture(GLenum dir, mat4 view, mat4 projection, MeshModel* m );
	void BindReflectionMaps();


	Texture					CalculateMarbleTexture();
	
};