#include "HWBrush.h"
#include <memory.h>
#include "../HGEngine/Level1/geom.h"

#define DEFAULT_TEXSIZE 64

CHWBrush::CHWBrush()
{
	DefConstructor();
}

CHWBrush::CHWBrush(const CHWBrush &b)
{
	DefConstructor();
	Copy(b);
}

CHWBrush::~CHWBrush()
{
	Destruct();
}

void CHWBrush::DefConstructor()
{
	point = NULL;
	side = NULL;
	point_selected = NULL;

	num_points = 0;
	num_sides = 0;

	tex = -1;

	selected = true;
}

void CHWBrush::Copy(const CHWBrush &b)
{
	Destruct();

	num_points = b.num_points;
	point = new CVector3f[num_points];
	point_selected = new bool[num_points];
	memcpy(point, b.point, sizeof(CVector3f) * num_points);
	memcpy(point_selected, b.point_selected, sizeof(bool) * num_points);

	num_sides = b.num_sides;
	side = new CHWBrushSide[num_sides];
	memcpy(side, b.side, sizeof(CHWBrushSide) * num_sides);

	tex = b.tex;
	selected = b.selected;
}

void CHWBrush::Destruct()
{
	if(point)
	{
		delete [] point;
		point = NULL;
		num_points = 0;
	}

	if(point_selected)
	{
		delete [] point_selected;
		point_selected = NULL;
	}

	if(side)
	{
		delete [] side;
		side = NULL;
		num_sides = 0;
	}
}

void CHWBrush::CalcPlanes()
{
	for(int i = 0; i < num_sides; i++)
	{
		side[i].plane.Create(
			point[side[i].point[0]],
			point[side[i].point[1]],
			point[side[i].point[2]]);
	}
}

void CHWBrush::CalcTexCoords()
{
	for(int s = 0; s < num_sides; s++)
	{
		for(int i = 0; i < 3; i++)
		{
			// x axis
			if((abs_f(side[s].plane.normal.x) > abs_f(side[s].plane.normal.y)) &&
				(abs_f(side[s].plane.normal.x) > abs_f(side[s].plane.normal.z)))
			{
				side[s].texcoord[i].u = point[side[s].point[i]].z / DEFAULT_TEXSIZE;
				side[s].texcoord[i].v = -point[side[s].point[i]].y / DEFAULT_TEXSIZE;
			}
			// y axis
			else if((abs_f(side[s].plane.normal.y) > abs_f(side[s].plane.normal.x)) &&
				(abs_f(side[s].plane.normal.y) > abs_f(side[s].plane.normal.z)))
			{
				side[s].texcoord[i].u = point[side[s].point[i]].x / DEFAULT_TEXSIZE;
				side[s].texcoord[i].v = -point[side[s].point[i]].z / DEFAULT_TEXSIZE;
			}
			// z axis
			else
			{
				side[s].texcoord[i].u = point[side[s].point[i]].x / DEFAULT_TEXSIZE;
				side[s].texcoord[i].v = -point[side[s].point[i]].y / DEFAULT_TEXSIZE;
			}
		}
	}
}

int CHWBrush::ClassifySide(int s, const CPlane &plane) const
{
	int front = 0, back = 0, coplanar = 0;
	float d;

	d = DotProduct(point[side[s].point[0]], plane.normal) + plane.d;
	if(d > 0)
		front++;
	else if(d < 0)
		back++;
	else
		coplanar++;

	d = DotProduct(point[side[s].point[1]], plane.normal) + plane.d;
	if(d > 0)
		front++;
	else if(d < 0)
		back++;
	else
		coplanar++;

	d = DotProduct(point[side[s].point[2]], plane.normal) + plane.d;
	if(d > 0)
		front++;
	else if(d < 0)
		back++;
	else
		coplanar++;

	if(front == 3)
		return FRONT;
	if(back == 3)
		return BACK;
	if(COPLANAR == 3)
		return COPLANAR;

	return SPANNING;
}

creationresult_t CHWBrush::CreateBlock(const CList<CVector3f> &points, float depth, int c)
{
	CVector3f p_min, p_max;
	int i;

	i = points.GetCount();

	if(i > 2) // dont do this
		return CRRES_IMPOSSIBLE;

	if(i < 2)
		return CRRES_NOTYET;

	p_min = *points.GFDN()->GetData();
	p_max = *points.GFDN()->next->GetData();

	if(p_min.Equal(p_max))
		return CRRES_IMPOSSIBLE;

	((float*)&p_max)[c] += depth;

	num_points = 8;
	point = new CVector3f[num_points];
	point_selected = new bool[num_points];
	for(i = 0; i < num_points; i++)
		point_selected[i] = false;
	num_sides = 12;
	side = new CHWBrushSide[num_sides];

	point[0] = p_min;

	point[1].x = p_max.x;
	point[1].y = p_min.y;
	point[1].z = p_min.z;

	point[2].x = p_max.x;
	point[2].y = p_min.y;
	point[2].z = p_max.z;

	point[3].x = p_min.x;
	point[3].y = p_min.y;
	point[3].z = p_max.z;

	point[4] = p_max;

	point[5].x = p_min.x;
	point[5].y = p_max.y;
	point[5].z = p_max.z;

	point[6].x = p_min.x;
	point[6].y = p_max.y;
	point[6].z = p_min.z;

	point[7].x = p_max.x;
	point[7].y = p_max.y;
	point[7].z = p_min.z;

	// front
	side[0].point[0] = 0;
	side[0].point[1] = 7;
	side[0].point[2] = 6;
	side[1].point[0] = 0;
	side[1].point[1] = 1;
	side[1].point[2] = 7;

	// back
	side[2].point[0] = 2;
	side[2].point[1] = 5;
	side[2].point[2] = 4;
	side[3].point[0] = 2;
	side[3].point[1] = 3;
	side[3].point[2] = 5;

	// left
	side[4].point[0] = 3;
	side[4].point[1] = 6;
	side[4].point[2] = 5;
	side[5].point[0] = 3;
	side[5].point[1] = 0;
	side[5].point[2] = 6;

	// right
	side[6].point[0] = 1;
	side[6].point[1] = 4;
	side[6].point[2] = 7;
	side[7].point[0] = 1;
	side[7].point[1] = 2;
	side[7].point[2] = 4;

	// top
	side[8].point[0] = 6;
	side[8].point[1] = 4;
	side[8].point[2] = 5;
	side[9].point[0] = 6;
	side[9].point[1] = 7;
	side[9].point[2] = 4;

	// bottom
	side[10].point[0] = 3;
	side[10].point[1] = 1;
	side[10].point[2] = 0;
	side[11].point[0] = 3;
	side[11].point[1] = 2;
	side[11].point[2] = 1;

	CalcPlanes();
	CalcTexCoords();

	return CRRES_OK;
}

creationresult_t CHWBrush::CreatePolyhedron(CList<CVector3f> &points, float depth, int c)
{
	int i, j;
	static bool front;

	i = points.GetCount();

	if(i <= 1)
		return CRRES_NOTYET;

	if(CheckDoublePoints(points, points.GLDN(), (i < 4) ? true : false))
		return CRRES_IMPOSSIBLE;

	if(i == 3)
	{
		CVector3f v;
		CPlane plane;

		v.Set(0, 0, 0);
		if(c == 2)
			((float*)&v)[c] = -1;
		else
			((float*)&v)[c] = 1;
		plane.normal = CrossProduct(v, (*points.GFDN()->next->GetData() - *points.GFDN()->GetData()));
		plane.normal.Normalize();
		plane.d = -DotProduct(*points.GFDN()->GetData(), plane.normal);

		if((DotProduct(*points.GLDN()->GetData(), plane.normal) + plane.d) >= 0)
			front = true;
		else
			front = false;
	}

	if(i < 4)
		return CRRES_NOTYET;

	if(!CheckPointsConvex(points, front, c))
		return CRRES_IMPOSSIBLE;

	if(!points.GFDN()->GetData()->Equal(*points.GLDN()->GetData(), EPSILON))
		return CRRES_NOTYET;

	points.GLDN()->Delete();

	// reversing the point order if necessary, to create outfacing brushsides
	if(c == 2)
	{
		if(front)
			points.Reverse();
	}
	else
	{
		if(!front)
			points.Reverse();
	}

	num_points = points.GetCount() * 2;
	point = new CVector3f[num_points];
	point_selected = new bool[num_points];
	for(i = 0; i < num_points; i++)
		point_selected[i] = false;
	num_sides = num_points + (((num_points / 2) - 2) * 2);
	side = new CHWBrushSide[num_sides];

	i = 0;
	for(CNode<CVector3f> *p_node = points.GFDN(); p_node; p_node = p_node->next, i++)
	{
		point[i] = *p_node->GetData();
	}

	for(CNode<CVector3f> *p_node = points.GFDN(); p_node; p_node = p_node->next, i++)
	{
		point[i] = *p_node->GetData();
		((float*)&point[i])[c] += depth;
	}

	for(i = 0, j = 0; i < num_points / 2; i++)
	{
		if(i == (num_points / 2) - 1)
		{
			side[j].point[0] = i;
			side[j].point[1] = 0;
			side[j].point[2] = num_points / 2;

			j++;

			side[j].point[0] = i;
			side[j].point[1] = num_points / 2;
			side[j].point[2] = i + (num_points / 2);

			j++;
		}
		else
		{
			side[j].point[0] = i;
			side[j].point[1] = i + 1;
			side[j].point[2] = i + 1 + (num_points / 2);

			j++;

			side[j].point[0] = i;
			side[j].point[1] = i + 1 + (num_points / 2);
			side[j].point[2] = i + (num_points / 2);

			j++;
		}
	}

	for(i = 0; i < (num_points / 2) - 2; i++)
	{
		side[j].point[0] = 0;
		side[j].point[1] = i + 2;
		side[j].point[2] = i + 1;

		j++;
	}

	for(i = num_points / 2; i < (num_points / 2) + ((num_points / 2) - 2); i++)
	{
		side[j].point[0] = num_points / 2;
		side[j].point[1] = i + 1;
		side[j].point[2] = i + 2;

		j++;
	}

	CalcPlanes();
	CalcTexCoords();

	return CRRES_OK;
}

bool CHWBrush::CheckIfConvex() const
{
	int i, j;

	for(i = 0; i < num_sides; i++)
	{
		for(j = 0; j < num_points; j++)
		{
			if((DotProduct(point[j], side[i].plane.normal) + side[i].plane.d) > EPSILON)
				return false;
		}
	}

	return true;
}

bool CHWBrush::CheckDoublePoints(const CList<CVector3f> &points, CNode<CVector3f> *n, bool b) const
{
	if(points.GetCount() < 2)
		return false;

	for(CNode<CVector3f> *p_node = b ? points.GFDN() : points.GFDN()->next;p_node;p_node=p_node->next)
	{
		CVector3f *p = p_node->GetData();

		if(p_node == n)
			continue;

		if(p->Equal(*n->GetData(),EPSILON))
			return true;
	}

	return false;
}

bool CHWBrush::CheckPointsConvex(const CList<CVector3f> &points, bool front, int c) const
{
	CPlane plane;
	CVector3f v(0, 0, 0);

	if(c == 2)
		((float*)&v)[c] = -1;
	else
		((float*)&v)[c] = 1;

	for(CNode<CVector3f> *p_node1 = points.GFDN(); p_node1->next; p_node1 = p_node1->next)
	{
		CVector3f *p1 = p_node1->GetData(), *p2 = p_node1->next->GetData();

		plane.normal = CrossProduct(v, (*p2 - *p1));
		plane.normal.Normalize();
		plane.d = -DotProduct(*p1, plane.normal);

		for(CNode<CVector3f> *p_node2 = points.GFDN(); p_node2; p_node2 = p_node2->next)
		{
			CVector3f *p = p_node2->GetData();

			if(p == p1 || p == p2)
				continue;

			if(front)
			{
				if((DotProduct(*p, plane.normal) + plane.d) < -EPSILON)
					return false;
			}
			else
			{
				if((DotProduct(*p, plane.normal) + plane.d) > EPSILON)
					return false;
			}
		}
	}

	return true;
}

bool CHWBrush::Intersects(const CHWBrush &b) const
{
	for(int i = 0; i < num_sides; i++)
	{
		if(SideInSolid(i, b, 0))
			return true;
	}

	return false;
}

bool CHWBrush::SideInSolid(int s, const CHWBrush &b, int cs) const
{
	switch(ClassifySide(s, b.side[cs].plane))
	{
	case FRONT:
		break;

	case BACK:
	case COPLANAR:
	case SPANNING:
		if(cs >= b.num_sides - 1)
			return true;
		return SideInSolid(s, b, cs + 1);
		break;
	}

	return false;
}

struct t_point
{
	CVector3f p;
	int idx;
};

struct t_side
{
	int p[3];
};

void CHWBrush::ClipToPlane(const CPlane &plane, CHWBrush *bnew_front, CHWBrush *bnew_back) const
{
	CList<t_point> pfront, pback;
	t_point p;

	// splitting existing points and saving index so the brushsides can link to them
	for(int i = 0; i < num_points; i++)
	{
		switch(ClassifyPoint(point[i], plane, EPSILON))
		{
		case FRONT:
			p.p = point[i];
			p.idx = i;
			pfront.Add(p);
			break;

		case BACK:
			p.p = point[i];
			p.idx = i;
			pback.Add(p);
			break;

		case COPLANAR:
			p.p = point[i];
			p.idx = i;
			pfront.Add(p);
			pback.Add(p);
			break;
		}
	}

	// getting the split points by going along the side edges
	int c = num_points;
	for(i = 0; i < num_sides; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			CVector3f &p1 = point[side[i].point[j]];
			CVector3f &p2 = point[side[i].point[j == 2 ? 0 : j + 1]];
			switch(ClassifyLineseg(point[side[i].point[j]], point[side[i].point[j == 2 ? 0 : j + 1]], plane, EPSILON))
			{
			case SPANNING_FB:
				GetIntersectionPoint_lineseg_plane(p1, p2, plane, &p.p);
				p.idx = c++;
				pfront.Add(p);
				pback.Add(p);
				side[i].point[j == 2 ? 0 : j + 1] = p.idx;
				break;

			case SPANNING_BF:
				GetIntersectionPoint_lineseg_plane(p1, p2, plane, &p.p);
				p.idx = c++;
				pfront.Add(p);
				pback.Add(p);
				break;
			}
		}
	}
}