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

	void			LoadFile(string fileName);
	void			BindToRenderer(Renderer* r);
	void virtual	Draw(Renderer * r);

	// Transformations
	void			WFRotate(mat4 m);
	void			WFScale(mat4 m);
	void			WFTranslate(mat4 m);
	void			MFRotate(mat4 m);
	void			MFTranslate(mat4 m);
	void			MFScale(mat4 m);
	virtual vec3	Origin();
	// Drawing options
	bool			ToggleShowFaceNormals();
	bool			ToggleShowBoundingBox();
	bool			ToggleShowVertexNormals();
	bool			ToggleShowModelFrame();
	// Color manipulations
	void			SetDefaultColor(MaterialColor _c);
	MaterialColor	GetDefaultColor();
	void			SetRandomColor();
	void			SetProgressiveColor();

protected :
	vector<Face>			_faces;
	vector<Vertex>			_vertices;
	vector<vec4>			_normals;
	vector<vec4>			_faceNormals;
	vector<vec2>			_textures;
	vector<MaterialColor>	_vertexColors;
	MaterialColor			_defaultColor;

	mat4					_world_transform;
	mat4					_normal_transform;
	mat4					_inner_transform;

	bool					_drawVN;
	bool					_drawFN;
	bool					_drawBB;
	bool					_drawMF;

	ModelBind				_oglBind;

	void					CalculateFaceNormals();
	vector<Vertex>			Triangles();
	vector<vec4>			Normals(ShadingType st);
};
