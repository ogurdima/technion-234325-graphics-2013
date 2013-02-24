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

typedef enum {SPHERICAL, USER_GIVEN, FLATTENED} TexCoordSource_t;
typedef enum {NONE, HUE, LERP} ColorAnim_t;


class MeshModel : public Model
{
public:
							MeshModel();
							MeshModel(string fileName);
							MeshModel(const MeshModel& rhs);
							~MeshModel(void);
	void					LoadFile(string fileName);
	// Transformations
	void					WFRotate(mat4 m);
	void					WFScale(mat4 m);
	void					WFTranslate(mat4 m);
	void					MFRotate(mat4 m);
	void					MFTranslate(mat4 m);
	void					MFScale(mat4 m);
	virtual vec3			Origin();
	vec3					BoundingBoxCenter();
	// Drawing options
	bool					ToggleShowFaceNormals();
	bool					ToggleShowBoundingBox();
	bool					ToggleShowVertexNormals();
	bool					ToggleShowModelFrame();
	// Color manipulations
	void					SetDefaultColor(MaterialColor _c);
	MaterialColor			GetDefaultColor();
	void					SetRandomColor();
	void					SetProgressiveColor();
	
	vector<Vertex>			Triangles();
	vector<vec2>			TextureCoords();
	vector<vec2>			SphereTextures();
	vector<vec4>			FaceNormals();
	vector<vec4>			VertexNormals();
	vector<vec4>			AverageVertexNormals();
	vector<vec3>			Randoms();
	void					TangentBitangent(vector<vec3>& outTangent, vector<vec3>& outBitangent);
	int						FaceCount();
	

	void					SetTextureCoordinatesSource(TexCoordSource_t _s);
	bool					SetDrawTexture(bool val);
	bool					GetDrawTexture();
	void					SetNormalMap(bool);
	bool					GetNormalMap();
	bool					GetDrawEnvMap();
	void					SetDrawEnvMap(bool);

	ModelBind				_oglBind;

	mat4					Transformation();
	mat4					NormalTransformation();

	void					SetVertexAnimation(bool val);
	bool					GetVertexAnimation();
	void					SetColorAnimation(ColorAnim_t val);
	ColorAnim_t				GetColorAnimation();
	//void					ChangeColorAnimationParam(float factor);
	//float					GetColorAnimationParam();

	void					Animation();
	void					VertexAnimation();
	void					ColorAnimation();
	
protected :
	vector<Face>			_faces;
	vector<Vertex>			_vertices;
	vector<vec4>			_normals;
	vector<vec4>			_faceNormals;
	vector<vec2>			_textures;
	vector<MaterialColor>	_vertexColors;
	MaterialColor			_defaultColor;

	vector<int>*			_vertexPositionIdxs;
	vector<vector<int> >*	_vNormalSets;

	mat4					_world_transform;
	mat4					_normal_transform;
	mat4					_inner_transform;

	bool					_drawVN;
	bool					_drawFN;
	bool					_drawBB;
	bool					_drawMF;

	bool					_vertexAnimation;
	ColorAnim_t				_colorAnimation;

	float					_colorAnimationSharedCoeff;
	Rgb						_colorAnimationLerpRandom;
	bool					_colorAnimationLerpSubtract;

	bool					_drawTexture;
	bool					_envMap;
	bool					_normalMap;
	TexCoordSource_t		_texCoordSource;

	void					CalculateFaceNormals();
	void					CalculateIdxs();
	
};
