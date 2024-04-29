#include "HGExporter.h"
#include "../../../HGEngine/Level1/geom.h"
#include "../../../stuff/proc.h"
#include "polystuff.h"

bool CHGExporter::Export(const CHWWorld &world, const char *file)
{
	return true;
}

bool CHGExporter::Export(const CHWBuilding &building, const char *file)
{
	CList<CHWPoly> *polys = CSG_Union(building.brushes);
	FilterUsedTextures(building, polys);
	CBSPNode *bsp = BSP(polys);

	if(!fb.Open(file, false))
		return false;

	// bsp segment
	SaveBSP(bsp);

	// brush segment
	int c = clip_brushes.GetCount();
	fb.Write(&c, sizeof(int));
	for(CNode<CClipBrush> *b_node = clip_brushes.GFDN(); b_node; b_node = b_node->next)
	{
		CClipBrush *brush = b_node->GetData();

		c = brush->planes.GetCount();
		fb.Write(&c, sizeof(int));
		for(CNode<CPlane> *p_node = brush->planes.GFDN(); p_node; p_node = p_node->next)
		{
			CPlane *plane = p_node->GetData();
			fb.Write(plane, sizeof(CPlane));
		}
	}

	// texture segment
	SaveTextures(building, bsp);

	fb.Close();

	delete bsp;

	return true;
}

void CHGExporter::FilterUsedTextures(const CHWBuilding &building, CList<CHWPoly> *polys)
{
	CList<int> tex_idx;
	int counter = 0;

	for(CNode<CHWPoly> *p_node = polys->GFDN(); p_node; p_node = p_node->next)
	{
		CHWPoly *poly = p_node->GetData();

		// check if the polys texture is already in the list
		bool inlist = false;
		for(CNode<int> *tidx_node = tex_idx.GFDN(); tidx_node; tidx_node = tidx_node->next)
		{
			if(poly->tex == *tidx_node->GetData())
			{
				inlist = true;
				break;
			}
		}
		if(!inlist)
		{
			tex_idx.Add(poly->tex);
			ttextures.Add(building.textures->name[poly->tex]);
			poly->new_tex = counter;
			// setting all other polys with the same texture
			for(CNode<CHWPoly> *p_node2 = polys->GFDN(); p_node2; p_node2 = p_node2->next)
			{
				CHWPoly *poly2 = p_node2->GetData();

				if(poly == poly2)
					continue;

				if(poly2->tex == poly->tex)
				{
					poly2->new_tex = counter;
				}
			}

			counter++;
		}
	}
}

void CHGExporter::SaveTextures(const CHWBuilding &building, CBSPNode *bsp)
{
	int c = ttextures.GetCount();
	fb.Write(&c, sizeof(int));
	for(CNode<char*> *t_node = ttextures.GFDN(); t_node; t_node = t_node->next)
	{
		char *str = *t_node->GetData();
		CleanStr(str, 32);
		fb.Write(str, 32);
	}
}

void CHGExporter::SaveBSP(CBSPNode *node)
{
	static unsigned char c;
	static int ci;

	if(node->isleaf)
	{
		fb.Write(&node->isleaf, sizeof(bool));
		fb.Write(&node->bbmin, sizeof(CVector3f));
		fb.Write(&node->bbmax, sizeof(CVector3f));

		int ci = node->brushes.GetCount();
		fb.Write(&ci, sizeof(int));
		for(CNode<CClipBrush*> *cl_node = node->brushes.GFDN(); cl_node; cl_node = cl_node->next)
		{
			ci = clip_brushes.GetIndex(*cl_node->GetData());
			fb.Write(&ci, sizeof(int));
		}

		ci = node->polys->GetCount();
		fb.Write(&ci, sizeof(int));
		for(CNode<CHWPoly> *p_node = node->polys->GFDN(); p_node; p_node = p_node->next)
		{
			CHWPoly *poly = p_node->GetData();

			fb.Write(&poly->new_tex, sizeof(int));
			fb.Write(&poly->plane, sizeof(CPlane));

			ci = poly->vertices.GetCount();
			fb.Write(&ci, sizeof(int));
			for(CNode<CHWVertex> *v_node = poly->vertices.GFDN(); v_node; v_node = v_node->next)
			{
				fb.Write(v_node->GetData(), sizeof(CHWVertex));
			}
		}

		return;
	}

	fb.Write(&node->isleaf, sizeof(bool));
	fb.Write(&node->splitter, sizeof(CPlane));
	fb.Write(&node->bbmin, sizeof(CVector3f));
	fb.Write(&node->bbmax, sizeof(CVector3f));

	SaveBSP(node->front);
	SaveBSP(node->back);
}

CBSPNode *CHGExporter::BSP(CList<CHWPoly> *polys)
{
	CBSPNode *root_node = new CBSPNode;
	root_node->polys = polys;
	BSP(root_node);

	return root_node;
}

void CHGExporter::BSP(CBSPNode *node)
{
	CHWPoly *splitter;
	CList<CPlane> planes;

	node->CalcAABB();

	splitter = FindBestSplitter(node);

	if(!splitter)
	{
		node->isleaf = true;
		// finding all brushes belonging to this leaf
		for(CNode<CHWPoly> *p_node = node->polys->GFDN(); p_node; p_node = p_node->next)
		{
			CHWPoly *poly = p_node->GetData();

			bool ok = true;
			for(CNode<CClipBrush*> *cl_node = node->brushes.GFDN(); cl_node; cl_node = cl_node->next)
			{
				CClipBrush *cl = *cl_node->GetData();

				if(poly->brush == cl)
				{
					ok = false;
					break;
				}
			}
			if(ok)
				node->brushes.Add(poly->brush);
		}
		return;
	}

	splitter->issplitter = true;
	node->splitter = splitter->plane;
	node->front = new CBSPNode;
	node->back = new CBSPNode;
	node->front->polys = new CList<CHWPoly>;
	node->back->polys = new CList<CHWPoly>;
	node->front->polys->Add(*splitter);

	for(CNode<CHWPoly> *p_node = node->polys->GFDN(); p_node; p_node = p_node->next)
	{
		CHWPoly *poly = p_node->GetData();

		if(poly == splitter)
			continue;

		switch(ClassifyPolygon(*poly, node->splitter, EPSILON))
		{
		case FRONT:
			node->front->polys->Add(*poly);
			break;

		case BACK:
			node->back->polys->Add(*poly);
			break;

		case COPLANAR:
			poly->issplitter = true;
			if(poly->plane.normal.Equal(node->splitter.normal, EPSILON))
				node->front->polys->Add(*poly);
			else
				node->back->polys->Add(*poly);
			break;

		case SPANNING:
			CHWPoly f, b;
			ClipPolygon(*poly, node->splitter, &f, &b);
			node->front->polys->Add(f);
			node->back->polys->Add(b);
			break;
		}
	}

	node->polys->Clear();

	BSP(node->front);
	BSP(node->back);
}

CList<CHWPoly> *CHGExporter::CSG_Union(const CList<CHWBrush> &brushes)
{
	CList<CHWPoly> *polys = new CList<CHWPoly>;

	for(CNode<CHWBrush> *b_node1 = brushes.GFDN(); b_node1; b_node1 = b_node1->next)
	{
		CHWBrush *brush1 = b_node1->GetData();

		CTBrush tbrush(*brush1);

		for(CNode<CHWBrush> *b_node2 = brushes.GFDN(); b_node2; b_node2 = b_node2->next)
		{
			CHWBrush *brush2 = b_node2->GetData();

			if(brush1 == brush2)
				continue;
			if(!brush1->Intersects(*brush2))
				continue;

			ClipBrushToBrush(&tbrush, *brush2);
		}

		// putting clipped brush into the polylist
		CClipBrush *cl = new CClipBrush(tbrush);
		for(CNode<CHWPoly> *p_node = tbrush.polys.GFDN(); p_node; p_node = p_node->next)
			p_node->GetData()->brush = cl;
		clip_brushes.Add(cl);
		polys->AddList(tbrush.polys);
	}

	return polys;
}

bool CHGExporter::ClipPolyToBrush(CHWPoly *poly, const CHWBrush &brush, int side, bool splitted)
{
	CHWPoly front, back;

	switch(ClassifyPolygon(*poly, brush.side[side].plane, EPSILON))
	{
	case FRONT:
		if(splitted)
			return true;
		tpolys.Add(*poly);
		break;

	case COPLANAR:
		if(poly->plane.normal.Equal(brush.side[side].plane.normal, EPSILON))
		{
			if(splitted)
				return true;
			tpolys.Add(*poly);
			break;
		}
	case BACK:
		if(side >= brush.num_sides - 1)
		{
			break;
		}
		return ClipPolyToBrush(poly, brush, side + 1, splitted);

	case SPANNING:
		ClipPolygon(*poly, brush.side[side].plane, &front,&back);
		if(side >= brush.num_sides - 1)
		{
			tpolys.Add(front);
			break;
		}
		if(ClipPolyToBrush(&back, brush, side + 1, true))
		{
			if(splitted)
				return true;
			tpolys.Add(*poly);
		}
		else
			tpolys.Add(front);
		break;
	}

	return false;
}

void CHGExporter::ClipBrushToBrush(CTBrush *brush, const CHWBrush &clipper)
{
	for(CNode<CHWPoly> *p_node = brush->polys.GFDN(); p_node; p_node = p_node->next)
	{
		CHWPoly *poly = p_node->GetData();

		ClipPolyToBrush(poly, clipper, 0, false);
	}
	brush->polys.Copy(tpolys);
	tpolys.Clear();
}

void CHGExporter::ClipPolygon(const CHWPoly &poly,const CPlane &plane,CHWPoly *front,CHWPoly *back)
{
	float d1, d2;
	CHWVertex *v1, *v2;
	CHWVertex v_new;

	front->plane = back->plane = poly.plane;
	front->tex = back->tex = poly.tex;
	front->new_tex = back->new_tex = poly.new_tex;
	front->issplitter = back->issplitter = poly.issplitter;
	front->brush = back->brush = poly.brush;

	for(CNode<CHWVertex> *node = poly.vertices.GFDN(); node; node = node->next)
	{
		v1 = node->GetData();
		if(!node->next)
			v2 = poly.vertices.GFDN()->GetData();
		else
			v2 = node->next->GetData();

		d1 = DotProduct(plane.normal, v1->p) + plane.d;
		d2 = DotProduct(plane.normal, v2->p) + plane.d;

		if(d1 > 0 && d2 < 0) // v1=front v2=back
		{
			// adding new vertex to both polys
			GetIntersectionPoint_lineseg_plane(
				v1->p,
				v2->p,
				plane,
				&v_new.p);
			float l1 = (v2->p - v1->p).Length();
			float l2 = (v_new.p - v1->p).Length();
			float percent = l2 / l1;
			float deltax = v2->texcoord.u - v1->texcoord.u;
			float deltay = v2->texcoord.v - v1->texcoord.v;
			v_new.texcoord.u = v1->texcoord.u + (deltax * percent);
			v_new.texcoord.v = v1->texcoord.v + (deltay * percent);
			front->vertices.Add(v_new);
			back->vertices.Add(v_new);

			// adding v2 to back poly
			back->vertices.Add(*v2);
		}
		else if(d1 < 0 && d2 > 0) // v1=back v2=front
		{
			// adding new vertex to both polys
			GetIntersectionPoint_lineseg_plane(
				v2->p,
				v1->p,
				plane,
				&v_new.p);
			float l1 = (v2->p - v1->p).Length();
			float l2 = (v_new.p - v1->p).Length();
			float percent = l2 / l1;
			float deltax = v2->texcoord.u - v1->texcoord.u;
			float deltay = v2->texcoord.v - v1->texcoord.v;
			v_new.texcoord.u = v1->texcoord.u + (deltax * percent);
			v_new.texcoord.v = v1->texcoord.v + (deltay * percent);
			front->vertices.Add(v_new);
			back->vertices.Add(v_new);

			// adding v2 to front poly
			front->vertices.Add(*v2);
		}
		else if(d1 < 0 && d2 == 0) // v1=back v2=onplane
		{
			// adding v2 to both
			front->vertices.Add(*v2);
			back->vertices.Add(*v2);
		}
		else if(d1 > 0 && d2 == 0) // v1=front v2==onplane
		{
			// adding v2 to both
			front->vertices.Add(*v2);
			back->vertices.Add(*v2);
		}
		else if(d1 == 0 && d2 > 0) // v1=onplane v2=front
		{
			// adding v2 to front poly
			front->vertices.Add(*v2);
		}
		else if(d1 == 0 && d2 < 0) // v1=onplane v2=back
		{
			// adding v2 to back poly
			back->vertices.Add(*v2);
		}
		else if(d1 > 0 && d2 > 0) // v1=front v2=front
		{
			// adding v2 to front poly
			front->vertices.Add(*v2);
		}
		else if(d1 < 0 && d2 < 0) // v1=back v2=back
		{
			// adding v2 to back poly
			back->vertices.Add(*v2);
		}
		else
		{
			// Error clipping poly
		}
	}
}

CHWPoly *CHGExporter::FindBestSplitter(CBSPNode *node)
{
	int front, back, splitted;
	int points, min = 2147483647;
	CHWPoly *best_splitter = NULL;

	for(CNode<CHWPoly> *node1 = node->polys->GFDN(); node1; node1 = node1->next)
	{
		CHWPoly *test_splitter = node1->GetData();

		if(test_splitter->issplitter)
			continue;

		front = back = splitted = 0;

		for(CNode<CHWPoly> *node2 = node->polys->GFDN(); node2; node2 = node2->next)
		{
			CHWPoly *poly = node2->GetData();

			if(test_splitter == poly) // dont test the splitter against itself
				continue;

			switch(ClassifyPolygon(*poly, test_splitter->plane, EPSILON))
			{
			case FRONT:
				front++;
				break;

			case BACK:
				back++;
				break;

			case COPLANAR:
				//poly->plane.normal.Equal(test_splitter->plane.normal) ? front++ : back++;
				break;

			case SPANNING:
				splitted++;
				break;
			}
		}

		points = abs(front - back) + (splitted * 3);
		if(points < min)
		{
			if(((front > 0) && (back > 0)) || splitted > 0)
			{
				min = points;
				best_splitter = test_splitter;
			}
		}
	}

	return best_splitter;
}