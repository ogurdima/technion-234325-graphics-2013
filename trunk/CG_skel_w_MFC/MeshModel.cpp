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

vector<Vertex> MeshModel::transformVertices(vector<Vertex> inModelCoords)
{
	for(vector<Vertex>::iterator it = inModelCoords.begin();   it != inModelCoords.end(); it++) 
	{
		vec4 v = _world_transform * (*it);
		*it = v;
	}
	return inModelCoords; // it is actually now in world coordinates
}

void MeshModel::addLeftWorldTransformation(mat4 transform)
{
	_world_transform = transform * _world_transform;
}

vector<vec3> MeshModel::coordinates() {
	vector<vec3> v;
	vec4 tx = _world_transform * vec4(1,0,0,0);
	vec4 ty = _world_transform * vec4(0,1,0,0);
	vec4 tz = _world_transform * vec4(0,0,1,0);
	v.push_back( vec3(tx.x, tx.y, tx.z) );
	v.push_back( vec3(ty.x, ty.y, ty.z) );
	v.push_back( vec3(tz.x, tz.y, tz.z) );
	return v;
}

vec3 MeshModel::origin()
{
	vec4 orig4 = _world_transform * vec4(0,0,0,1);
	return vec3( orig4.x, orig4.y, orig4.z );
}

void MeshModel::drawBoundingBox(Renderer * r, Rgb color)
{
	typedef enum {X = 0, Y, Z} COORD;
	if (r == NULL || _vertices.size() == 0)
		return;
	Vertex v = _vertices[0];
	float max[3] = {};
	float min[3] = {};
	min[X] = max[X] = v.x;
	min[Y] = max[Y] = v.y;
	min[Z] = max[Z] = v.z;
	for (int i = 1; i < _vertices.size(); i++) {
		v = _vertices[i];
		min[X] = min(min[X], v.x);
		min[Y] = min(min[Y], v.y);
		min[Z] = min(min[Z], v.z);
		max[X] = max(max[X], v.x);
		max[Y] = max(max[Y], v.y);
		max[Z] = max(max[Z], v.z);
	}
	vector<Vertex> rims;

	//Hello shitcode!

	rims.clear();
	rims.push_back( Vertex(min[X],min[Y],min[Z],1) ); rims.push_back( Vertex(max[X],min[Y],min[Z],1) ); rims.push_back( Vertex(max[X],max[Y],min[Z],1) ); rims.push_back( Vertex(min[X],max[Y],min[Z],1) ); rims.push_back( Vertex(min[X],min[Y],min[Z],1) );
	rims = transformVertices(rims);
	r->DrawPolyline(rims, color);

	rims.clear();
	rims.push_back( Vertex(min[X],min[Y],max[Z],1) ); rims.push_back( Vertex(max[X],min[Y],max[Z],1) ); rims.push_back( Vertex(max[X],max[Y],max[Z],1) ); rims.push_back( Vertex(min[X],max[Y],max[Z],1) ); rims.push_back( Vertex(min[X],min[Y],max[Z],1) );
	rims = transformVertices(rims);
	r->DrawPolyline(rims, color);

	rims.clear();
	rims.push_back( Vertex(min[X],min[Y],min[Z],1) ); rims.push_back( Vertex(max[X],min[Y],min[Z],1) ); rims.push_back( Vertex(max[X],min[Y],max[Z],1) ); rims.push_back( Vertex(min[X],min[Y],max[Z],1) ); rims.push_back( Vertex(min[X],min[Y],min[Z],1) );
	rims = transformVertices(rims);
	r->DrawPolyline(rims, color);

	rims.clear();
	rims.push_back( Vertex(min[X],max[Y],min[Z],1) ); rims.push_back( Vertex(max[X],max[Y],min[Z],1) ); rims.push_back( Vertex(max[X],max[Y],max[Z],1) ); rims.push_back( Vertex(min[X],max[Y],max[Z],1) ); rims.push_back( Vertex(min[X],max[Y],min[Z],1) );
	rims = transformVertices(rims);
	r->DrawPolyline(rims, color);

	rims.clear();
	rims.push_back( Vertex(min[X],min[Y],min[Z],1) ); rims.push_back( Vertex(min[X],max[Y],min[Z],1) ); rims.push_back( Vertex(min[X],max[Y],max[Z],1) ); rims.push_back( Vertex(min[X],min[Y],max[Z],1) ); rims.push_back( Vertex(min[X],min[Y],min[Z],1) );
	rims = transformVertices(rims);
	r->DrawPolyline(rims, color);

	rims.clear();
	rims.push_back( Vertex(max[X],min[Y],min[Z],1) ); rims.push_back( Vertex(max[X],max[Y],min[Z],1) ); rims.push_back( Vertex(max[X],max[Y],max[Z],1) ); rims.push_back( Vertex(max[X],min[Y],max[Z],1) ); rims.push_back( Vertex(max[X],min[Y],min[Z],1) );
	rims = transformVertices(rims);
	r->DrawPolyline(rims, color);
}