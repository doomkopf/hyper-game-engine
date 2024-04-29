#ifndef HWIMPORTER_H_
#define HWIMPORTER_H_

#include "../HWWorld.h"

class IHWImporter
{
public:
	virtual bool Import(CHWWorld *, const char *) = 0;
	virtual bool Import(CHWBuilding *, const char *) = 0;
};

#endif