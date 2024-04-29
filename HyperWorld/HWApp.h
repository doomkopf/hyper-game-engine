#ifndef HWAPP_H_
#define HWAPP_H_

#include "HWWorld.h"
#include "HWWindow.h"
#include "HWTools.h"
#include "../HGEngine/Level0/RenderDevice.h"
#include "HWRender.h"
#include "HWView.h"
#include "../HGEngine/Level0/Timer.h"
#include "HWTextureManager.h"

class CHWApp
{
public:
	CHWApp();
	~CHWApp();
	void Run();

	CHWWorld world;
	CHWView *vp[5];
	int v_idx;
	CHWWindow win;
	CHWTools tools;
	CHWTextureManager textures;

	static float frame_time, speed_factor, sf;

protected:
	bool Init();
	void MainRefresh();
	void ProcessInput();

	CRenderDevice *rd;
	CHWRender *rnd;
	CTimer timer;
	float frame_start, time;
};

#endif