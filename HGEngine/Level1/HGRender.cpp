#include "HGRender.h"

CRender *CreateRender(CRenderDevice *r, CServer **s, CWindow *aw, CGUI *g)
{
	return new CHGRender(r, s, aw, g);
}

CHGRender::CHGRender(CRenderDevice *r, CServer **s, CWindow *aw, CGUI *g) : CRender(r, s, aw, g)
{
	map = NULL;
	oh = NULL;
}

CHGRender::~CHGRender()
{
	Destruct();
}

bool CHGRender::Init()
{
	if(!CRender::Init())
		return false;

	rd->ALBL_SetMode(ALBL_TRANSPARENCE);

	return true;
}

void CHGRender::Destruct()
{
	map = NULL;
	oh = NULL;

	CRender::Destruct();
}

void CHGRender::GetServerAttributes()
{
	CRender::GetServerAttributes();

	CHGServer *s = (CHGServer*)srv;

	map = &s->map;
	oh = &s->oh;
}

void CHGRender::RenderServerStuff3D()
{
	CRender::RenderServerStuff3D();

	if(oh->player_spawned)
		UpdateCamera();

	rd->cam.UpdateViewMatrix();
	rd->cam.CalcViewFrustum();
	rd->SetTransform3D();

	map->Render(rd);
	oh->Render(rd);
}

void CHGRender::RenderServerStuff2D()
{
	CRender::RenderServerStuff2D();

	rd->SetTransform2D();
}

void CHGRender::UpdateCamera()
{
	rd->cam.pos = oh->player.pos;
	rd->cam.right = oh->player.right;
	rd->cam.up = oh->player.up;
	rd->cam.dir = oh->player.dir;

	rd->cam.pos.y += AVATAR_EYE_HEIGHT;
}