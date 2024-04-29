#include "EditBox.h"

CEditBox::CEditBox() : height(0.025f), char_width(0.025f)
{
	flags = 0;
	text[0] = 0;
	length = 0.1f;
}

bool CEditBox::Clicked()
{
	if(flags & GUI_CLICK)
	{
		flags &= ~GUI_CLICK;
		return true;
	}

	return false;
}

bool CEditBox::MouseOver()
{
	return flags & GUI_MOUSEOVER ? true : false;
}