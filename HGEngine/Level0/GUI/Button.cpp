#include "Button.h"

#ifndef NULL
#define NULL 0
#endif

CButton::CButton()
{
	flags = 0;
	tex = -1;
	text = NULL;
}

bool CButton::Clicked()
{
	if(flags & GUI_CLICK)
	{
		flags &= ~GUI_CLICK;
		return true;
	}

	return false;
}

bool CButton::MouseOver()
{
	return flags & GUI_MOUSEOVER ? true : false;
}