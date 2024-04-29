#include "HGClient.h"
#include "../Level0/Engine.h"
#include "str_ops.h"
#include "HGServer.h"

CHGClient::CHGClient(CServer **s, CRenderDevice *r, CControl *c, CWindow *aw, CTimer *t) : CClient(s, r, c, aw, t)
{
}

bool CHGClient::Init()
{
	if(!CClient::Init())
		return false;

	return true;
}

void CHGClient::Update1()
{
	CClient::Update1();

	if(srv)
	{
		if(!((CHGServer*)srv)->active)
		{
			delete srv;
			srv = NULL;
		}
	}
}

#define ROT_SPEED 0.2f

void CHGClient::UpdateInput1()
{
	CClient::UpdateInput1();

	if(control->hit[KEY_I])
		rnd->idle = true;

	if(control->key[KEY_MLEFT])
	{
		rnd->rd->cam.RotateX(-control->mouse_y * ROT_SPEED);
		rnd->rd->cam.RotateY(-control->mouse_x * ROT_SPEED);
	}

	if(control->hit[KEY_V])
		rnd->rd->SetViewport(0, 0, rnd->rd->bb_x, 500);

	control->key[KEY_MMIDDLE] ? rnd->rd->WIREFRAME_Enable() : rnd->rd->WIREFRAME_Disable();
}

void CHGClient::UpdateInput2()
{
	CClient::UpdateInput2();

	if(control->hit[KEY_I])
		rnd->idle = false;
}

#define ERROR_USAGE "usage: <key> <value>"

void CHGClient::ExecCommand(const char *cmd)
{
	static char key[MAX_CONSTRING];

	CClient::ExecCommand(cmd);

	GetKey(cmd, key);

	if(key[0] == 0)
		return;

	if(strcmp(key, "rnd_wireframe") == 0)
	{
		if(strlen(cmd) == 15)
		{
			if(cmd[14] == '1')
				rnd->rd->WIREFRAME_Enable();
			if(cmd[14] == '0')
				rnd->rd->WIREFRAME_Disable();
		}
		else
			rnd->con.Print(ERROR_USAGE, 255, 0, 0);
	}
	else if(strcmp(key, "eng_maxfps") == 0)
	{
		int v;

		if(GetValueINT(cmd, &v))
		{
			if(v == 0)
				min_ft = -1;
			else
				min_ft = (float)FPS(v);
		}
		else
			rnd->con.Print(ERROR_USAGE, 255, 0, 0);
	}
	else if(strcmp(key, "god") == 0)
	{
		rnd->con.Print("GOD-Mode ;-)");
	}
	else
	{
		//con.Print("Unknown Command");
		strcpy(key, "Unknown Command: ");
		strcat(key, cmd);
		rnd->con.Print(key);
	}
}