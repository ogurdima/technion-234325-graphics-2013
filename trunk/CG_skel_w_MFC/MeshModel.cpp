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
_drawMF(false)
{
	_world_transform = Identity4();
	_normal_transform = Identity4();
}

MeshModel::MeshModel(string fileName) :
_drawBB(false),
_drawVN(false),
_drawFN(false),
_drawMF(false)
{
	loadFile(fileName);
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

void MeshModel::BindToRenderer(Renderer* r, ShadingType st)
{
	oglBind = r->BindModel(triangles(), normals(st), _defaultColor);
}

// Drawing function
void MeshModel::draw(Renderer * r)
{
	cout << "MeshModel::draw" << endl;
	r->SetUniformMatrix(oglBind.pointMat, _world_transform);
	r->SetUniformMatrix(oglBind.normMat, _normal_transform);
	r->DrawTriangles(oglBind.vao, _faces.size() * 3);
}


// Transformations
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

vec3 MeshModel::origin()
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

// Color manipulations
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
	vector<Vertex> vertices = transformVertices();
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

// private functions
vector<Vertex> MeshModel::transformVertices(){
	vector<Vertex> vertex_positions;
	for (vector<Face>::iterator it = _faces.begin(); it != _faces.end(); ++it){
		for (int i = 0; i < 3; i++){
			vertex_positions.push_back( _world_transform * _vertices[it->v[i] - 1] ); // Pushing only verticies (w/o normals/textures)
		}
	}
	return vertex_positions;
}

vector<Vertex> MeshModel::triangles()
{
	vector<Vertex> vertex_positions;
	for (vector<Face>::iterator it = _faces.begin(); it != _faces.end(); ++it){
		for (int i = 0; i < 3; i++){
			vertex_positions.push_back( _vertices[it->v[i] - 1] ); // Pushing only verticies (w/o normals/textures)
		}
	}
	return vertex_positions;
}

vector<vec4> MeshModel::normals(ShadingType st)
{
	vector<vec4> normals;
	if (st != FLAT)
	{
		normals = _normals;
	}
	else
	{
		for (int i = 0; i < _faceNormals.size(); i++)
		{
			for(int j = 0; j < 3; j++) // push face normal instead of vertex normal for every vertex
			{
				normals.push_back(_faceNormals[i]);
			}
		}
	}
	return normals;
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