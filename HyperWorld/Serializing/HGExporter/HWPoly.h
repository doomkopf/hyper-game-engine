#ifndef HWPOLY_H_
#define HWPOLY_H_

#include "../../../stuff/Mathtypes.h"
#include "../../../stuff/List.h"
#include "../../../HGEngine/Level0/RenderDevice.h"

class CHWVertex
{
public:
	CVector3f p;
	texcoord_t texcoord;
};

class CClipBrush;

class CHWPoly
{
public:
	CHWPoly();
	void CalcPlane();
	bool CheckIfConvex() const;
	void CalcEdgePlane(CNode<CHWVertex> *, CPlane *) const;
	CNode<CHWVertex> *GetNextVertex(CNode<CHWVertex> *v_node) const;
	CNode<CHWVertex> *GetPrevVertex(CNode<CHWVertex> *v_node) const;

	CList<CHWVertex> vertices;
	int tex;
	int new_tex; // for saving - not all textures are used
	CPlane plane;
	bool issplitter;
	CClipBrush *brush;
};

#endif