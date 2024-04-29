#ifndef POLY_H_
#define POLY_H_

#include "../../stuff/Math.h"
#include "../Level0/RenderDevice.h"

#ifndef NULL
#define NULL 0
#endif

class CVertex
{
public:
	CVector3f p;
	texcoord_t tc;
};

class CPoly
{
public:
	CPoly();
	~CPoly();
	void Destruct();

	int num_v;
	CVertex *vertex;
	CPlane plane;
	int tex;
};

#endif