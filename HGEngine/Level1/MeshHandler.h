#ifndef MESHHANDLER_H_
#define MESHHANDLER_H_

#include "StaticMesh.h"
#include "AnimatedMesh.h"
#include "../../stuff/List.h"

class CMeshHandler
{
public:
	void AddMesh(CStaticMesh *);
	void AddMesh(CAnimatedMesh *);
	CStaticMesh *GetStaticMesh(const char *);
	CAnimatedMesh *GetAnimatedMesh(const char *);

	CList<CStaticMesh> s_meshes;
	CList<CAnimatedMesh> a_meshes;
};

#endif