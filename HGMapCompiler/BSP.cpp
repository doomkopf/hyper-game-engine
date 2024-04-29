#include "stdafx.h"
#include "BSP.h"

CBSPNode::CBSPNode()
{
	isleaf = false;
	front = NULL;
	back = NULL;
}

CBSPNode::CBSPNode(const CBSPNode &node)
{
	isleaf = false;
	front = NULL;
	back = NULL;

	Copy(node);
}

CBSPNode::~CBSPNode()
{
	Destruct();
}

void CBSPNode::Destruct()
{
	isleaf = false;

	if(front)
	{
		delete front;
		front = NULL;
	}

	if(back)
	{
		delete back;
		back = NULL;
	}

	polys.Clear();
}

void CBSPNode::Copy(const CBSPNode &node)
{
	Destruct();

	isleaf = node.isleaf;
	splitter = node.splitter;
	polys = node.polys;

	if(node.front)
		front = new CBSPNode(*node.front);
	if(node.back)
		back = new CBSPNode(*node.back);
}

//--------------------------------------------------------------

CBSP::CBSP()
{
	root_node = NULL;
}

CBSP::CBSP(const CBSP &bsp)
{
	root_node = NULL;
	Copy(bsp);
}

CBSP::~CBSP()
{
	Destruct();
}

void CBSP::Destruct()
{
	if(root_node)
	{
		delete root_node;
		root_node = NULL;
	}
}

void CBSP::Copy(const CBSP &bsp)
{
	Destruct();

	root_node = new CBSPNode;
	root_node->Copy(*bsp.root_node);
}

CBSP &CBSP::operator=(const CBSP &bsp)
{
	Copy(bsp);

	return *this;
}

void CBSP::BSP()
{
	BSP(root_node);
}

void CBSP::BSP(CBSPNode *node)
{
	CPoly *splitter;
	CList<CPlane> planes;

	splitter = FindBestSplitter(node);

	if(!splitter)
	{
		MakeSolidLeaf(&node->polys, node);
		return;
	}

	if(splitter->bevel)
	{
		Bevel(splitter, node, &planes);
		if(planes.GFDN())
		{
			splitter->bevel = false;
			BSP(node, planes.GFDN());
			return;
		}
	}

	splitter->issplitter = true;
	node->splitter = splitter->plane;
	node->front = new CBSPNode;
	node->back = new CBSPNode;

	for(CNode<CPoly> *p_node = node->polys.GFDN(); p_node; p_node = p_node->next)
	{
		CPoly *poly = p_node->GetData();

		switch(ClassifyPolygon(*poly, node->splitter, EPSILON))
		{
		case FRONT:
			node->front->polys.Add(*poly);
			break;

		case BACK:
			node->back->polys.Add(*poly);
			break;

		case COPLANAR:
			poly->issplitter = true;
			if(poly->plane.normal.Equal(node->splitter.normal, EPSILON))
				node->front->polys.Add(*poly);
			else
				node->back->polys.Add(*poly);
			break;

		case SPANNING:
			CPoly f, b;
			ClipPolygon(*poly, node->splitter, &f, &b);
			node->front->polys.Add(f);
			node->back->polys.Add(b);
			break;
		}
	}

	node->polys.Clear();

	BSP(node->front);
	BSP(node->back);
}

// walks through a list of planes, then delegates to the standard bsp function
void CBSP::BSP(CBSPNode *node, CNode<CPlane> *pl_node)
{
	if(!pl_node)
	{
		BSP(node);
		return;
	}

	node->splitter = *pl_node->GetData();
	node->front = new CBSPNode;
	node->back = new CBSPNode;

	for(CNode<CPoly> *p_node = node->polys.GFDN(); p_node; p_node = p_node->next)
	{
		CPoly *poly = p_node->GetData();

		switch(ClassifyPolygon(*poly, node->splitter, EPSILON))
		{
		case FRONT:
			node->front->polys.Add(*poly);
			break;

		case BACK:
			node->back->polys.Add(*poly);
			break;

		case COPLANAR:
			poly->issplitter = true;
			if(poly->plane.normal.Equal(node->splitter.normal, EPSILON))
				node->front->polys.Add(*poly);
			else
				node->back->polys.Add(*poly);
			break;

		case SPANNING:
			CPoly f, b;
			ClipPolygon(*poly, node->splitter, &f, &b);
			node->front->polys.Add(f);
			node->back->polys.Add(b);
			break;
		}
	}

	node->polys.Clear();

	BSP(node->front);
	BSP(node->back, pl_node->next);
}

CPoly *CBSP::FindBestSplitter(CBSPNode *node)
{
	int front, back, splitted;
	int points, min = 2147483647;
	CPoly *best_splitter = NULL;

	for(CNode<CPoly> *node1 = node->polys.GFDN(); node1; node1 = node1->next)
	{
		CPoly *test_splitter = node1->GetData();

		if(test_splitter->issplitter)
			continue;

		front = back = splitted = 0;

		for(CNode<CPoly> *node2 = node->polys.GFDN(); node2; node2 = node2->next)
		{
			CPoly *poly = node2->GetData();

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

bool CBSP::MakeSolidLeaf(CList<CPoly> *polys, CBSPNode *node)
{
	node->isleaf = true;

	for(CNode<CPoly> *p_node = polys->GFDN(); p_node; p_node = p_node->next)
	{
		CPoly *poly = p_node->GetData();

		if(!poly->issplitter)
		{
			poly->issplitter = true;
			node->splitter = poly->plane;
			node->front = new CBSPNode;
			if(MakeSolidLeaf(polys, node->front))
			{
				delete node->front;
				node->front = NULL;
			}
			return false;
		}
	}

	return true;
}

void CBSP::Bevel(CPoly *poly, CBSPNode *node, CList<CPlane> *planes)
{
	CPlane plane;
	bool _break = false;

	for(CNode<CPoly> *p_node = node->polys.GFDN(); p_node; p_node = p_node->next)
	{
		CPoly *p = p_node->GetData();

		if(p == poly)
			continue;

		if(DotProduct(p->plane.normal, poly->plane.normal) < -0.1f)
		{
			for(CNode<CVertex> *v_node1 = p->vertices.GFDN(); v_node1; v_node1 = v_node1->next)
			{
				CVertex *v1 = v_node1->GetData();

				for(CNode<CVertex> *v_node2 = poly->vertices.GFDN(); v_node2; v_node2 = v_node2->next)
				{
					CVertex *v2 = v_node2->GetData();

					if(v1->p.Equal(v2->p, EPSILON))
					{
						plane.normal = p->plane.normal + poly->plane.normal;
						plane.normal.Normalize();
						plane.d = -DotProduct(v1->p, plane.normal);

						planes->Add(plane);

						_break = true;
						break;
					}
				}
				if(_break)
				{
					_break = false;
					break;
				}
			}
		}
	}
}