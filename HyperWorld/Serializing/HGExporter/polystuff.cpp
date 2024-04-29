#include "polystuff.h"
#include "../../../HGEngine/Level1/geom.h"
#include "../../HWBrush.h"

int ClassifyPolygon(const CHWPoly &poly, const CPlane &plane, float epsilon)
{
	int front, back, coplanar, c = 0;
	float dist;
	CHWVertex *vertex;

	front = back = coplanar = 0;

	for(CNode<CHWVertex> *node = poly.vertices.GFDN(); node; node = node->next)
	{
		vertex = node->GetData();

		dist = DotProduct(plane.normal, vertex->p) + plane.d;

		if(dist > epsilon)
			front++;
		else if(dist < -epsilon)
			back++;
		else
		{
			coplanar++;
			front++;
			back++;
		}

		c++;
	}

	if(coplanar == c)
		return COPLANAR;
	if(front == c)
		return FRONT;
	if(back == c)
		return BACK;

	return SPANNING;
}

bool MergePolys(const CHWPoly &p1, const CHWPoly &p2, CHWPoly *new_poly)
{
	if(ClassifyPolygon(p1, p2.plane, EPSILON) != COPLANAR)
		return false;

	for(CNode<CHWVertex> *v_node11 = p1.vertices.GFDN(); v_node11; v_node11 = v_node11->next)
	{
		CHWVertex *v11 = v_node11->GetData();

		for(CNode<CHWVertex> *v_node21 = p2.vertices.GFDN(); v_node21; v_node21 = v_node21->next)
		{
			CHWVertex *v21 = v_node21->GetData();

			if(v11->p.Equal(v21->p, EPSILON) && (Equal(v11->texcoord.u, v21->texcoord.u, EPSILON) && Equal(v11->texcoord.v, v21->texcoord.v, EPSILON)))
			{
				CNode<CHWVertex> *v_node12, *v_node22;
				CHWVertex *v12, *v22;

				v_node12 = p1.GetNextVertex(v_node11);
				v_node22 = p2.GetPrevVertex(v_node21);

				v12 = v_node12->GetData();
				v22 = v_node22->GetData();

				if(v12->p.Equal(v22->p, EPSILON))
				{
					for(CNode<CHWVertex> *v_node1 = p1.vertices.GFDN(); v_node1; v_node1 = v_node1->next)
					{
						CHWVertex *v1 = v_node1->GetData();

						new_poly->vertices.Add(*v1);

						if(v1 == v11)
						{
							for(CNode<CHWVertex> *v_node2 = p2.GetNextVertex(v_node21); v_node2 != v_node22; v_node2 = p2.GetNextVertex(v_node2))
							{
								CHWVertex *v2 = v_node2->GetData();

								new_poly->vertices.Add(*v2);
							}
						}
					}

					new_poly->issplitter = p1.issplitter;
					new_poly->plane = p1.plane;
					new_poly->tex = p1.tex;
					new_poly->brush = p1.brush;

					if(!new_poly->CheckIfConvex())
					{
						new_poly->vertices.Clear();
						return false;
					}

					OptimizePoly(new_poly);

					return true;
				}
				else
					break;
			}
		}
	}

	return false;
}

void OptimizePoly(CHWPoly *poly)
{
	if(poly->vertices.GetCount() < 4)
		return;

	CPlane plane;

	for(CNode<CHWVertex> *v_node = poly->vertices.GFDN(); v_node; v_node = v_node->next)
	{
		CNode<CHWVertex> *n = poly->GetNextVertex(v_node);
		CHWVertex *v = poly->GetNextVertex(n)->GetData();

		poly->CalcEdgePlane(v_node, &plane);

		if(ClassifyPoint(v->p, plane, EPSILON) == COPLANAR)
			n->Delete();
	}
}