#pragma once
#include <string>
#include "vec.h"
#include "mat.h"
#include "Vertex.h"
#include "Face.h"
#include "Model.h"
#include "MaterialColor.h"
#include "Binds.h"

using namespace std;


class MeshModel : public Model
{
public:
					MeshModel();
					MeshModel(string fileName);
					MeshModel(const MeshModel& rhs);
					~MeshModel(void);

	void			LoadFile(string fileName);
//	void			SetTexture(vector<byte>& t, Renderer* r, unsigned int w, unsigned int h);
	//void			BindToRenderer(Renderer* r);
	//void			Unbind(Renderer* r);
	//void			QuickRebind(Renderer* r);
	//void virtual	Draw(Renderer * r);

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
	bool			SetDrawTexture(bool val);
	// Color manipulations
	void			SetDefaultColor(MaterialColor _c);
	MaterialColor	GetDefaultColor();
	void			SetRandomColor();
	void			SetProgressiveColor();


	vector<Vertex>			Triangles();
	vector<vec2>			Textures();
	vector<vec4>			FaceNormals();
	vector<vec4>			VertexNormals();
	mat4					Transformation();
	mat4					NormalTransformation();

	int						FaceCount();

	bool					GetDrawTexture();
	bool					GetDrawEnvMap();
	void					SetDrawEnvMap(bool);

	ModelBind				_oglBind;

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
	bool					_drawTexture;
	bool					_envMap;

	void					CalculateFaceNormals();
	
};
