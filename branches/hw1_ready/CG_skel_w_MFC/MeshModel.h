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
	vector<vec4> _faceNormals;

	mat4 _world_transform;
	mat4 _normal_transform;

	bool _drawVN;
	bool _drawFN;
	bool _drawBB;
	bool _drawMF;

	void CalculateFaceNormals();

public:
	MeshModel() :
	_drawBB(false),
	_drawVN(false),
	_drawFN(false),
	_drawMF(false)
	{
		_world_transform = Identity4();
		_normal_transform = Identity4();
	}

	MeshModel(const MeshModel& rhs) {
		_world_transform = rhs._world_transform;
		_normal_transform = rhs._normal_transform;
		_faces = rhs._faces;
		_vertices = rhs._vertices;
		_normals = rhs._normals;
		_faceNormals = rhs._faceNormals;
		_drawVN = rhs._drawVN;
		_drawFN = rhs._drawFN;
		_drawBB = rhs._drawBB;
		_drawMF = rhs._drawMF;
	}

	vector<Vertex> transformVertices();
	vector<vec4> transformNormals(float len = 1);
	vector<Vertex> transformFaceNormals(float len = 1);

	vector<Vertex> transformVertices(vector<Vertex> inModelCoords);
	MeshModel(string fileName);
	~MeshModel(void);
	void loadFile(string fileName);
	void virtual draw(Renderer * r, Rgb color = Rgb(0.7,0.7,0.7) );
	
	
	void Rotate(mat4 m);
	void Scale(mat4 m);
	void Translate(mat4 m);
	vector<vec3> virtual coordinates();
	vec3 virtual origin();

	virtual void drawBoundingBox(Renderer * r = NULL, Rgb color = Rgb(0.4,0.4,0.4) );

	
	bool ToggleShowFaceNormals();
	bool ToggleShowBoundingBox();
	bool ToggleShowVertexNormals();
	bool ToggleShowModelFrame();

};
