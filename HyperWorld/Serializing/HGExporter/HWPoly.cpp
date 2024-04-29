#include "HWPoly.h"
#include "../../../HGEngine/Level1/geom.h"
#include "../../HWBrush.h"

CHWPoly::CHWPoly()
{
	issplitter = false;
	brush = NULL;
}

void CHWPoly::CalcPlane()
{
	plane.Create(
		vertices.GFDN()->GetData()->p,
		vertices.GFDN()->next->GetData()->p,
		vertices.GFDN()->next->next->GetData()->p);
}

bool CHWPoly::CheckIfConvex() const
{
	CPlane e_plane;

	for(CNode<CHWVertex> *v_node1 = vertices.GFDN(); v_node1; v_node1 = v_node1->next)
	{
		CalcEdgePlane(v_node1, &e_plane);

		for(CNode<CHWVertex> *v_node2 = vertices.GFDN(); v_node2; v_node2 = v_node2->next)
		{
			CHWVertex *v = v_node2->GetData();

			if(ClassifyPoint(v->p, e_plane, EPSILON) == FRONT)
				return false;
		}
	}

	return true;
}

void CHWPoly::CalcEdgePlane(CNode<CHWVertex> *v_node, CPlane *e_plane) const
{
	CHWVertex *v1 = v_node->GetData();
	CHWVertex *v2 = GetNextVertex(v_node)->GetData();

	e_plane->normal = v2->p - v1->p;
	e_plane->normal = CrossProduct(e_plane->normal, plane.normal);
	e_plane->normal.Normalize();
	e_plane->d = -DotProduct(e_plane->normal, v1->p);
}

CNode<CHWVertex> *CHWPoly::GetNextVertex(CNode<CHWVertex> *v_node) const
{
	if(!v_node->next)
		return vertices.GFDN();

	return v_node->next;
}

CNode<CHWVertex> *CHWPoly::GetPrevVertex(CNode<CHWVertex> *v_node) const
{
	if(!v_node->prev)
		return vertices.GLDN();

	return v_node->prev;
}