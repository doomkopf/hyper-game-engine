#ifndef ANIMATEDMESH_H_
#define ANIMATEDMESH_H_

#include "../Level0/Renderable.h"

class CAnimatedMesh : public IRenderable
{
public:
	virtual bool Load(const char *) = 0;
	virtual void SetState(void *) = 0;

	char name[64];
};

#endif