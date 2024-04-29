#include "GameClient.h"
#include "GameRealServer.h"
#include "GameVirtualServer.h"

class CTestWindow : public CGUIWindow
{
public:
	CTestWindow()
	{
		id = 0;
		tex = 0;

		b[0].tex = 0;
		b[0].text = "Host Game";
		b[0].min.x = 0.1f;
		b[0].min.y = 0.1f;
		b[0].max.x = 0.35f;
		b[0].max.y = 0.15f;

		b[1].tex = 0;
		b[1].text = "Join Game";
		b[1].min.x = 0.5f;
		b[1].min.y = 0.1f;
		b[1].max.x = 0.75f;
		b[1].max.y = 0.15f;

		e[0].pos.x = 0.6f;
		e[0].pos.y = 0.2f;
		e[0].length = 0.2f;
		strcpy(e[0].text, "hostname");

		e[1].pos.x = 0.6f;
		e[1].pos.y = 0.3f;
		e[1].length = 0.2f;
	}

	virtual CButton *GetButtons(int *n)
	{
		*n = 2;
		return b;
	}

	virtual CEditBox *GetEditBoxes(int *n)
	{
		*n = 2;
		return e;
	}

	CButton b[2];
	CEditBox e[2];
};

CClient *CreateClient(CServer **s, CRenderDevice *r, CControl *c, CWindow *aw, CTimer *t)
{
	return new CGameClient(s, r, c, aw, t);
}

CGameClient::CGameClient(CServer **s, CRenderDevice *r, CControl *c, CWindow *aw, CTimer *t) : CHGClient(s, r, c, aw, t)
{
	timer = t;
}

bool CGameClient::Init()
{
	if(!CHGClient::Init())
		return false;

	InitGUI();
	rnd->idle = true;
	gui.GetWindowByID(0)->visible = true;

	return true;
}

void CGameClient::InitGUI()
{
	gui.LoadTextures("../textures/window.bmp");

	CGUIWindow *win = new CTestWindow;
	gui.AddWindows(win, 1);
}

void CGameClient::UpdateInput1()
{
	CHGClient::UpdateInput1();
}

void CGameClient::UpdateInput2()
{
	CHGClient::UpdateInput2();
}

void CGameClient::Update1()
{
	CHGClient::Update1();

	CGUIWindow *win = gui.GetWindowByID(0);
	int n;
	CButton *btn = win->GetButtons(&n);
	CEditBox *eb = win->GetEditBoxes(&n);
	if(win->visible)
	{
		if(btn[0].Clicked()) // host game
		{
			srv = new CGameRealServer(timer, 1234);
			if(!srv->Init())
			{
				delete srv;
				srv = NULL;
			}
			else
			{
				win->visible = false;
				rnd->idle = false;
			}
		}
		else if(btn[1].Clicked()) // join game
		{
			if(strlen(eb[0].text) > 0)
			{
				srv = new CGameVirtualServer(timer, eb[0].text, 1234);
				if(!srv->Init())
				{
					delete srv;
					srv = NULL;
					strcpy(eb[1].text, "Error");
				}
				else
				{
					win->visible = false;
					rnd->idle = false;
				}
			}
		}
	}
}