#ifndef BRUSHTYPES_H_
#define BRUSHTYPES_H_

#include "../../HWBrush.h"
#include "HWPoly.h"

class CTBrush
{
public:
	CTBrush();
	CTBrush(const CHWBrush &);

	CList<CHWPoly> polys;
};

class CClipBrush
{
public:
	CClipBrush(const CTBrush &);

	CList<CPlane> planes;

protected:
	void AddIfNotExists(const CPlane &);
};

#endif