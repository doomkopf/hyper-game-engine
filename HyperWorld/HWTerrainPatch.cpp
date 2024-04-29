#include "HWTerrainPatch.h"
#include "HWTerrain.h"
#include "../stuff/Math.h"
#include <stdio.h>

CTriNode::CTriNode()
{
	Construct();
}

void CTriNode::Construct()
{
	left = NULL;
	right = NULL;
	nb_left = NULL;
	nb_right = NULL;
	nb_base = NULL;
}

void CTriNode::CalcNormal(short *hm,
						  const CVector2d &left,
						  const CVector2d &right,
						  const CVector2d &apex,
						  int hm_per_side)
{
	normal.NormalFromPoints(
		CVector3f((float)left.x, (float)hm[(left.y * hm_per_side) + left.x], (float)left.y),
		CVector3f((float)apex.x, (float)hm[(apex.y * hm_per_side) + apex.x], (float)apex.y),
		CVector3f((float)right.x, (float)hm[(right.y * hm_per_side) + right.x], (float)right.y));
}

CTriNode::~CTriNode()
{
	Destruct();
}

void CTriNode::Destruct()
{
	left = NULL;
	right = NULL;
	nb_left = NULL;
	nb_right = NULL;
	nb_base = NULL;
}

// ------------------------------------------------------------------------

CVarianceTree::CVarianceTree(int mvd)
{
	max_v_depth = mvd;
	v = new CVarianceNode[max_v_depth];
	Reset();
}

CVarianceTree::~CVarianceTree()
{
	delete [] v;
}

void CVarianceTree::Reset()
{
	for(idx = 0; idx < max_v_depth; idx++)
		v[idx].v = -1;
	idx = 0;
}

void CVarianceTree::AddVariance(int x1, int y1, int x2, int y2, int x3, int y3, short variance)
{
	if(idx >= max_v_depth)
	{
		static bool b = false;
		if(!b)
			MessageBox(NULL, "Variance Tree Overrun", NULL, MB_OK);
		b = true;
		return;
	}

	for(int i = 0; i < idx; i++)
	{
		if(v[i].indices[0] == x1 &&
			v[i].indices[1] == y1 &&
			v[i].indices[2] == x2 &&
			v[i].indices[3] == y2 &&
			v[i].indices[4] == x3 &&
			v[i].indices[5] == y3)
		{
			v[i].v = variance;
			return;
		}
	}

	v[idx].indices[0] = x1;
	v[idx].indices[1] = y1;
	v[idx].indices[2] = x2;
	v[idx].indices[3] = y2;
	v[idx].indices[4] = x3;
	v[idx].indices[5] = y3;
	v[idx].v = variance;
	idx++;
}

short CVarianceTree::GetVariance(int x1, int y1, int x2, int y2, int x3, int y3)
{
	for(int i = 0; i < idx; i++)
	{
		if(v[i].indices[0] == x1 &&
			v[i].indices[1] == y1 &&
			v[i].indices[2] == x2 &&
			v[i].indices[3] == y2 &&
			v[i].indices[4] == x3 &&
			v[i].indices[5] == y3)
			return v[i].v;
	}

	return -1;
}

// ------------------------------------------------------------------------

CHWTerrainPatch::CHWTerrainPatch()
{
	hm = NULL;
	n1 = NULL;
	n2 = NULL;
	v1 = NULL;
	v2 = NULL;
	hm_normal = NULL;
}

CHWTerrainPatch::~CHWTerrainPatch()
{
	Destruct();
}

void CHWTerrainPatch::Init(short *h,
						   CHMNormal *hn,
						   CHWTerrainPatch *l,
						   CHWTerrainPatch *r,
						   CHWTerrainPatch *t,
						   CHWTerrainPatch *b,
						   int hmps,
						   int mvd,
						   int xmin,
						   int ymin,
						   int xmax,
						   int ymax)
{
	v1 = new CVarianceTree(mvd);
	v2 = new CVarianceTree(mvd);
	hm = h;
	hm_normal = hn;
	hm_per_side = hmps;
	ln = l;
	rn = r;
	tn = t;
	bn = b;
	min.x = xmin;
	min.y = ymin;
	max.x = xmax;
	max.y = ymax;

	n1 = CHWTerrain::GetNodeFromPool();
	n2 = CHWTerrain::GetNodeFromPool();
}

void CHWTerrainPatch::CreateVarianceTree()
{
	n1->nb_base = n2;
	n1->nb_left = ln ? ln->n2 : NULL;
	n1->nb_right = tn ? tn->n2 : NULL;

	n2->nb_base = n1;
	n2->nb_left = rn ? rn->n1 : NULL;
	n2->nb_right = bn ? bn->n1 : NULL;

	v1->Reset();
	v2->Reset();

	cv = v1;
	RCreateVarianceTree(
		CVector2d(min.x, max.y), hm[(max.y * hm_per_side) + min.x],
		CVector2d(max.x, min.y), hm[(min.y * hm_per_side) + max.x],
		CVector2d(min.x, min.y), hm[(min.y * hm_per_side) + min.x]);

	cv = v2;
	RCreateVarianceTree(
		CVector2d(max.x, min.y), hm[(min.y * hm_per_side) + max.x],
		CVector2d(min.x, max.y), hm[(max.y * hm_per_side) + min.x],
		CVector2d(max.x, max.y), hm[(max.y * hm_per_side) + max.x]);

	/*FILE *f;
	f = fopen("test.txt", "w+");
	for(int i = 0; i < v1.idx; i++)
	{
		fprintf(f, "%d\n", v1.v[i]);
	}
	fclose(f);*/

	//v >>= 1;
}

short CHWTerrainPatch::RCreateVarianceTree(const CVector2d &left, short lefth,
										   const CVector2d &right, short righth,
										   const CVector2d &apex, short apexh)
{
	CVector2d center;
	center.x = (left.x + right.x) >> 1;
	center.y = (left.y + right.y) >> 1;

	short center_h = hm[(center.y * hm_per_side) + center.x];
	short variance = abs((int)center_h - (((int)lefth + (int)righth) >> 1));

	if((left - right).Length() >= 3)
	{
		variance = MAX(variance, RCreateVarianceTree(apex, apexh, left, lefth, center, center_h));
		variance = MAX(variance, RCreateVarianceTree(right, righth, apex, apexh, center, center_h));
	}

	cv->AddVariance(left.x, left.y, right.x, right.y, apex.x, apex.y, variance);

	return variance;
}

void CHWTerrainPatch::Reset()
{
	n1 = CHWTerrain::GetNodeFromPool();
	n2 = CHWTerrain::GetNodeFromPool();

	n1->nb_base = n2;
	n1->nb_left = ln ? ln->n2 : NULL;
	n1->nb_right = tn ? tn->n2 : NULL;

	n2->nb_base = n1;
	n2->nb_left = rn ? rn->n1 : NULL;
	n2->nb_right = bn ? bn->n1 : NULL;
}

void CHWTerrainPatch::CalcNormals()
{
	RCalcNormals(n1,
		CVector2d(min.x, max.y),
		CVector2d(max.x, min.y),
		CVector2d(min.x, min.y));

	RCalcNormals(n2,
		CVector2d(max.x, min.y),
		CVector2d(min.x, max.y),
		CVector2d(max.x, max.y));
}

void CHWTerrainPatch::RCalcNormals(CTriNode *n,
									const CVector2d &left,
									const CVector2d &right,
									const CVector2d &apex)
{
	if(!n->left)
	{
		n->CalcNormal(hm, left, right, apex, hm_per_side);
		return;
	}

	CVector2d center;
	center.x = (left.x + right.x) >> 1;
	center.y = (left.y + right.y) >> 1;

	RCalcNormals(n->left, apex, left, center);
	RCalcNormals(n->right, right, apex, center);
}

void CHWTerrainPatch::CalcFlatShade()
{
	RCalcFlatShade(n1);
	RCalcFlatShade(n2);
}

void CHWTerrainPatch::RCalcFlatShade(CTriNode *n)
{
	if(!n->left)
	{
		n->light = unsigned char(((-DotProduct(CVector3f(-1, 0, 0), n->normal) + 1) * 0.5f) * 255);
		return;
	}

	RCalcFlatShade(n->left);
	RCalcFlatShade(n->right);
}

void CHWTerrainPatch::FindVertexNormals()
{
	RFindNormals(n1,
		CVector2d(min.x, max.y),
		CVector2d(max.x, min.y),
		CVector2d(min.x, min.y));

	RFindNormals(n2,
		CVector2d(max.x, min.y),
		CVector2d(min.x, max.y),
		CVector2d(max.x, max.y));
}

void CHWTerrainPatch::RFindNormals(CTriNode *n,
								   const CVector2d &left,
								   const CVector2d &right,
								   const CVector2d &apex)
{
	if(!n->left)
	{
		int idx = (left.y * hm_per_side) + left.x;
		hm_normal[idx].n += n->normal;
		hm_normal[idx].c++;

		idx = (right.y * hm_per_side) + right.x;
		hm_normal[idx].n += n->normal;
		hm_normal[idx].c++;

		idx = (apex.y * hm_per_side) + apex.x;
		hm_normal[idx].n += n->normal;
		hm_normal[idx].c++;

		return;
	}

	CVector2d center;
	center.x = (left.x + right.x) >> 1;
	center.y = (left.y + right.y) >> 1;

	RFindNormals(n->left, apex, left, center);
	RFindNormals(n->right, right, apex, center);
}

void CHWTerrainPatch::CalcGouraudShade()
{
	RCalcGouraudShade(n1,
		CVector2d(min.x, max.y),
		CVector2d(max.x, min.y),
		CVector2d(min.x, min.y));

	RCalcGouraudShade(n2,
		CVector2d(max.x, min.y),
		CVector2d(min.x, max.y),
		CVector2d(max.x, max.y));
}

void CHWTerrainPatch::RCalcGouraudShade(CTriNode *n,
										const CVector2d &left,
										const CVector2d &right,
										const CVector2d &apex)
{
	if(!n->left)
	{
		int idx = (left.y * hm_per_side) + left.x;
		if(!hm_normal[idx].calced)
		{
			hm_normal[idx].light = unsigned char(((-DotProduct(CVector3f(-1, 0, 0), hm_normal[idx].n) + 1) * 0.5f) * 255);
			hm_normal[idx].calced = true;
		}

		idx = (right.y * hm_per_side) + right.x;
		if(!hm_normal[idx].calced)
		{
			hm_normal[idx].light = unsigned char(((-DotProduct(CVector3f(-1, 0, 0), hm_normal[idx].n) + 1) * 0.5f) * 255);
			hm_normal[idx].calced = true;
		}

		idx = (apex.y * hm_per_side) + apex.x;
		if(!hm_normal[idx].calced)
		{
			hm_normal[idx].light = unsigned char(((-DotProduct(CVector3f(-1, 0, 0), hm_normal[idx].n) + 1) * 0.5f) * 255);
			hm_normal[idx].calced = true;
		}

		return;
	}

	CVector2d center;
	center.x = (left.x + right.x) >> 1;
	center.y = (left.y + right.y) >> 1;

	RCalcGouraudShade(n->left, apex, left, center);
	RCalcGouraudShade(n->right, right, apex, center);
}

void CHWTerrainPatch::Tessellate(const CVector3f &cam_pos, int heights_space, short min_v)
{
	cv = v1;
	RTessellate(n1,
		CVector2d(min.x, max.y),
		CVector2d(max.x, min.y),
		CVector2d(min.x, min.y),
		cam_pos, heights_space, min_v);

	cv = v2;
	RTessellate(n2,
		CVector2d(max.x, min.y),
		CVector2d(min.x, max.y),
		CVector2d(max.x, max.y),
		cam_pos, heights_space, min_v);
}

void CHWTerrainPatch::RTessellate(CTriNode *n,
								  const CVector2d &left,
								  const CVector2d &right,
								  const CVector2d &apex,
								  const CVector3f &cam_pos,
								  int heights_space,
								  short min_v)
{
	CVector2d center;
	center.x = (left.x + right.x) >> 1;
	center.y = (left.y + right.y) >> 1;

	float dist = (CVector3f(float(apex.x * heights_space), cam_pos.y, float(-apex.y * heights_space)) - cam_pos).Length();

	if(cv->GetVariance(left.x, left.y, right.x, right.y, apex.x, apex.y) > (min_v + (((int)dist) >> 6)))
	{
		Split(n);

		if(n->right && (left - right).Length() >= 3)
		{
			RTessellate(n->left, apex, left, center, cam_pos, heights_space, min_v);
			RTessellate(n->right, right, apex, center, cam_pos, heights_space, min_v);
		}
	}
}

void CHWTerrainPatch::Split(CTriNode *n)
{
	if(n->left)
		return;

	if(n->nb_base && (n->nb_base->nb_base != n))
		Split(n->nb_base);

	n->left = CHWTerrain::GetNodeFromPool();
	n->right = CHWTerrain::GetNodeFromPool();

	if(!n->right)
		return;

	n->left->nb_base = n->nb_left;
	n->left->nb_left = n->right;

	n->right->nb_base = n->nb_right;
	n->right->nb_right = n->left;

	if(n->nb_left)
	{
		if(n->nb_left->nb_base == n)
			n->nb_left->nb_base = n->left;
		else if(n->nb_left->nb_left == n)
			n->nb_left->nb_left = n->left;
		else if(n->nb_left->nb_right == n)
			n->nb_left->nb_right = n->left;
	}

	if(n->nb_right)
	{
		if(n->nb_right->nb_base == n)
			n->nb_right->nb_base = n->right;
		else if(n->nb_right->nb_right == n)
			n->nb_right->nb_right = n->right;
		else if(n->nb_right->nb_left == n)
			n->nb_right->nb_left = n->right;
	}

	if(n->nb_base)
	{
		if(n->nb_base->left)
		{
			n->nb_base->left->nb_right = n->right;
			n->nb_base->right->nb_left = n->left;
			n->left->nb_right = n->nb_base->right;
			n->right->nb_left = n->nb_base->left;
		}
		else
			Split(n->nb_base);
	}
	else
	{
		n->left->nb_right = NULL;
		n->right->nb_left = NULL;
	}
}

void CHWTerrainPatch::Destruct()
{
	if(v1)
	{
		delete v1;
		v1 = NULL;
	}

	if(v2)
	{
		delete v2;
		v2 = NULL;
	}
}