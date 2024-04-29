#ifndef GUIWINDOW_H_
#define GUIWINDOW_H_

#include "Button.h"
#include "EditBox.h"
#include "../Control.h"

class CGUIWindow
{
public:
	CGUIWindow();
	virtual ~CGUIWindow();
	virtual CButton *GetButtons(int *) = 0;
	virtual CEditBox *GetEditBoxes(int *) = 0;
	void ProcessInput(CControl *, const CVector2d &, int, int);
	virtual void Update();
	virtual void Destruct();

	int id, tex;
	bool visible;
	CVector2f min, max;
	CEditBox *cur_editbox;
};

#endif