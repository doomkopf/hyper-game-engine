#ifndef MAP_H_
#define MAP_H_

#include "BSP.h"
#include "../Level0/Renderable.h"
#include "../../stuff/FileBuf.h"
#include "../Level0/TextureManager.h"
class CObjectHandler;
#include "ObjectHandler.h"

class CClipBrush
{
public:
	CClipBrush()
	{
		plane = NULL;
		num_planes = 0;
	}
	~CClipBrush()
	{
		if(plane)
		{
			delete [] plane;
			plane = NULL;
			num_planes = 0;
		}
	}

	CPlane *plane;
	int num_planes;
};

class CMap : public IRenderable
{
public:
	CMap(CObjectHandler *);
	~CMap();
	bool Load(const char *);
	void Destruct();
	virtual void Render(CRenderDevice *);

	CBSP bsp;
	CClipBrush *clipbrush;
	int num_clipbrushes;
	CTextureManager tm;

protected:
	void ReadBSP(CBSPNodeP &);
	void ReadAndLoadTextures();
	void RenderBSP(CBSPNode *, CRenderDevice *);

	//CObjectHandler *oh;

	CFileBuf fb;
};

#endif