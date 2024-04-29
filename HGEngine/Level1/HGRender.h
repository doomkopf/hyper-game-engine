#ifndef HGRENDER_H_
#define HGRENDER_H_

#include "../Level0/Render.h"
#include "HGServer.h"

class CHGRender : public CRender
{
public:
	CHGRender(CRenderDevice *, CServer **, CWindow *, CGUI *);
	virtual ~CHGRender();
	virtual bool Init();
	virtual void Destruct();

protected:
	virtual void RenderServerStuff3D();
	virtual void RenderServerStuff2D();
	virtual void GetServerAttributes();

	void UpdateCamera();

	CMap *map;
	CObjectHandler *oh;
};

#endif