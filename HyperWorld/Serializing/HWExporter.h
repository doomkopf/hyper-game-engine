#ifndef HWEXPORTER_H_
#define HWEXPORTER_H_

#include "../HWWorld.h"

class IHWExporter
{
public:
	virtual bool Export(const CHWWorld &, const char *) = 0;
	virtual bool Export(const CHWBuilding &, const char *) = 0;
};

#endif