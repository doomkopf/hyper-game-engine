#ifndef HWBUILDING_H_
#define HWBUILDING_H_

#include "HWBrush.h"
#include "HWTextureManager.h"

class CHWBuilding
{
public:
	CHWBuilding(CHWTextureManager *);
	void ApplyTextureToBrushes();
	void DefaultTexCoords();
	void CSG_Subtract();
	void DeleteSelectedBrushes();
	void DuplicateSelectedBrushes(int);

	CList<CHWBrush> brushes;
	CHWTextureManager *textures;

protected:
	void ClipBrushToBrush(const CHWBrush &, const CHWBrush &, CList<CHWBrush> *);
};

#endif