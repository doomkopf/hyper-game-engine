#ifndef GUI_H_
#define GUI_H_

#include "GUIWindow.h"
#include "../TextureManager.h"
#include "../Renderable.h"

class CGUI : public IRenderableText
{
public:
	CGUI();
	~CGUI();
	bool LoadTextures(const char *);
	void AddWindows(CGUIWindow *, int);
	void ProcessInput(CControl *, const CVector2d &, int, int);
	void Update();
	CGUIWindow *GetWindowByID(int);
	void Destruct();
	virtual void Render(CRenderDevice *, CText *);

	CGUIWindow *focus_window;

protected:
	void RenderWindow(CGUIWindow *, const CTextureManager &, CRenderDevice *, CText *);

	CGUIWindow **win;
	int num;
	CTextureManager tex_mgr;
};

#endif