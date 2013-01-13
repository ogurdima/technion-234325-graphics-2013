#pragma once

#include <vector>
#include <string>
#include "Renderer.h"
#include "Camera.h"
#include "Model.h"
#include "MeshModel.h"
#include "Rgb.h"
#include "Light.h"
#include "InitShader.h"

using namespace std;
 
typedef enum
{
	FLAT = 0,
	GOURAUD,
	PHONG
} ShadingType;


class Scene 
{

protected:
	vector<Model*> models;
	vector<Light*> lights;
	vector<Camera*> cameras;
	Renderer* m_renderer;
	vector<vec3> anchored;
	int activeModel;
	int activeLight;
	int activeCamera;
	bool drawCameras;
	bool drawWorldFrame;
	bool drawLights;

	GLuint oglPrograms[3];
	void initShaders();

public:
	~Scene();
	Scene(Renderer *renderer);
	void loadOBJModel(string fileName);
	void RemoveGeometry();
	void RemoveCameras();
	void RemoveLights();
	void AddCamera(Camera c);
	void AddMeshModel(MeshModel m );
	void AddLight(Light l);
	
	void SetActiveModelAnchor();
	vector<vec3> getAnchoredModelCoordinates();

	void RotateActiveModel(mat4 rotMatrix);
	void TranslateActiveModel(mat4 transMatrix);
	void AddActiveModelTransform(mat4 trans);

	Camera* ActiveCam();
	MeshModel* ActiveModel();

	void draw();
	void drawWorldAxes();
	void drawModelAxes(Model* m);
	void DrawSnowflake(vec4 at, float len, Rgb col = Rgb(0,0,0));
	void DrawLights();
	void DrawCameras();
	bool isLegal();
	void ToggleActiveModel();
	void ToggleActiveCamera();
	bool ToggleShowCameras();
	bool ToggleShowWorldFrame();
	bool ToggleShowLights();

	
};