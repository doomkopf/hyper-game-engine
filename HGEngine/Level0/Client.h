#ifndef CLIENT_H_
#define CLIENT_H_

#include "../../stuff/windows/Window.h"
#include "Render.h"
#include "Server.h"
#include "Control.h"
#include "Console.h"
#include "GUI/GUI.h"
#include "Timer.h"

class CClient
{
public:
	CClient(CServer **, CRenderDevice *, CControl *, CWindow *, CTimer *);
	virtual ~CClient();
	virtual bool Init();
	virtual void Destruct();
	virtual void Update1();
	void Update2();
	virtual void ExecCommand(const char *);

	bool run;
	CWindow *wnd;
	CControl *control;
	CRender *rnd;
	CGUI gui;

protected:
	virtual void UpdateInput1();
	virtual void UpdateInput2();

	CServer *&srv;
	CTimer *timer;
	static float &min_ft;
};

#endif