#include "HWBuilding.h"
#include "../HGEngine/Level1/geom.h"
#include "../stuff/FileBuf.h"

CHWBuilding::CHWBuilding(CHWTextureManager *t)
{
	textures = t;
}

void CHWBuilding::ApplyTextureToBrushes()
{
	for(CNode<CHWBrush> *b_node = brushes.GFDN(); b_node; b_node = b_node->next)
	{
		CHWBrush *brush = b_node->GetData();
		if(brush->selected)
			brush->tex = textures->cur_tex;
	}
}

void CHWBuilding::DefaultTexCoords()
{
	for(CNode<CHWBrush> *b_node = brushes.GFDN(); b_node; b_node = b_node->next)
	{
		CHWBrush *brush = b_node->GetData();
		if(brush->selected)
			brush->CalcTexCoords();
	}
}

void CHWBuilding::DeleteSelectedBrushes()
{
dsb:
	for(CNode<CHWBrush> *b_node = brushes.GFDN(); b_node; b_node = b_node->next)
	{
		CHWBrush *brush = b_node->GetData();

		if(brush->selected)
		{
			b_node->Delete();
			goto dsb; // restart
		}
	}
}

void CHWBuilding::DuplicateSelectedBrushes(int gridsize)
{
	for(int i = 0, c = brushes.GetCount(); i < c; i++)
	{
		CHWBrush *brush = brushes.GetData(i);

		if(brush->selected)
		{
			CHWBrush *new_brush = new CHWBrush(*brush);
			for(int i = 0; i < new_brush->num_points; i++)
				new_brush->point[i] += CVector3f((float)gridsize, (float)gridsize, (float)gridsize);
			new_brush->CalcPlanes();
			brushes.Add(new_brush);
			brush->selected = false;
		}
	}
}

void CHWBuilding::CSG_Subtract()
{
csgsub:
	for(CNode<CHWBrush> *b_node1 = brushes.GFDN(); b_node1; b_node1 = b_node1->next)
	{
		CHWBrush *brush1 = b_node1->GetData();

		if(brush1->selected)
			continue;

		for(CNode<CHWBrush> *b_node2 = brushes.GFDN(); b_node2; b_node2 = b_node2->next)
		{
			CHWBrush *brush2 = b_node2->GetData();

			if(brush2->selected)
			{
				if(brush1->Intersects(*brush2))
				{
					// clipping not selected brush1 to selected brush2
					CList<CHWBrush> new_brushes;
					ClipBrushToBrush(*brush1, *brush2, &new_brushes);
					if(new_brushes.GetCount() > 0)
					{
						brushes.AddList(new_brushes);
						b_node1->Delete();
						goto csgsub;
					}
				}
			}
		}
	}
}

void CHWBuilding::ClipBrushToBrush(const CHWBrush &brush, const CHWBrush &clipper, CList<CHWBrush> *new_brushes)
{
	for(int i = 0; i < clipper.num_sides; i++)
	{
		//ClipBrushToPlane(brush, clipper.side[i].plane, 
	}
}