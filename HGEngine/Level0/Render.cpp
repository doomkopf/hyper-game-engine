#include "Render.h"

CMatrix4x4 CRender::m4x4;

CRender::CRender(CRenderDevice *r, CServer **s, CWindow *aw, CGUI *g) : srv(*s)
{
	wnd = aw;
	rd = r;
	gui = g;
	srv_active = false;
	idle = false;
	mpos.x = 0;
	mpos.y = 0;
}

CRender::~CRender()
{
	Destruct();
}

bool CRender::Init()
{
	if(!text.Init(rd))
		return false;

	if(!con.Init(rd))
		return false;

	return true;
}

void CRender::Destruct()
{
	srv_active = false;
}

void CRender::RenderScene()
{
	rd->BeginFrame();

	if(srv)
	{
		if(!srv_active)
		{
			GetServerAttributes();
			srv_active = true;
		}
		RenderServerStuff3D();
	}
	else
		srv_active = false;

	RenderClientStuff3D();


	if(srv)
	{
		RenderServerStuff2D();
		srv->gui.Render(rd, &text);
	}
	else
		srv_active = false;

	RenderClientStuff2D();

	rd->EndFrame();
}

void CRender::RenderServerStuff3D()
{
	rd->ZBuffer_Enable();
}

void CRender::RenderServerStuff2D()
{
	rd->ZBuffer_Disable();
	rd->SetTransform2D();
}

void CRender::RenderCursor()
{
	rd->SetTexture(0, NULL);
	rd->SetVertexShader(rd->vs_diffuse);

	rd->Diffuse_RGB(0, 0, 255);

	rd->BeginTriangles(RND_DIFFUSE);
	rd->Vertex(mpos.x, mpos.y);
	rd->Vertex(mpos.x + 10, mpos.y + 10);
	rd->Vertex(mpos.x, mpos.y + 10);
	rd->EndTriangles();
}

void CRender::RenderClientStuff3D()
{
	rd->ZBuffer_Enable();
}

void CRender::RenderClientStuff2D()
{
	rd->SetTransform2D();
	rd->ZBuffer_Disable();

	gui->Render(rd, &text);

	if(idle)
		RenderCursor();
	con.Render(rd, &text);
}