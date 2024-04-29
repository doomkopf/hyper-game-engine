#ifndef HWTERRAINPATCH_H_
#define HWTERRAINPATCH_H_

#include "../stuff/Mathtypes.h"

#define MAX(a, b) ((a < b) ? b : a)

class CTriNode
{
public:
	CTriNode();
	~CTriNode();
	void Construct();
	void Destruct();
	void CalcNormal(
		short *,
		const CVector2d &,
		const CVector2d &,
		const CVector2d &,
		int);

	CTriNode *left, *right, *nb_left, *nb_right, *nb_base;
	CVector3f normal;
	unsigned char light;
};

class CVarianceNode
{
public:
	short v;
	int indices[6];
};

class CVarianceTree
{
public:
	CVarianceTree(int);
	~CVarianceTree();
	void Reset();
	void AddVariance(int, int, int, int, int, int, short);
	short GetVariance(int, int, int, int, int, int);

	int max_v_depth;
	CVarianceNode *v;
	int idx;
};

class CHMNormal;

class CHWTerrainPatch
{
public:
	CHWTerrainPatch();
	~CHWTerrainPatch();
	void Destruct();
	void Init(
		short *,
		CHMNormal *,
		CHWTerrainPatch *,
		CHWTerrainPatch *,
		CHWTerrainPatch *,
		CHWTerrainPatch *,
		int,
		int,
		int,
		int,
		int,
		int);
	void CreateVarianceTree();
	void Reset();
	void Tessellate(const CVector3f &, int, short);
	void CalcNormals();
	void CalcFlatShade();
	void FindVertexNormals();
	void CalcGouraudShade();

	CTriNode *n1, *n2;
	CHWTerrainPatch *ln, *rn, *tn, *bn;
	short *hm;
	CHMNormal *hm_normal;
	int hm_per_side;
	CVector2d min, max;
	CVarianceTree *v1, *v2;
	CVarianceTree *cv;

protected:
	short RCreateVarianceTree(
		const CVector2d &, short,
		const CVector2d &, short,
		const CVector2d &, short);

	void RTessellate(CTriNode *,
		const CVector2d &,
		const CVector2d &,
		const CVector2d &,
		const CVector3f &,
		int, short);

	void RCalcNormals(CTriNode *,
		const CVector2d &,
		const CVector2d &,
		const CVector2d &);

	void RCalcFlatShade(CTriNode *);

	void RFindNormals(CTriNode *,
		const CVector2d &,
		const CVector2d &,
		const CVector2d &);
	void RCalcGouraudShade(CTriNode *,
		const CVector2d &,
		const CVector2d &,
		const CVector2d &);

	void Split(CTriNode *);
};

#endif