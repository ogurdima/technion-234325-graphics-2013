#pragma once
#include "vec.h"
#include "mat.h"
#include "Vertex.h"
#include "Face.h"
#include "Model.h"
#include <string>

using namespace std;

class MeshModel : public Model
{
protected :

	vector<Face> _faces;
	vector<Vertex> _vertices;
	vector<vec4> _normals;

	mat4 _world_transform;
	mat3 _normal_transform;

public:
	MeshModel() {
		_world_transform = Identity4();
		_normal_transform = Identity3();
	}

	MeshModel(const MeshModel& rhs) {
		_world_transform = rhs._world_transform;
		_normal_transform = rhs._normal_transform;
		_faces = rhs._faces;
		_vertices = rhs._vertices;
		_normals = rhs._normals;
	}

	vector<Vertex> transformVertices();
	vector<vec4> transformNormals(float len );

	vector<Vertex> transformVertices(vector<Vertex> inModelCoords);
	MeshModel(string fileName);
	~MeshModel(void);
	void loadFile(string fileName);
	void virtual draw(Renderer * r, Rgb color = Rgb(0.7,0.7,0.7) );
	void virtual drawNormals(Renderer * r, float len, Rgb color = Rgb(0.7,0.7,0.7) );

	
	void virtual addLeftWorldTransformation(mat4 transform);
	vector<vec3> virtual coordinates();
	vec3 virtual origin();

	virtual void drawBoundingBox(Renderer * r = NULL, Rgb color = Rgb(0.4,0.4,0.4) );

};
