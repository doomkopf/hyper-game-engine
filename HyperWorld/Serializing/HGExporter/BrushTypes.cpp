#include "BrushTypes.h"
#include "polystuff.h"

CTBrush::CTBrush()
{
}

CTBrush::CTBrush(const CHWBrush &b)
{
	CList<CHWPoly> polys2;
	CHWPoly p_new;

	for(int i = 0; i < b.num_sides; i++)
	{
		CHWPoly *poly = new CHWPoly;
		CHWVertex v;

		poly->tex = b.tex;

		v.p = b.point[b.side[i].point[0]];
		v.texcoord = b.side[i].texcoord[0];
		poly->vertices.Add(v);

		v.p = b.point[b.side[i].point[1]];
		v.texcoord = b.side[i].texcoord[1];
		poly->vertices.Add(v);

		v.p = b.point[b.side[i].point[2]];
		v.texcoord = b.side[i].texcoord[2];
		poly->vertices.Add(v);

		poly->CalcPlane();

		polys2.Add(poly);
	}

loopstart:
	for(CNode<CHWPoly> *p_node1 = polys2.GFDN(); p_node1; p_node1 = p_node1->next)
	{
		CHWPoly *poly1 = p_node1->GetData();

		for(CNode<CHWPoly> *p_node2 = polys2.GFDN(); p_node2; p_node2 = p_node2->next)
		{
			CHWPoly *poly2 = p_node2->GetData();

			if(poly1 == poly2)
				continue;

			if(MergePolys(*poly1, *poly2, &p_new))
			{
				p_node2->Delete();
				p_node1->Delete();
				polys.Add(p_new);

				p_new.vertices.Clear();
				goto loopstart;
			}
		}

		for(p_node2 = polys.GFDN(); p_node2; p_node2 = p_node2->next)
		{
			CHWPoly *poly2 = p_node2->GetData();

			if(poly1 == poly2)
				continue;

			if(MergePolys(*poly1, *poly2, &p_new))
			{
				p_node2->Delete();
				p_node1->Delete();
				polys.Add(p_new);

				p_new.vertices.Clear();
				goto loopstart;
			}
		}
	}

	polys.AddList(polys2);
}

// -------------------------------------------------------------------

CClipBrush::CClipBrush(const CTBrush &b)
{
	for(CNode<CHWPoly> *p_node1 = b.polys.GFDN(); p_node1; p_node1 = p_node1->next)
	{
		CHWPoly *poly = p_node1->GetData();

		AddIfNotExists(poly->plane);
	}

	for(p_node1 = b.polys.GFDN(); p_node1; p_node1 = p_node1->next)
	{
		CHWPoly *poly1 = p_node1->GetData();

		for(CNode<CHWPoly> *p_node2 = b.polys.GFDN(); p_node2; p_node2 = p_node2->next)
		{
			CHWPoly *poly2 = p_node2->GetData();

			if(poly1 == poly2)
				continue;

			if(DotProduct(poly1->plane.normal, poly2->plane.normal) < -EPSILON)
			{
				for(CNode<CHWVertex> *v_node1 = poly1->vertices.GFDN(); v_node1; v_node1 = v_node1->next)
				{
					CVector3f *v1 = &v_node1->GetData()->p;

					for(CNode<CHWVertex> *v_node2 = poly2->vertices.GFDN(); v_node2; v_node2 = v_node2->next)
					{
						CVector3f *v2 = &v_node2->GetData()->p;

						if(v1 == v2)
							continue;

						if(v1->Equal(*v2, EPSILON))
						{
							CPlane plane;
							plane.normal = poly1->plane.normal + poly2->plane.normal;
							plane.normal.Normalize();
							plane.d = -DotProduct(plane.normal, *v1);

							AddIfNotExists(plane);
						}
					}
				}
			}
		}
	}
}

void CClipBrush::AddIfNotExists(const CPlane &plane)
{
	bool ok = true;

	for(CNode<CPlane> *pl_node = planes.GFDN(); pl_node; pl_node = pl_node->next)
	{
		CPlane *pl = pl_node->GetData();

		if(plane.normal.Equal(pl->normal, EPSILON) && (fabs(plane.d - pl->d) <= EPSILON))
		{
			ok = false;
			break;
		}
	}

	if(ok)
		planes.Add(plane);
}