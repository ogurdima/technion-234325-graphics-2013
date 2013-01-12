#include "StdAfx.h"
#include "PrimMeshModel.h"


PrimMeshModel::PrimMeshModel(void)
{
	_vertices.push_back(Vertex(0,0,2,1));
	_vertices.push_back(Vertex(0,0,-1,1));
	_vertices.push_back(Vertex(1,-1,0,1));
	_vertices.push_back(Vertex(-1,-1,0,1));

	_normals.push_back(vec4(0,1,0,0));
	_normals.push_back(normalize(vec4(1,1,0,0)));
	_normals.push_back(normalize(vec4(-1,1,0,0)));
	
	Face fb;
	fb.v[0] = 1;
	fb.v[1] = 2;
	fb.v[2] = 3;
	fb.vn[0] = 1;
	fb.vn[1] = 1;
	fb.vn[2] = 2;
	_faces.push_back(fb);

	fb.v[0] = 1;
	fb.v[1] = 4;
	fb.v[2] = 2;
	fb.vn[0] = 1;
	fb.vn[1] = 3;
	fb.vn[2] = 1;
	_faces.push_back(fb);

	

	CalculateFaceNormals();
}


PrimMeshModel::~PrimMeshModel(void)
{
}
