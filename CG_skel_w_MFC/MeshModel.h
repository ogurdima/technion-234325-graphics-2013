#pragma once
#include "scene.h"
#include "vec.h"
#include "mat.h"
#include "Vertex.h"
#include "Face.h"
#include <string>

using namespace std;

class MeshModel : public Model
{
protected :
	MeshModel() {}

	//vec3 *vertex_positions;

	vector<Face> _faces;
	vector<Vertex> _vertices;
	vector<vec4> _normals;

	//add more attributes
	mat4 _world_transform;
	mat3 _normal_transform;

public:

	MeshModel(string fileName);
	~MeshModel(void);
	void loadFile(string fileName);
	void draw();
	
};
