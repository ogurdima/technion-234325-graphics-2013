#include "StdAfx.h"
#include "MeshModel.h"
#include "vec.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

vec4 hmgFromVec3(const vec3& v)
{
	return vec4(v.x,v.y,v.z,1);
}

vec3 vec3fFromStream(std::istream & aStream)
{
	float x, y, z;
	aStream >> x >> std::ws >> y >> std::ws >> z;
	return vec3(x, y, z);
}

vec2 vec2fFromStream(std::istream & aStream)
{
	float x, y;
	aStream >> x >> std::ws >> y;
	return vec2(x, y);
}

MeshModel::MeshModel(string fileName)
{
	loadFile(fileName);
}

MeshModel::~MeshModel(void)
{
}

void MeshModel::loadFile(string fileName)
{
	ifstream ifile(fileName.c_str());

	// while not end of file
	while (!ifile.eof())
	{
		// get line
		string curLine;
		getline(ifile, curLine);

		// read type of the line
		istringstream issLine(curLine);
		string lineType;

		issLine >> std::ws >> lineType;

		// based on the type parse data
		if (lineType == "v") 
			_vertices.push_back(hmgFromVec3(vec3fFromStream(issLine)));
		else if (lineType == "vn") 
			_normals.push_back(hmgFromVec3(vec3fFromStream(issLine)));
		else if (lineType == "f") 
			_faces.push_back(Face(issLine));
		else if (lineType == "#" || lineType == "")
		{
			// comment / empty line
		}
		else
		{
			cout<< "Found unknown line Type \"" << lineType << "\"" << endl;
		}
	}

	//Vertex_positions is an array of vec3. Every three elements define a triangle in 3D.
	//If the face part of the obj is
	//f 1 2 3
	//f 1 3 4
	//Then vertex_positions should contain:
	//vertex_positions={v1,v2,v3,v1,v3,v4}

	//vertex_positions = new vec3[faces.size() * 3];

	// iterate through all stored faces and create triangles
	//int k=0;
	//for (vector<Face>::iterator it = faces.begin(); it != faces.end(); ++it)
	//{
	//	for (int i = 0; i < 3; i++) // Assuming all faces are constructed with 3 verticies
	//	{
	//		vertex_positions[k++] = vec3(vertices[it->v[i] - 1]); // Pushing only verticies (w/o normals/textures)
	//	}
	//}
}

void MeshModel::draw(Renderer * r, Rgb color)
{
	vector<Vertex> vp = transformVertices();
	r->Draw(vp, color);
}


vector<Vertex> MeshModel::transformVertices()
{
	vector<Vertex> vertex_positions;

	// iterate through all stored faces and create triangles
	//int k=0;
	for (vector<Face>::iterator it = _faces.begin(); it != _faces.end(); ++it)
	{
		for (int i = 0; i < 3; i++) // Assuming all faces are constructed with 3 verticies
		{
			vertex_positions.push_back( _world_transform * _vertices[it->v[i] - 1] ); // Pushing only verticies (w/o normals/textures)
		}
	}
	return vertex_positions;
}

void MeshModel::addLeftWorldTransformation(mat4 transform)
{
	_world_transform = transform * _world_transform;
}

vector<vec4> MeshModel::coordinates() {
	vector<vec4> v;
	v.push_back(_world_transform * vec4(1,0,0,0));
	v.push_back(_world_transform * vec4(0,1,0,0));
	v.push_back(_world_transform * vec4(0,0,1,0));
	return v;
}

vec4 MeshModel::origin()
{
	return _world_transform * vec4(0,0,0,1);
}