#include "StdAfx.h"
#include "MeshModel.h"
#include "vec.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

static vec3 vec3fFromStream(std::istream & aStream);
static vec2 vec2fFromStream(std::istream & aStream);
static Rgb interpolate(float t, Rgb a, Rgb b);


MeshModel::MeshModel() :
_drawBB(false),
_drawVN(false),
_drawFN(false),
_drawMF(false),
_drawTexture(false),
_envMap(false)
{
	_world_transform = Identity4();
	_normal_transform = Identity4();
	_inner_transform = Identity4();
}

MeshModel::MeshModel(string fileName) :
_drawBB(false),
_drawVN(false),
_drawFN(false),
_drawMF(false),
_drawTexture(false),
_envMap(false)
{
	LoadFile(fileName);
}

MeshModel::MeshModel(const MeshModel& rhs) 
{
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
	_drawTexture = rhs._drawTexture;
	_envMap = rhs._envMap;
}

MeshModel::~MeshModel(void)
{
}

void MeshModel::LoadFile(string fileName)
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
		else if (lineType == "vt")
		{
			_textures.push_back(vec2fFromStream(issLine));
		}
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


// Transformations
void MeshModel::MFRotate(mat4 m)
{
	_inner_transform = m * _inner_transform;
}

void MeshModel::MFTranslate(mat4 m)
{
	_inner_transform = m * _inner_transform;
}

void MeshModel::WFRotate(mat4 m)
{
	_world_transform = m * _world_transform;
	_normal_transform = m * _normal_transform;
}

void MeshModel::WFTranslate(mat4 m)
{
	_world_transform = m * _world_transform;
	_normal_transform = m * _normal_transform;
}

void MeshModel::WFScale(mat4 m)
{
	_world_transform = m * _world_transform;
	m[0][0] = 1 / m[0][0];
	m[1][1] = 1 / m[1][1];
	m[2][2] = 1 / m[2][2];
	_normal_transform = m * _normal_transform;
}

vec3 MeshModel::Origin()
{
	vec4 orig4 = _world_transform * vec4(0,0,0,1);
	return vec3( orig4.x, orig4.y, orig4.z );
}

// Drawing options
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

bool MeshModel::SetDrawTexture(bool val)
{
	bool oldval = _drawTexture;
	_drawTexture = val;
	return oldval;
}

// Color manipulations
MaterialColor MeshModel::GetDefaultColor()
{
	return _defaultColor;
}

void MeshModel::SetDefaultColor(MaterialColor _c)
{
	_drawTexture = false;
	_vertexColors.clear();
	_defaultColor = _c;
}

void MeshModel::SetRandomColor()
{
	vector<Vertex> vertices = Triangles();
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
	vector<Vertex> vertices = Triangles();
	_vertexColors.clear();
	float yMin = vertices[0].y , yMax = vertices[0].y;
	for(int i = 1; i < vertices.size(); ++i)
	{
		if(vertices[i].y < yMin)
		{
			yMin = vertices[i].y;
		}
		if(vertices[i].y > yMax)
		{
			yMax = vertices[i].y;
		}
	}
	if( yMin >= yMax)
	{
		return;
	}
	float delta = yMax - yMin;
	float med = (yMin + yMax) / 2;
	for(int i = 0; i < vertices.size(); ++i )
	{
		float y = vertices[i].y;
		float t1 = ( y - yMin)/ delta;
		Rgb color = interpolate( t1, Rgb(1,0,0), Rgb(0,0,1));
		float t2;
		if( y < med)
		{
			t2 = (y - yMin) * 2 / delta;
			
		}
		else
		{
			t2 = (yMax - y) * 2 / delta;
		}
		color += interpolate(t2, Rgb(0,0,0), Rgb(0,1,0));
		MaterialColor mc;
		mc.ambient = color;
		mc.diffuse = color;
		mc.emissive = color;
		mc.specular = color;
		_vertexColors.push_back(mc);
	}
}

vector<Vertex> MeshModel::Triangles()
{
	vector<Vertex> vertex_positions;
	for (vector<Face>::iterator it = _faces.begin(); it != _faces.end(); ++it){
		for (int i = 0; i < 3; i++){
			vertex_positions.push_back( _vertices[it->v[i] - 1] ); // Pushing only verticies (w/o normals/textures)
		}
	}
	return vertex_positions;
}

vector<vec4> MeshModel::VertexNormals()
{
	vector<vec4> normals;

	for (int i = 0; i < _faces.size(); i++)
	{
		for (int j = 0; j < 3; j++) // push face normal instead of vertex normal for every vertex
		{
			if(_faces[i].vn[j] <= 0) // normal not defined
			{
				normals.clear();
				break;
			}
			else
			{
				normals.push_back(_normals[_faces[i].vn[j] - 1]);
			}
		}
	}
	return normals;
}

vector<vec4> MeshModel::FaceNormals()
{
	vector<vec4> normals;

	for (int i = 0; i < _faceNormals.size(); i++)
	{
		for(int j = 0; j < 3; j++) // push face normal instead of vertex normal for every vertex
		{
			normals.push_back(_faceNormals[i]);
		}
	}
	return normals;
}

vector<vec2> MeshModel::Textures()
{
	vector<vec2> textures;
	for (int i = 0; i < _faces.size(); i++)
	{
		for (int j = 0; j < 3; j++) // push face normal instead of vertex normal for every vertex
		{
			if(_faces[i].vt[j] <= 0) // normal not defined
			{
				return vector<vec2>();
			}
			else
			{
				textures.push_back(_textures[_faces[i].vt[j] - 1]);
			}
		}
	}
	return textures;
}

mat4 MeshModel::Transformation()
{
	return _world_transform * _inner_transform;
}

mat4 MeshModel::NormalTransformation()
{
	return _normal_transform * _inner_transform;
}

bool MeshModel::GetDrawTexture()
{
	return _drawTexture;
}
bool MeshModel::GetDrawEnvMap()
{
	return _envMap;
}

void MeshModel::SetDrawEnvMap(bool arg)
{
	_envMap = arg;
}

int MeshModel::FaceCount()
{
	return _faces.size();
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

// static helper functions
static vec3 vec3fFromStream(std::istream & aStream)
{
	float x, y, z;
	aStream >> x >> std::ws >> y >> std::ws >> z;
	return vec3(x, y, z);
}

static vec2 vec2fFromStream(std::istream & aStream)
{
	float x, y;
	aStream >> x >> std::ws >> y;
	return vec2(x, y);
}

static Rgb interpolate(float t, Rgb a, Rgb b)
{
	return Rgb(  a.r + t * (b.r - a.r),  a.g + t * (b.g - a.g), a.b + t * (b.b - a.b) );
}