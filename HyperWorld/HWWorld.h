#ifndef HWWORLD_H_
#define HWWORLD_H_

#include "HWBuilding.h"
#include "HWTerrain.h"

class CHWWorld
{
public:
	CHWWorld(CHWTextureManager *);

	CHWTerrain terrain;
	CList<CHWBuilding> buildings;
	CHWBuilding *building; // current editable building
	CHWTextureManager *textures;
};

#endif