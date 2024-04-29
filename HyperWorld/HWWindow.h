#ifndef HWWINDOW_H_
#define HWWINDOW_H_

#include "../stuff/windows/Window.h"
#include "HWWorld.h"
#include "resource.h"
#include "Serializing/HWExporter.h"
#include "Serializing/HWImporter.h"
#include "HWTools.h"

class CHWWindow : public CWindow
{
public:
	CHWWindow(CHWTools *, CHWWorld *, const char *, int, int, int, int, DWORD, HWND, WORD menu_id = -1);
	virtual void ProcessCommand(WPARAM, LPARAM);

protected:
	CHWWorld *world;
	IHWExporter *export;
	IHWImporter *import;
	CHWTools *tools;
};

#endif