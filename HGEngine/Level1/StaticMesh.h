#ifndef STATICMESH_H_
#define STATICMESH_H_

#include "../../stuff/Math.h"
#include "../../stuff/FileBuf.h"
#include <string.h>

class CMeshVertex
{
public:
	CVector3f p;//, normal;
};

class CMeshFace
{
public:
	int v[3]; // vertex indices
	//CPlane plane;
};

class CStaticMesh
{
public:
	CStaticMesh();
	~CStaticMesh();
	void Destruct();
	bool LoadSMF(const char *);

	void CreateSphere(const CVector3f &, float, int, int);
	void CreateCylinder(const CVector3f &, const CVector3f &, float, int, int, bool closed = false);
	void CreateBlock(const CVector3f &, const CVector3f &, const CVector3f &);

	char name[64];

	CMeshVertex *vertex;
	int num_vertices;

	CMeshFace *face;
	int num_faces;
};

#endif