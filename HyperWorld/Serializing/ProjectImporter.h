#ifndef PROJECTIMPORTER_H_
#define PROJECTIMPORTER_H_

#include "HWImporter.h"
#include "../HWTools.h"

class CProjectImporter : public IHWImporter
{
public:
	CProjectImporter(CHWTools *);
	virtual bool Import(CHWWorld *, const char *);
	virtual bool Import(CHWBuilding *, const char *);

protected:
	CHWTools *tools;
};

#endif