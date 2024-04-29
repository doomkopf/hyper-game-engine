#include "Map.h"

CMap::CMap(CObjectHandler *o)
{
	//oh = o;
	clipbrush = NULL;
	num_clipbrushes = 0;
}

CMap::~CMap()
{
	Destruct();
}

bool CMap::Load(const char *file)
{
	if(!fb.Open(file, true))
		return false;

	// bsp segment
	ReadBSP(bsp.root_node);
	bsp.Init();

	// brush segment
	fb.Read(&num_clipbrushes, sizeof(int));
	clipbrush = new CClipBrush[num_clipbrushes];
	for(int i = 0; i < num_clipbrushes; i++)
	{
		fb.Read(&clipbrush[i].num_planes, sizeof(int));
		clipbrush[i].plane = new CPlane[clipbrush[i].num_planes];
		fb.Read(clipbrush[i].plane, sizeof(CPlane) * clipbrush[i].num_planes);
	}

	// texture segment
	ReadAndLoadTextures();

	fb.Close();

	return true;
}

void CMap::ReadBSP(CBSPNodeP &node)
{
	node = new CBSPNode;
	fb.Read(&node->isleaf, sizeof(bool));

	if(node->isleaf) // leaf
	{
		fb.Read(&node->bbmin, sizeof(CVector3f));
		fb.Read(&node->bbmax, sizeof(CVector3f));
		fb.Read(&node->num_brushes, sizeof(int));
		node->brush_idx = new int[node->num_brushes];
		fb.Read(node->brush_idx, sizeof(int) * node->num_brushes);

		fb.Read(&node->num_p, sizeof(int));
		node->poly = new CPoly[node->num_p];
		for(int i = 0; i < node->num_p; i++)
		{
			fb.Read(&node->poly[i].tex, sizeof(int));
			fb.Read(&node->poly[i].plane, sizeof(CPlane));
			fb.Read(&node->poly[i].num_v, sizeof(int));
			node->poly[i].vertex = new CVertex[node->poly[i].num_v];
			fb.Read(node->poly[i].vertex, sizeof(CVertex) * node->poly[i].num_v);
		}

		return;
	}

	fb.Read(&node->splitter, sizeof(CPlane));
	fb.Read(&node->bbmin, sizeof(CVector3f));
	fb.Read(&node->bbmax, sizeof(CVector3f));

	ReadBSP(node->front);
	ReadBSP(node->back);
}

void CMap::ReadAndLoadTextures()
{
	int num_tex;
	char tex_name[32], path[64];

	fb.Read(&num_tex, sizeof(int));
	for(int i = 0; i < num_tex; i++)
	{
		fb.Read(tex_name, sizeof(tex_name));
		strcpy(path, "../");
		strcat(path, tex_name);
		tm.LoadFromFile(path);
	}
}

void CMap::Destruct()
{
	if(clipbrush)
	{
		delete [] clipbrush;
		clipbrush = NULL;
		num_clipbrushes = 0;
	}
}

void CMap::Render(CRenderDevice *rd)
{
	rd->SetVertexShader(rd->vs_tex0);
	RenderBSP(bsp.root_node, rd);
}

void CMap::RenderBSP(CBSPNode *node, CRenderDevice *rd)
{
	if(!node)
		return;

	if(!rd->cam.AABBInViewFrustum(node->bbmin, node->bbmax))
		return;

	if(node->isleaf)
	{
		for(int i = 0; i < node->num_p; i++)
		{
			if(node->poly[i].tex == -1)
				continue;
			rd->SetTexture(0, tm.tex[node->poly[i].tex]);
			rd->BeginPoly(RND_TEX0);
			for(int j = 0; j < node->poly[i].num_v; j++)
			{
				rd->Texcoord(node->poly[i].vertex[j].tc);
				rd->Vertex(node->poly[i].vertex[j].p);
			}
			rd->EndPoly();
		}

		return;
	}

	RenderBSP(node->front, rd);
	RenderBSP(node->back, rd);
}