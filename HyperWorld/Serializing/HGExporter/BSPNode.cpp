#include "BSPNode.h"

CBSPNode::CBSPNode()
{
	isleaf = false;
	front = NULL;
	back = NULL;
	polys = NULL;
}

CBSPNode::~CBSPNode()
{
	Destruct();
}

void CBSPNode::CalcAABB()
{
	bbmax = bbmin = polys->GFDN()->GetData()->vertices.GFDN()->GetData()->p;
	for(CNode<CHWPoly> *p_node = polys->GFDN(); p_node; p_node = p_node->next)
	{
		CHWPoly *poly = p_node->GetData();

		for(CNode<CHWVertex> *v_node = poly->vertices.GFDN(); v_node; v_node = v_node->next)
		{
			CHWVertex *vertex = v_node->GetData();

			if(vertex->p.x > bbmax.x)
				bbmax.x = vertex->p.x;
			if(vertex->p.y > bbmax.y)
				bbmax.y = vertex->p.y;
			if(vertex->p.z > bbmax.z)
				bbmax.z = vertex->p.z;

			if(vertex->p.x < bbmin.x)
				bbmin.x = vertex->p.x;
			if(vertex->p.y < bbmin.y)
				bbmin.y = vertex->p.y;
			if(vertex->p.z < bbmin.z)
				bbmin.z = vertex->p.z;
		}
	}
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

	if(polys)
	{
		delete polys;
		polys = NULL;
	}
}