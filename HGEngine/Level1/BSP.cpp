#include "BSP.h"

CBSPNode::CBSPNode()
{
	isleaf = false;
	front = NULL;
	back = NULL;
	parent = NULL;
	poly = NULL;
	brush_idx = NULL;

	num_p = 0;
	num_brushes = 0;
}

CBSPNode::~CBSPNode()
{
	Destruct();
}

void CBSPNode::SetParents(CBSPNode *p)
{
	parent = p;
	if(!isleaf)
	{
		front->SetParents(this);
		back->SetParents(this);
	}
}

void CBSPNode::Destruct()
{
	if(poly)
	{
		delete [] poly;
		poly = NULL;
		num_p = 0;
	}
	if(brush_idx)
	{
		delete [] brush_idx;
		brush_idx = NULL;
		num_brushes = 0;
	}

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
}

//---------------------------------------------------------------------------

CBSP::CBSP()
{
	root_node = NULL;
}

CBSP::~CBSP()
{
	Destruct();
}

void CBSP::Init()
{
	UpdateParents();
}

void CBSP::UpdateParents()
{
	root_node->SetParents(NULL);
}

/*int CBSP::GetPolys(CPoly ***p)
{
	int c = GetPolyCount(), idx = 0;
	*p = new CPoly*[c];

	GP(root_node, *p, &idx);
	// TODO: sort polys by texture

	return c;
}

void CBSP::GP(CBSPNode *n, CPoly **p, int *idx)
{
	if(n->isleaf)
	{
		for(int i = 0; i < n->num_p; i++)
			p[(*idx)++] = &n->poly[i];

		return;
	}

	GP(n->front, p, idx);
	GP(n->back, p, idx);
}

int CBSP::GetPolyCount()
{
	int c = 0;
	GPC(root_node, &c);

	return c;
}

void CBSP::GPC(CBSPNode *n, int *c)
{
	if(n->isleaf)
	{
		*c += n->num_p;
		return;
	}

	GPC(n->front, c);
	GPC(n->back, c);
}*/

void CBSP::Destruct()
{
	if(root_node)
	{
		delete root_node;
		root_node = NULL;
	}
}