#ifndef PROJECTEXPORTER_H_
#define PROJECTEXPORTER_H_

#include "HWExporter.h"

class CProjectExporter : public IHWExporter
{
public:
	virtual bool Export(const CHWWorld &, const char *);
	virtual bool Export(const CHWBuilding &, const char *);
};

#endif