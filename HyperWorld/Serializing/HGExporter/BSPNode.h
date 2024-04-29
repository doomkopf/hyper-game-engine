#ifndef BSPNODE_H_
#define BSPNODE_H_

#include "HWPoly.h"

class CBSPNode
{
public:
	CBSPNode();
	~CBSPNode();
	void Destruct();
	void CalcAABB();

	bool isleaf;
	CVector3f bbmin, bbmax;

	// node relevant stuff
	CPlane splitter;
	CBSPNode *front, *back;

	// leaf relevant stuff
	CList<CHWPoly> *polys;
	CList<CClipBrush*> brushes;
};

#endif