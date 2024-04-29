#include "ProjectExporter.h"
#include "../../stuff/FileBuf.h"
#include "../../stuff/proc.h"

bool CProjectExporter::Export(const CHWWorld &world, const char *file)
{
	return true;
}

bool CProjectExporter::Export(const CHWBuilding &building, const char *file)
{
	CFileBuf fb;
	int c;
	fb.Open(file, false);

	c = building.brushes.GetCount();
	fb.Write(&c, sizeof(int));
	for(CNode<CHWBrush> *b_node = building.brushes.GFDN(); b_node; b_node = b_node->next)
	{
		CHWBrush *brush = b_node->GetData();

		fb.Write(&brush->tex, sizeof(int));

		fb.Write(&brush->num_points, sizeof(int));
		fb.Write(brush->point, sizeof(CVector3f) * brush->num_points);

		fb.Write(&brush->num_sides, sizeof(int));
		fb.Write(brush->side, sizeof(CHWBrushSide) * brush->num_sides);
	}

	fb.Write(&building.textures->num_tex, sizeof(int));
	for(int i = 0; i < building.textures->num_tex; i++)
	{
		CleanStr(building.textures->name[i], 64);
		fb.Write(building.textures->name[i], 64);
	}

	return true;
}