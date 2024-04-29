#ifndef HWRENDER_H_
#define HWRENDER_H_

#include "../HGEngine/Level0/RenderDevice.h"

class CHWApp;

class CHWRender
{
public:
	CHWRender(CHWApp *);
	void RenderScene(CRenderDevice *);

protected:
	void Render3D(CRenderDevice *);
	void Render2D(CRenderDevice *);

	CHWApp *app;
};

#endif