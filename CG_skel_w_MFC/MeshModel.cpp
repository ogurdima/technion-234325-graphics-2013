#include "StdAfx.h"
#include "MeshModel.h"
#include "vec.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
using namespace std;
//#pragma region
static vec3 vec3fFromStream(std::istream & aStream);
static vec2 vec2fFromStream(std::istream & aStream);
static Rgb interpolate(float t, Rgb a, Rgb b);
static vec2 unitSphereAngles(vec3 center, vec3 pt);
static vec2 unitSphereAngles(vec3 pt);
//#pragma endregion

//#pragma region Custruction destruction
MeshModel::MeshModel() :
_drawBB(false),
_drawVN(false),
_drawFN(false),
_drawMF(false),
_drawTexture(false),
_envMap(false),
_texCoordSource(USER_GIVEN),
_normalMap(false),
_vertexAnimation(false)
{
	_world_transform = Identity4();
	_normal_transform = Identity4();
	_inner_transform = Identity4();
	_vertexPositionIdxs = new vector<int>;
	_vNormalSets = new vector<vector<int> >;
}

MeshModel::MeshModel(string fileName) :
_drawBB(false),
_drawVN(false),
_drawFN(false),
_drawMF(false),
_drawTexture(false),
_envMap(false),
_texCoordSource(USER_GIVEN),
_normalMap(false),
_vertexAnimation(false)
{
	_vertexPositionIdxs = new vector<int>;
	_vNormalSets = new vector<vector<int> >;
	LoadFile(fileName);
}

MeshModel::MeshModel(const MeshModel& rhs) 
{
	_vertexPositionIdxs = new vector<int>( *rhs._vertexPositionIdxs);
	_vNormalSets = new vector<vector<int> >(*rhs._vNormalSets);
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
	_texCoordSource = rhs._texCoordSource;
	_normalMap = rhs._normalMap;
	_vertexAnimation = rhs._vertexAnimation;
}

MeshModel::~MeshModel(void)
{
	delete _vertexPositionIdxs;
	delete _vNormalSets;
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
	CalculateIdxs();
}
//#pragma endregion

//#pragma region Transformations
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
//#pragma endregion

//#pragma region Drawing options
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
//#pragma endregion

//#pragma region Color manipulations

MaterialColor MeshModel::GetDefaultColor()
{
	MaterialColor mc = _defaultColor;
	if( NONE == _colorAnimation )
	{
		return mc;
	}
	if ( HUE == _colorAnimation ) 
	{
		vec3 hsl = mc.diffuse.toHSL();
		hsl.x = _colorAnimationSharedCoeff;
		mc.diffuse.setHSL(hsl);
		return mc;
	}
	if ( LERP == _colorAnimation ) 
	{
		mc.diffuse = interpolate(_colorAnimationSharedCoeff, mc.diffuse, _colorAnimationLerpRandom);
		return mc;
	}
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
//#pragma endregion

//#pragma region Arrays
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

vector<vec4> MeshModel::AverageVertexNormals()
{
	vector<vec4> normals;
	for(int i = 0; i < _vertexPositionIdxs->size(); ++i)
	{
		int vIdx = (*_vertexPositionIdxs)[i];
		vector<int>& normalIdxs = (*_vNormalSets)[vIdx];
		if(0 == normalIdxs.size())
		{
			normals.push_back(normalize(_faceNormals[i/3]));
		}
		else
		{
			vec4 avgNormal(0,0,0,0);
			for( int j = 0; j < normalIdxs.size(); j++)
			{
				avgNormal += normalize(_normals[ normalIdxs[j]]);
			}
			normals.push_back(normalize(avgNormal));
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

vector<vec2> MeshModel::TextureCoords()
{
	if (_texCoordSource == SPHERICAL) {
		return SphereTextures();
	}

	vector<vec2> textures;
	for (int i = 0; i < _faces.size(); i++)
	{
		for (int j = 0; j < 3; j++) // push face normal instead of vertex normal for every vertex
		{
			if(_faces[i].vt[j] <= 0) // normal not defined
			{
				return SphereTextures();
			}
			else
			{
				textures.push_back(_textures[_faces[i].vt[j] - 1]);
			}
		}
	}
	return textures;
}

void MeshModel::TangentBitangent(vector<vec3>& outTangent, vector<vec3>& outBitangent)
{
	vector<vec2> textureCoords = TextureCoords();
	vector<vec4> vertices = Triangles();
	if(0 == textureCoords.size() || textureCoords.size() < vertices.size())
	{
		outTangent = vector<vec3>();
		outBitangent = vector<vec3>();
		return;
	}

	for( int i = 0; i < vertices.size(); i+=3)
	{
		// Shortcuts for vertices
		vec4& v0 = vertices[i+0];
		vec4& v1 = vertices[i+1];
		vec4& v2 = vertices[i+2];
 
		// Shortcuts for UVs
		vec2& uv0 = textureCoords[i+0];
		vec2& uv1 = textureCoords[i+1];
		vec2& uv2 = textureCoords[i+2];
 
		// Edges of the triangle : postion delta
		vec3 deltaPos1 = vec3(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
		vec3 deltaPos2 = vec3(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z);
 
		// UV delta
		vec2 deltaUV1 = uv1-uv0;
		vec2 deltaUV2 = uv2-uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		vec3 tangent = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r;
		vec3 bitangent = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r;


		outTangent.push_back( tangent);
		outTangent.push_back( tangent);
		outTangent.push_back( tangent);
		outBitangent.push_back(bitangent);
		outBitangent.push_back(bitangent);
		outBitangent.push_back(bitangent);
	}
}

vector<vec2> MeshModel::SphereTextures()
{
	vector<vec2> textures;
	vec3 center = BoundingBoxCenter();
	for (int i = 0; i < _faces.size(); i++)
	{
		for (int j = 0; j < 3; j++) // push face normal instead of vertex normal for every vertex
		{
			Vertex v = _vertices[_faces[i].v[j] - 1];
			vec2 angles = unitSphereAngles(center, vec3(v.x, v.y, v.z));
			vec2 uv = vec2( angles.x / (2 * M_PI), angles.y / M_PI );
			textures.push_back(uv);
		}
	}
	return textures;
}

vector<vec3> MeshModel::Randoms()
{
	vector<vec3> rands;
	rands.resize(_vertices.size());
	for(int i = 0; i < _vertices.size(); ++i)
	{
		rands[i] = normalize( normalize(vec3( rand(), rand(), rand())) * 2 - vec3(1,1,1) );
	}
	vector<vec3> outRands;
	outRands.resize(_vertexPositionIdxs->size());
	for(int i = 0; i < (*_vertexPositionIdxs).size(); ++i)
	{
		outRands[i] = rands[(*_vertexPositionIdxs)[i]];
	}
	return outRands;
}

vector<float> MeshModel::RandomFloatPerVertex()
{
	vector<float> rands;
	rands.resize(_vertices.size());
	for(int i = 0; i < _vertices.size(); ++i)
	{
		rands[i] = ( ((float)(rand() % 10000)) / (10000 + 1) ) * 2 - 1;
	}
	vector<float> outRands;
	outRands.resize(_vertexPositionIdxs->size());
	for(int i = 0; i < (*_vertexPositionIdxs).size(); ++i)
	{
		outRands[i] = rands[(*_vertexPositionIdxs)[i]];
	}
	return outRands;
}

//#pragma endregion

//#pragma region Transformations
mat4 MeshModel::Transformation()
{
	return _world_transform * _inner_transform;
}

mat4 MeshModel::NormalTransformation()
{
	return _normal_transform * _inner_transform;
}
//#pragma endregion

//#pragma region Mapping
bool MeshModel::SetDrawTexture(bool val)
{
	bool oldval = _drawTexture;
	_drawTexture = val;
	return oldval;
}

bool MeshModel::GetDrawTexture()
{ return _drawTexture; }

void MeshModel::SetTextureCoordinatesSource(TexCoordSource_t _s)
{
	_texCoordSource = _s;
	// send ne textures (Textures()) to the renderer, so it could rebind the buffer.
}

bool MeshModel::GetDrawEnvMap()
{
	return _envMap;
}

void MeshModel::SetDrawEnvMap(bool arg)
{
	_envMap = arg;
}

void MeshModel::SetNormalMap(bool val)
{
	_normalMap = val;
}

bool MeshModel::GetNormalMap()
{
	return _normalMap;
}
//#pragma endregion

int MeshModel::FaceCount()
{
	return _faces.size();
}

vec3 MeshModel::BoundingBoxCenter()
{
	float maxX = 0, minX = 0, maxY = 0, minY = 0, maxZ = 0, minZ = 0;
	if (_vertices.size() < 1) {
		return vec3(0,0,0);
	}
	Vertex v = _vertices[0];
	maxX = minX = v.x;
	maxY = minY = v.y;
	maxZ = minZ = v.z;

	for(int i = 1; i < _vertices.size(); i++) {
		Vertex& v = _vertices[i];
		if (v.x < minX) {
			minX = v.x;
		}
		if (v.x > maxX) {
			maxX = v.x;
		}
		if (v.y < minY) {
			minY = v.y;
		}
		if (v.y > maxY) {
			maxY = v.y;
		}
		if (v.z < minZ) {
			minZ = v.z;
		}
		if (v.z > maxZ) {
			maxZ = v.z;
		}
	}
	float x = (maxX + minX) / 2;
	float y = (maxY + minY) / 2;
	float z = (maxY + minY) / 2;
	return vec3(x,y,z);
}

//#pragma region	Animation
void MeshModel::SetVertexAnimation(bool val)
{
	_vertexAnimation = val;
	_vertexAnimationCoeff = 0;
	_vertexAnimationSubtract = false;
}

bool MeshModel::GetVertexAnimation()
{
	return _vertexAnimation;
}

float MeshModel::GetVertexAnimationParam()
{
	return _vertexAnimationCoeff;
}

void MeshModel::Animation()
{
	VertexAnimation();
	ColorAnimation();
}

void MeshModel::VertexAnimation()
{
	if (_vertexAnimationSubtract) {
		_vertexAnimationCoeff -= 0.05;
		if (_vertexAnimationCoeff <= 0) {
			_vertexAnimationCoeff = 0;
			_vertexAnimationSubtract = false;
		}
	}
	else {
		_vertexAnimationCoeff += 0.05;
		if (_vertexAnimationCoeff >= 1) {
			_vertexAnimationCoeff = 1;
			_vertexAnimationSubtract = true;
		}
	}
	return;
}

void MeshModel::ColorAnimation()
{
	if( NONE == _colorAnimation )
	{
		return;
	}
	if ( HUE == _colorAnimation ) 
	{
		_colorAnimationSharedCoeff = fmodf ( ( _colorAnimationSharedCoeff + 0.02 ), 1.0);
		return;
	}
	if ( LERP == _colorAnimation ) 
	{
		if (_colorAnimationLerpSubtract) {
			_colorAnimationSharedCoeff -= 0.02;
			if (_colorAnimationSharedCoeff <= 0) {
				_colorAnimationSharedCoeff = 0;
				_colorAnimationLerpSubtract = false;
			}
		}
		else {
			_colorAnimationSharedCoeff += 0.02;
			if (_colorAnimationSharedCoeff >= 1) {
				_colorAnimationSharedCoeff = 1;
				_colorAnimationLerpSubtract = true;
			}
		}
		return;
	}
}

void MeshModel::SetColorAnimation(ColorAnim_t val)
{
	_colorAnimationLerpRandom = Rgb(0,0,0);
	_colorAnimationLerpSubtract = false;
	_colorAnimationSharedCoeff = 0;
	if (LERP == val) {
		float randR = ( (float)(rand() % 10000) ) / (10000 - 1);
		float randG = ( (float)(rand() % 10000) ) / (10000 - 1);
		float randB = ( (float)(rand() % 10000) ) / (10000 - 1);
		_colorAnimationLerpRandom = Rgb(randR, randG, randB);
	}
	_colorAnimation = val;
}

ColorAnim_t MeshModel::GetColorAnimation()
{
	return _colorAnimation;
}

/*
void MeshModel::ChangeColorAnimationParam(float factor)
{
	_colorAnimationParam = fmodf ( ( _colorAnimationParam + factor ), 1.0);
}

float MeshModel::GetColorAnimationParam()
{
	return _colorAnimationParam;
}

*/

//#pragma endregion

//#pragma region Precalculations
void MeshModel::CalculateIdxs()
{
	vector<vector<int> >* _vTextureIdxsSet = new vector<vector<int> >(_vertices.size());
	_vNormalSets->resize(_vertices.size());
	for (int i=0; i < _faces.size(); ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			int idx = _faces[i].v[j] - 1;
			_vertexPositionIdxs->push_back( idx );

			if(_faces[i].vn[j] > 0) 
			{
				int normalIdx = _faces[i].vn[j] - 1;
				vector<int>& normalSet = (*_vNormalSets)[idx];
				bool found = false;
				for( int k = 0; k < normalSet.size(); ++k)
				{
					if( normalSet[k] == normalIdx)
					{
						found = true;
					}
				}
				if(!found)
				{
					normalSet.push_back(normalIdx);
				}
			}

			if(_faces[i].vt[j] > 0) 
			{
				int textureIdx = _faces[i].vt[j] - 1;
				vector<int>& texIdxSet = (*_vTextureIdxsSet)[idx];
				bool found = false;
				for( int k = 0; k < texIdxSet.size(); ++k)
				{
					if( texIdxSet[k] == textureIdx)
					{
						found = true;
					}
				}
				if(!found)
				{
					texIdxSet.push_back(textureIdx);
				}
			}
		}
	}
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
//#pragma endregion

//#pragma region helper functions

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

static vec2 unitSphereAngles(vec3 center, vec3 pt)
{
	return unitSphereAngles(pt - center);
}

static vec2 unitSphereAngles(vec3 pt) 
{
	vec3 z = vec3(0,0,1);
	pt = normalize(pt);
	float cost = dot(pt, z);
	if (cost > 1) cost = 1;
	if (cost < -1) cost = -1;
	float t = acosf(cost);
	float f = atan2f(pt.y, pt.x);
	if (f < 0) {
		f += 2 * M_PI;
	}
	// 0 < t < pi; 0 < f < 2pi
	return vec2(f, t);
}
//#pragma endregion