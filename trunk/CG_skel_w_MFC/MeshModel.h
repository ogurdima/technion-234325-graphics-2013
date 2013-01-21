#pragma once
#include <string>
#include "vec.h"
#include "mat.h"
#include "Vertex.h"
#include "Face.h"
#include "Model.h"
#include "Renderer.h"
#include "MaterialColor.h"

using namespace std;


class MeshModel : public Model
{
	
public:
	MeshModel();
	MeshModel(string fileName);
	MeshModel(const MeshModel& rhs);
	~MeshModel(void);
	void loadFile(string fileName);
	void BindToRenderer(Renderer* r, ShadingType st);
	// Drawing function
	void virtual draw(Renderer * r);
	// Transformations
	void Rotate(mat4 m);
	void Scale(mat4 m);
	void Translate(mat4 m);
	virtual vec3 origin();
	// Drawing options
	bool ToggleShowFaceNormals();
	bool ToggleShowBoundingBox();
	bool ToggleShowVertexNormals();
	bool ToggleShowModelFrame();
	// Color manipulations
	void SetDefaultColor(MaterialColor _c);
	MaterialColor GetDefaultColor();
	void SetRandomColor();
	void SetProgressiveColor();

protected :
	vector<Face> _faces;
	vector<Vertex> _vertices;
	vector<vec4> _normals;
	vector<vec4> _faceNormals;
	vector<vec2> _textures;
	vector<MaterialColor> _vertexColors;
	MaterialColor _defaultColor;
	mat4 _world_transform;
	mat4 _normal_transform;
	bool _drawVN;
	bool _drawFN;
	bool _drawBB;
	bool _drawMF;

	ModelBind oglBind;

	void CalculateFaceNormals();
	vector<Vertex> triangles();
	vector<vec4> normals(ShadingType st);
};
