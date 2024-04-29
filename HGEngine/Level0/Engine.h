#ifndef ENGINE_H_
#define ENGINE_H_

#include "Server.h"
#include "Client.h"
#include "Timer.h"

#define FPS(x) (1.0f / x)

class CEngine
{
public:
	void Run();
	CEngine();
	~CEngine();
	void Destruct();

	static float frame_time, min_frame_time, speed_factor, sf;
	static CTimer timer;

protected:
	void Init();
	void MainRefresh();

	bool run;
	CServer *server;
	CClient *client;

	CWindow *wnd;
	//CRender *rnd;
	CRenderDevice *rd;
	CControl *control;

	float frame_start, time;
};

#endif