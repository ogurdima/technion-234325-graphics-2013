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

	CalculateFaceNormals();
}

void MeshModel::draw(Renderer * r, Rgb color)
{
	vector<Vertex> vp = transformVertices();
	vector<vec4> normalPairs = transformNormals(1);
	vector<vec4> normals;
	if (normalPairs.size() != 0) {
		for (int i = 0; i < normalPairs.size(); i+=2)
		{
			normals.push_back(  normalize(normalPairs[i+1] - normalPairs[i])  );
		}
	}
	r->DDrawTriangles(vp, _defaultColor, normals, _vertexColors);
	//r->DrawNgons(vp,3,Rgb(0,1,0));
	if (_drawBB) 
	{
		drawBoundingBox(r);
	}
	if (_drawVN && _normals.size() > 0)
	{
		r->DrawNgons(transformNormals(0.1), 2, Rgb(0.31,0.58,0.93));
	}
	if (_drawFN)
	{
		//compute face normals and draw them
		r->DrawNgons(transformFaceNormals(0.1), 2, Rgb(0.85,0.60,0.60));
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

vector<Vertex> MeshModel::transformVertices(){
	vector<Vertex> vertex_positions;
	for (vector<Face>::iterator it = _faces.begin(); it != _faces.end(); ++it){
		for (int i = 0; i < 3; i++){
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
			if(it->vn[i] <= 0) // normal is not defined for vertex
				continue;
			vec4 normalStart = _world_transform * _vertices[it->v[i] - 1];
			vec4 normalEnd = normalStart + len * normalize(_normal_transform * _normals[it->vn[i] - 1]);
			retVal.push_back( normalStart ); // push vertex
			retVal.push_back( normalEnd ); // push normal endpoint
		}
	}
	return retVal;
}

vector<Vertex> MeshModel::transformFaceNormals(float len) 
{
	vector<Vertex> origNormalList;
	for (int i = 0; i < _faces.size(); i++) 
	{
		Face& f = _faces[i];
		vec4& n = _faceNormals[i];
		vec4 normalStart = (_vertices[f.v[0] - 1] + _vertices[f.v[1] - 1] + _vertices[f.v[2] - 1]) / 3;
		// Starting from here we are in world frame
		normalStart = _world_transform * normalStart;
		vec4 normalEnd = normalStart + len * normalize(_normal_transform * n);
		origNormalList.push_back(normalStart);
		origNormalList.push_back(normalEnd);
	}
	return origNormalList;
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

void MeshModel::Rotate(mat4 m)
{
	_world_transform = m * _world_transform;
	_normal_transform = m * _normal_transform;
}

void MeshModel::Translate(mat4 m)
{
	_world_transform = m * _world_transform;
	_normal_transform = m * _normal_transform;
}

void MeshModel::Scale(mat4 m)
{
	_world_transform = m * _world_transform;
	m[0][0] = 1 / m[0][0];
	m[1][1] = 1 / m[1][1];
	m[2][2] = 1 / m[2][2];
	_normal_transform = m * _normal_transform;
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
	rims.push_back( Vertex(min[X],min[Y],min[Z],1) ); rims.push_back( Vertex(max[X],min[Y],min[Z],1) ); rims.push_back( Vertex(max[X],max[Y],min[Z],1) ); rims.push_back( Vertex(min[X],max[Y],min[Z],1) ); 
	rims = transformVertices(rims);
	r->DrawNgons(rims, rims.size(), color);

	rims.clear();
	rims.push_back( Vertex(min[X],min[Y],max[Z],1) ); rims.push_back( Vertex(max[X],min[Y],max[Z],1) ); rims.push_back( Vertex(max[X],max[Y],max[Z],1) ); rims.push_back( Vertex(min[X],max[Y],max[Z],1) );
	rims = transformVertices(rims);
	r->DrawNgons(rims, rims.size(), color);

	rims.clear();
	rims.push_back( Vertex(min[X],min[Y],min[Z],1) ); rims.push_back( Vertex(max[X],min[Y],min[Z],1) ); rims.push_back( Vertex(max[X],min[Y],max[Z],1) ); rims.push_back( Vertex(min[X],min[Y],max[Z],1) );
	rims = transformVertices(rims);
	r->DrawNgons(rims, rims.size(), color);

	rims.clear();
	rims.push_back( Vertex(min[X],max[Y],min[Z],1) ); rims.push_back( Vertex(max[X],max[Y],min[Z],1) ); rims.push_back( Vertex(max[X],max[Y],max[Z],1) ); rims.push_back( Vertex(min[X],max[Y],max[Z],1) );
	rims = transformVertices(rims);
	r->DrawNgons(rims, rims.size(), color);

	rims.clear();
	rims.push_back( Vertex(min[X],min[Y],min[Z],1) ); rims.push_back( Vertex(min[X],max[Y],min[Z],1) ); rims.push_back( Vertex(min[X],max[Y],max[Z],1) ); rims.push_back( Vertex(min[X],min[Y],max[Z],1) );
	rims = transformVertices(rims);
	r->DrawNgons(rims, rims.size(), color);

	rims.clear();
	rims.push_back( Vertex(max[X],min[Y],min[Z],1) ); rims.push_back( Vertex(max[X],max[Y],min[Z],1) ); rims.push_back( Vertex(max[X],max[Y],max[Z],1) ); rims.push_back( Vertex(max[X],min[Y],max[Z],1) );
	rims = transformVertices(rims);
	r->DrawNgons(rims, rims.size(), color);
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

void MeshModel::CalculateFaceNormals()
{
	vec4 p1, p2, p3, d1, d2, crs;
	for (vector<Face>::iterator it = _faces.begin(); it != _faces.end(); ++it)
	{
		p1 = _vertices[it->v[0] - 1];
		p2 = _vertices[it->v[1] - 1];
		p3 = _vertices[it->v[2] - 1];
		d1 = p2 - p1; //assuming counterclockwise enumeration
		d2 = p3 - p2;
		crs = normalize(vec4(cross(d1, d2), 0));
		_faceNormals.push_back(crs);
	}
}

MaterialColor MeshModel::GetDefaultColor()
{
	return _defaultColor;
}

void MeshModel::SetDefaultColor(MaterialColor _c)
{
	_vertexColors.clear();
	_defaultColor = _c;
}

void MeshModel::SetRandomColor()
{
	vector<Vertex> vertices = transformVertices();
	srand(time(0));
	_vertexColors.clear();
	for(int i = 0; i < vertices.size(); ++i)
	{
		MaterialColor mc;
		mc.ambient = Rgb(rand(), rand(), rand());
		mc.emissive = Rgb(rand(), rand(), rand());
		mc.diffuse = Rgb(rand(), rand(), rand());
		mc.specular = Rgb(rand(), rand(), rand()); 
		_vertexColors.push_back(mc);
	}
}

void MeshModel::SetProgressiveColor()
{

}