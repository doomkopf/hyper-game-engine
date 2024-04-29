#include "ProjectImporter.h"
#include "../../stuff/FileBuf.h"

CProjectImporter::CProjectImporter(CHWTools *t)
{
	tools = t;
}

bool CProjectImporter::Import(CHWWorld *world, const char *file)
{
	return true;
}

bool CProjectImporter::Import(CHWBuilding *building, const char *file)
{
	CFileBuf fb;
	char str[64];
	int c;
	fb.Open(file, true);

	fb.Read(&c, sizeof(int)); // number of brushes
	for(int i = 0; i < c; i++)
	{
		CHWBrush *brush = new CHWBrush;

		brush->selected = false;

		fb.Read(&brush->tex, sizeof(int));

		fb.Read(&brush->num_points, sizeof(int));
		brush->point = new CVector3f[brush->num_points];
		brush->point_selected = new bool[brush->num_points];
		for(int j = 0; j < brush->num_points; j++)
			brush->point_selected[j] = false;
		fb.Read(brush->point, sizeof(CVector3f) * brush->num_points);

		fb.Read(&brush->num_sides, sizeof(int));
		brush->side = new CHWBrushSide[brush->num_sides];
		fb.Read(brush->side, sizeof(CHWBrushSide) * brush->num_sides);

		building->brushes.Add(brush);
	}

	fb.Read(&c, sizeof(int));
	for(i = 0; i < c; i++)
	{
		fb.Read(str, 64);
		if(!tools->LoadTexture(str))
			return false;
	}

	return true;
}