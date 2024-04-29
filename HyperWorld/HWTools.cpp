#include "HWTools.h"
#include "../stuff/FileCrawler.h"

CHWTools::CHWTools(WORD dlg_id, HWND parent, CHWWorld *w) : CDialog(dlg_id, parent)
{
	world = w;
	bt = BT_POLYHEDRON;
}

bool CHWTools::LoadTexturesDefaultPath()
{
	const char *def_tex_path = "textures/";
	char path[80], file[80];
	strcpy(path, def_tex_path);
	strcat(path, "*.*");
	CFileCrawler fc(path);

	while(fc.NextFile(file))
	{
		strcpy(path, def_tex_path);
		strcat(path, file);
		if(!world->textures->LoadFromFile(path))
			return false;
		LISTBOX_AddText(IDC_LISTTEXTURES, path);
	}

	return true;
}

bool CHWTools::LoadTexture(const char *file)
{
	if(!world->textures->LoadFromFile(file))
		return false;

	LISTBOX_AddText(IDC_LISTTEXTURES, file);

	return true;
}

bool CHWTools::ProcessCommand(WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case IDC_BTNTEXLOADDEF:
		LoadTexturesDefaultPath();
		break;

	case IDC_BTNTEXAPPLY:
		world->building->ApplyTextureToBrushes();
		break;

	case IDC_BTNTEXDEFAULT:
		world->building->DefaultTexCoords();
		break;

	case IDC_LISTTEXTURES:
		switch(HIWORD(wParam))
		{
		case LBN_SELCHANGE:
			world->textures->cur_tex = LISTBOX_GetSelIndex(IDC_LISTTEXTURES);
			break;
		}
		break;

	case IDC_BTNCSGSUBTRACT:
		world->building->CSG_Subtract();
		break;

	default:
		return false;
	}

	return true;
}