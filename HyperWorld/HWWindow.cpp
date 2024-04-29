#include "HWWindow.h"
#include "Serializing/ProjectExporter.h"
#include "Serializing/ProjectImporter.h"
#include "Serializing/HGExporter/HGExporter.h"

CHWWindow::CHWWindow(CHWTools *t, CHWWorld *w, const char *title, int x, int y, int cx, int cy, DWORD styles, HWND parent, WORD menu_id) : CWindow(title, x, y, cx, cy, styles, parent, menu_id)
{
	tools = t;
	world = w;
}

void CHWWindow::ProcessCommand(WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case ID_PROJECT_SAVE:
		export = new CProjectExporter;
		export->Export(*world->building, "test1.hwm");
		delete export;
		break;

	case ID_PROJECT_OPEN:
		import = new CProjectImporter(tools);
		import->Import(world->building, "test1.hwm");
		delete import;
		break;

	case ID_PROJECT_EXPORT_HGBUILDING:
		export = new CHGExporter;
		export->Export(*world->building, "test1.hgb");
		delete export;
		break;
	}
}