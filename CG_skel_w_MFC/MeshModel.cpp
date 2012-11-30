#include "StdAfx.h"
#include "MeshModel.h"
#include "vec.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

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

MeshModel::MeshModel(string fileName) :
_drawBB(false),
_drawVN(false),
_drawFN(false),
_drawMF(false)
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
			_vertices.push_back(vec4(vec3fFromStream(issLine),1));
		else if (lineType == "vn")
		{
			//cout << "NORMAL" << endl;
			_normals.push_back(vec4(vec3fFromStream(issLine),0));
		}
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
	if (_drawBB) 
	{
		drawBoundingBox(r);
	}
	if (_drawVN)
	{
		r->DrawLineSegments(transformNormals(0.1));
	}
	if (_drawFN)
	{
		//compute face normals and draw them
	}
	if (_drawMF)
	{
		vector<vec3> fr = coordinates();
		vec3 o = origin();
		r->DrawLine3D(o, fr[0], Rgb(0.5,0.5,1));
		r->DrawLine3D(o, fr[1], Rgb(0.5,0.5,1));
		r->DrawLine3D(o, fr[2], Rgb(0.5,0.5,1));
	}
}

	


void MeshModel::drawNormals(Renderer * r,float len, Rgb color)
{
	
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

// returns vertex-normal list
vector<Vertex> MeshModel::transformNormals(float len)
{
	vector<Vertex> retVal;
	for (vector<Face>::iterator it = _faces.begin(); it != _faces.end(); ++it)
	{
		for (int i = 0; i < 3; i++) // Assuming all faces are constructed with 3 verticies
		{
			if(it->vn[i] == 0) // normal is not defined for vertex
				continue;
			vec4 normalEnd = _vertices[it->v[i] - 1] + len * normalize((_normals[it->vn[i] - 1]));
			retVal.push_back( _world_transform * _vertices[it->v[i] - 1] ); // push vertex
			retVal.push_back( _world_transform * normalEnd ); // push normal endpoint
		}
	}
	return retVal;
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
	//mat4 stam = transpose(_world_transform);
	vec4 tx = _world_transform * vec4(1,0,0,1);
	vec4 ty = _world_transform * vec4(0,1,0,1);
	vec4 tz = _world_transform * vec4(0,0,1,1);
	vec4 orig4 = _world_transform * vec4(0,0,0,1);

	v.push_back( vec3(tx.x / tx.w, tx.y / tx.w, tx.z / tx.w) );
	v.push_back( vec3(ty.x / ty.w, ty.y / ty.w, ty.z / ty.w) );
	v.push_back( vec3(tz.x / tz.w, tz.y / tz.w, tz.z / tz.w) );
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


bool MeshModel::ToggleShowFaceNormals()
{
	bool oldval = _drawFN;
	_drawFN = ! _drawFN;
	return oldval;
}

bool MeshModel::ToggleShowBoundingBox()
{
	bool oldval = _drawBB;
	_drawBB = ! _drawBB;
	return oldval;
}

bool MeshModel::ToggleShowVertexNormals()
{
	bool oldval = _drawVN;
	_drawVN = ! _drawVN;
	return oldval;
}

bool MeshModel::ToggleShowModelFrame() {
	bool oldval = _drawMF;
	_drawMF = ! _drawMF;
	return oldval;
}