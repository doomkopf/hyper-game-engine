#include "Client.h"
#include "Engine.h"

CRender *CreateRender(CRenderDevice *, CServer **, CWindow *, CGUI *);

float &CClient::min_ft = CEngine::min_frame_time;

CClient::CClient(CServer **s, CRenderDevice *r, CControl *c, CWindow *aw, CTimer *t) : srv(*s)
{
	rnd = CreateRender(r, s, aw, &gui);
	control = c;
	wnd = aw;
	timer = t;

	run = false;
}

CClient::~CClient()
{
	Destruct();
}

bool CClient::Init()
{
	if(!rnd->Init())
		return false;

	return true;
}

void CClient::Destruct()
{
	run = false;
}

void CClient::Update1()
{
	wnd->Update();
	control->Update();

	if(/*wnd->key[VK_ESCAPE] || */control->key[KEY_ESCAPE])
		run = false;

	if(control->hit[KEY_TAB])
		rnd->con.DropToggle();
	rnd->con.Update();

	if(rnd->con.active)
	{
		rnd->con.AddInput(*control);
		if(rnd->con.HasCommand())
		{
			ExecCommand(rnd->con.input.str);
			rnd->con.cur = 0;
			rnd->con.input.str[rnd->con.cur] = 0;
		}
	}
	else if(rnd->idle)
	{
		UpdateInput2();

		rnd->mpos.x += (int)control->mouse_x;
		if(rnd->mpos.x < 0)
			rnd->mpos.x = 0;
		else if(rnd->mpos.x > rnd->rd->bb_x)
			rnd->mpos.x = rnd->rd->bb_x;

		rnd->mpos.y += (int)control->mouse_y;
		if(rnd->mpos.y < 0)
			rnd->mpos.y = 0;
		else if(rnd->mpos.y > rnd->rd->bb_y)
			rnd->mpos.y = rnd->rd->bb_y;

		if(srv)
		{
			srv->gui.ProcessInput(control, rnd->mpos, rnd->rd->bb_x, rnd->rd->bb_y);
			srv->gui.Update();
			if(!srv->gui.focus_window)
				srv->ProcessClientInput2(control, rnd->mpos, rnd->rd->bb_x, rnd->rd->bb_y);
		}
	}
	else
	{
		UpdateInput1();

		if(srv)
			srv->ProcessClientInput1(control);
	}
}

void CClient::Update2()
{
	rnd->RenderScene();
}

void CClient::UpdateInput1()
{
}

void CClient::UpdateInput2()
{
	gui.ProcessInput(control, rnd->mpos, rnd->rd->bb_x, rnd->rd->bb_y);
	gui.Update();
}

void CClient::ExecCommand(const char *cmd)
{
}