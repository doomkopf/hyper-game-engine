#ifndef BSP_H_
#define BSP_H_

#include "Poly.h"
#include "func.h"

class CBSPNode
{
public:
	CBSPNode();
	CBSPNode(const CBSPNode &);
	~CBSPNode();
	void Destruct();
	void Copy(const CBSPNode &);

	bool isleaf;

	// node relevant stuff
	CPlane splitter;
	CBSPNode *front, *back;

	// leaf relevant stuff
	CList<CPoly> polys;
};

typedef CBSPNode *CBSPNodeP;

class CBSP
{
public:
	CBSP();
	CBSP(const CBSP &);
	~CBSP();
	void Destruct();
	void Copy(const CBSP &);
	CBSP &operator=(const CBSP &);
	void BSP();
	void BSP(CBSPNode *);
	void BSP(CBSPNode *, CNode<CPlane> *);

	CBSPNode *root_node;

protected:
	CPoly *FindBestSplitter(CBSPNode *);
	bool MakeSolidLeaf(CList<CPoly> *, CBSPNode *);
	void Bevel(CPoly *, CBSPNode *, CList<CPlane> *);
};

#endif