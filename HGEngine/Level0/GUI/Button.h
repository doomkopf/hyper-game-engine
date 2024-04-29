#ifndef BUTTON_H_
#define BUTTON_H_

#include "../../../stuff/Mathtypes.h"
#include "messages.h"

class CButton
{
	friend class CGUIWindow;
public:
	CButton();
	bool Clicked();
	bool MouseOver();

	int tex;
	CVector2f min, max;
	char *text;

protected:
	unsigned long flags;
};

#endif