#include "StdAfx.h"
#include "PrimMeshModel.h"


PrimMeshModel::PrimMeshModel(void)
{
	_vertices.push_back(Vertex(0,1,0,1));
	_vertices.push_back(Vertex(0,-1,0,1));
	_vertices.push_back(Vertex(1,0,0,1));
	_vertices.push_back(Vertex(0.5,0,1,1));
	
	Face fb;
	fb.v[0] = 1;
	fb.v[1] = 3;
	fb.v[2] = 2;
	_faces.push_back(fb);

	
	/*fb.v[0] = 2;
	fb.v[1] = 3;
	fb.v[2] = 4;
	_faces.push_back(fb);

	
	fb.v[0] = 1;
	fb.v[1] = 2;
	fb.v[2] = 4;
	_faces.push_back(fb);

	
	fb.v[0] = 1;
	fb.v[1] = 4;
	fb.v[2] = 3;
	_faces.push_back(fb);*/

	CalculateFaceNormals();
}


PrimMeshModel::~PrimMeshModel(void)
{
}
