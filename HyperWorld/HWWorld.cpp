#include "HWWorld.h"

CHWWorld::CHWWorld(CHWTextureManager *t)
{
	textures = t;

	building = new CHWBuilding(textures);
	buildings.Add(building);
}