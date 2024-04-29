#include "HWRender.h"
#include "HWApp.h"

CHWRender::CHWRender(CHWApp *a)
{
	app = a;
}

void CHWRender::RenderScene(CRenderDevice *rd)
{
	rd->BeginFrame();

	Render3D(rd);
	Render2D(rd);

	rd->EndFrame();
}

void CHWRender::Render3D(CRenderDevice *rd)
{
	rd->ZBuffer_Enable();

	app->vp[app->v_idx]->Render3D(rd);
}

void CHWRender::Render2D(CRenderDevice *rd)
{
	rd->ZBuffer_Disable();
	rd->SetTransform2D();

	app->vp[app->v_idx]->Render2D(rd);
}