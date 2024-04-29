#include "HWApp.h"
#include "HWView2D.h"
#include "HWView3D.h"
#include "HWViewTerrain.h"

float &GetSpeedFactor()
{
	return CHWApp::sf;
}

float CHWApp::frame_time = 0;
float CHWApp::speed_factor = 1.0f;
float CHWApp::sf = 0;

CHWApp::CHWApp() : win(&tools, &world, "HyperWorld", 0, 0, 1024, 768, ((WS_OVERLAPPEDWINDOW | WS_MAXIMIZE) & ~WS_MAXIMIZEBOX) & ~WS_SIZEBOX, NULL, IDR_MENU1),
					tools(IDD_DIALOG1, win.hwnd, &world), world(&textures)
{
	win.SetFocus();

	rd = NULL;
	rnd = NULL;
	vp[0] = NULL;
	vp[1] = NULL;
	vp[2] = NULL;
	vp[3] = NULL;
	vp[4] = NULL;
}

CHWApp::~CHWApp()
{
	for(int i = 0; i < 4; i++)
	{
		if(vp[i])
		{
			delete vp[i];
			vp[i] = NULL;
		}
	}

	if(rnd)
	{
		delete rnd;
		rnd = NULL;
	}

	if(rd)
	{
		delete rd;
		rd = NULL;
	}
}

void CHWApp::Run()
{
	if(!Init())
		return;

	while(win.created)
		MainRefresh();
}

bool CHWApp::Init()
{
	rd = new CRenderDevice(&win);
	if(!rd->Init(32, false))
		return false;
	rd->clear_r = 0;
	rd->clear_g = 0;
	rd->clear_b = 0;

	rnd = new CHWRender(this);
	if(!textures.Init())
		return false;

	vp[0] = new CHWViewTerrain(&world, &tools, &textures);
	vp[1] = new CHWView3D(&world, &tools, &textures);
	vp[2] = new CHWView2D(&world, &tools, 0, 1);
	vp[3] = new CHWView2D(&world, &tools, 0, 2);
	vp[4] = new CHWView2D(&world, &tools, 2, 1);
	v_idx = 3;

	if(!timer.Init())
		return false;

	time = timer.GetTime();

	return true;
}

void CHWApp::MainRefresh()
{
	frame_start = time;

	win.Update();
	tools.Update();
	ProcessInput();
	vp[v_idx]->ProcessInput(&win);
	rnd->RenderScene(rd);

	time = timer.GetTime();
	frame_time = time - frame_start;

	sf = frame_time * speed_factor;
}

void CHWApp::ProcessInput()
{
	if(win.hit[KEY_F1])
		v_idx = 0;
	else if(win.hit[KEY_F2])
		v_idx = 1;
	else if(win.hit[KEY_F3])
		v_idx = 2;
	else if(win.hit[KEY_F4])
		v_idx = 3;
	else if(win.hit[KEY_F5])
		v_idx = 4;
}