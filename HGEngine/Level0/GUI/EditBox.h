#ifndef EDITBOX_H_
#define EDITBOX_H_

#include "../../../stuff/Mathtypes.h"
#include "messages.h"

class CEditBox
{
	friend class CGUIWindow;
public:
	CEditBox();
	bool Clicked();
	bool MouseOver();

	const float height, char_width;
	CVector2f pos;
	float length;
	char text[64];

protected:
	unsigned long flags;
};

#endif