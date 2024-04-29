#include "Engine.h"

CClient *CreateClient(CServer **, CRenderDevice *, CControl *, CWindow *, CTimer *);

//CRender *CreateRender(CRenderDevice *, CServer **, CWindow *, CGUI *);

float &GetSpeedFactor()
{
	return CEngine::sf;
}

float CEngine::frame_time = 0;
float CEngine::min_frame_time = 0;
float CEngine::speed_factor = 1.0f;
float CEngine::sf = 0;
CTimer CEngine::timer;

void CEngine::Run()
{
	Init();

	while(run)
		MainRefresh();
}

CEngine::CEngine()
{
	server = NULL;
	client = NULL;
	wnd = NULL;
	control = NULL;
	//rnd = NULL;
	rd = NULL;

	run = false;
	min_frame_time = -1; // unlimited
}

CEngine::~CEngine()
{
	Destruct();
}

void CEngine::Destruct()
{
	if(server)
	{
		delete server;
		server = NULL;
	}

	if(client)
	{
		delete client;
		client = NULL;
	}

	/*if(rnd)
	{
		delete rnd;
		rnd = NULL;
	}*/

	if(rd)
	{
		delete rd;
		rd = NULL;
	}

	if(control)
	{
		delete control;
		control = NULL;
	}

	if(wnd)
	{
		delete wnd;
		wnd = NULL;
	}
}

void CEngine::Init()
{
	wnd = new CWindow("HyperGameEngine", 0, 0, 800, 600, WS_OVERLAPPED, NULL);
	control = new CControl(wnd);
	rd = new CRenderDevice(wnd);

	if(!rd->Init(32, false))
		return;
	//rnd = CreateRender(rd, &server, wnd, &client->gui);
	//rnd->Init();
	control->Init();
	client = CreateClient(&server, rd, control, wnd, &timer);

	client->run = client->Init();
	if(!client->run)
		return;

	if(!timer.Init())
		return;

	time = timer.GetTime();

	run = true;
}

void CEngine::MainRefresh()
{
	frame_start = time;


	client->Update1(); // user input stuff
	if(server)
		server->Update(); // gamelogic, physics, network, ... stuff
	client->Update2(); // rendering stuff

	if(!client->run)
		run = false;

	time = timer.GetTime();
	frame_time = time - frame_start;

	// if there is a framerate limit
	if(min_frame_time != -1)
	{
		while(frame_time < min_frame_time)
		{
			time = timer.GetTime();
			frame_time = time - frame_start;
		}
	}

	sf = frame_time * speed_factor;
}