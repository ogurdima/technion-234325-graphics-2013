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
	MeshModel() {
		_world_transform = Identity4();
		_normal_transform = Identity3();
	}

	vector<Face> _faces;
	vector<Vertex> _vertices;
	vector<vec4> _normals;

	mat4 _world_transform;
	mat3 _normal_transform;

public:
	vector<Vertex> transformVertices();
	vector<vec4> coordinates();
	MeshModel(string fileName);
	~MeshModel(void);
	void loadFile(string fileName);
	void draw(Renderer * r);
	void addLeftWorldTransformation(mat4 transform);
};
