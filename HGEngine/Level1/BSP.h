#ifndef BSP_H_
#define BSP_H_

#include "Poly.h"

class CBSPNode
{
public:
	CBSPNode();
	~CBSPNode();
	void Destruct();
	void SetParents(CBSPNode *);

	bool isleaf;
	CVector3f bbmin, bbmax;

	// node relevant stuff
	CPlane splitter;
	CBSPNode *front, *back, *parent;

	// leaf relevant stuff
	int num_p;
	CPoly *poly;
	int num_brushes;
	int *brush_idx;
};

typedef CBSPNode *CBSPNodeP;


class CBSP
{
public:
	CBSP();
	~CBSP();
	void Init();
	void UpdateParents();
	void Destruct();
	//int GetPolys(CPoly ***);
	//int GetPolyCount();

	CBSPNode *root_node;

protected:
	//void GP(CBSPNode *, CPoly **, int *);
	//void GPC(CBSPNode *, int *);
};

#endif