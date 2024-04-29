#ifndef RENDER_H_
#define RENDER_H_

#include "RenderDevice.h"
#include "Server.h"
#include "GUI/GUI.h"
#include "Console.h"
#include "Text.h"

class CRender
{
public:
	CRender(CRenderDevice *, CServer **, CWindow *, CGUI *);
	virtual ~CRender();
	virtual bool Init();
	virtual void Destruct();

	void RenderScene();

	CRenderDevice *rd;
	CText text;

	CConsole con;
	CVector2d mpos;
	bool idle;

protected:
	virtual void RenderServerStuff3D();
	virtual void RenderServerStuff2D();
	void RenderCursor();
	virtual void RenderClientStuff3D();
	virtual void RenderClientStuff2D();
	virtual void GetServerAttributes() {}

	CServer *&srv;
	CWindow *wnd;
	CGUI *gui;

	static CMatrix4x4 m4x4;

private:
	bool srv_active;
};

#endif