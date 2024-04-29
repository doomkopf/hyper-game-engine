#include "stdafx.h"
#include "HGCompiler.h"

CHGCompiler::CHGCompiler()
{
	texture_seg = NULL;
}

CHGCompiler::~CHGCompiler()
{
	if(texture_seg)
	{
		delete texture_seg;
		texture_seg = NULL;
	}
}

bool CHGCompiler::LoadRAWFromFile(const char *file)
{
	if(!fb.Open(file, true))
		return false;

	ReadPolys();
	ReadTextures();

	fb.Close();

	return true;
}

void CHGCompiler::ReadPolys()
{
	int c1, c2, i, j;
	CPoly poly;
	CVertex vertex;

	fb.Read(&c1, sizeof(int));
	for(i = 0; i < c1; i++)
	{
		fb.Read(&poly.tex, sizeof(int));
		fb.Read(&c2, sizeof(int));
		for(j = 0; j < c2; j++)
		{
			fb.Read(&vertex, sizeof(CVertex));
			poly.vertices.Add(vertex);
		}
		if(!bsp.root_node)
			bsp.root_node = new CBSPNode;
		poly.CalcPlane();
		bsp.root_node->polys.Add(poly);
		poly.vertices.Clear();
	}
}

void CHGCompiler::ReadTextures()
{
	fb.Read(&num_textures, sizeof(int));
	texture_seg = new unsigned char[num_textures * 32];
	fb.Read(texture_seg, num_textures * 32);
}

void CHGCompiler::SaveFile(const char *file)
{
	fb.Open(file, false);

	SaveBSP(bsp.root_node);
	SaveTextures();

	fb.Close();
}

void CHGCompiler::SaveBSP(CBSPNode *node)
{
	static unsigned char c;
	static int i;

	if(!node)
	{
		c = 0; // marking a solid node
		fb.Write(&c, sizeof(unsigned char));

		return;
	}

	if(node->isleaf)
	{
		if(node->polys.GFDN())
		{
			c = 1; // marking a poly leaf (first time in recursion where the polys form a convex volume)
			fb.Write(&c, sizeof(unsigned char));

			i = node->polys.GetCount();
			fb.Write(&i, sizeof(int));
			for(CNode<CPoly> *p_node = node->polys.GFDN(); p_node; p_node = p_node->next)
			{
				CPoly *poly = p_node->GetData();

				fb.Write(&poly->tex, sizeof(int));
				fb.Write(&poly->plane, sizeof(CPlane));
				fb.Write(&poly->issplitter, sizeof(bool));
				i = poly->vertices.GetCount();
				fb.Write(&i, sizeof(int));
				for(CNode<CVertex> *v_node = poly->vertices.GFDN(); v_node; v_node = v_node->next)
				{
					fb.Write(v_node->GetData(), sizeof(CVertex));
				}
			}
		}
		else
		{
			c = 2; // marking a leaf (or sub leaf)
			fb.Write(&c, sizeof(unsigned char));
		}

		fb.Write(&node->splitter, sizeof(CPlane));
		SaveBSP(node->front);
		// not saving back node because its always solid in this condition

		return;
	}

	c = 3; // marking a node
	fb.Write(&c, sizeof(unsigned char));
	fb.Write(&node->splitter, sizeof(CPlane));

	SaveBSP(node->front);
	SaveBSP(node->back);
}

void CHGCompiler::SaveTextures()
{
	fb.Write(&num_textures, sizeof(int));
	fb.Write(texture_seg, num_textures * 32);
}

void CHGCompiler::DoStuff()
{
	bsp.BSP();
}