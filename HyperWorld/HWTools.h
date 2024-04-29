#ifndef HWTOOLS_H_
#define HWTOOLS_H_

#include "../stuff/windows/Dialog.h"
#include "resource.h"
#include "HWWorld.h"

class CHWTools : public CDialog
{
public:
	CHWTools(WORD, HWND, CHWWorld *);
	bool LoadTexturesDefaultPath();
	bool LoadTexture(const char *);

	brush_type bt;

protected:
	virtual bool ProcessCommand(WPARAM, LPARAM);

	CHWWorld *world;
};

#endif