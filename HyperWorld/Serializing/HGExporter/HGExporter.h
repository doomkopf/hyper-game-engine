#ifndef HGEXPORTER_H_
#define HGEXPORTER_H_

#include "../HWExporter.h"
#include "../../../stuff/FileBuf.h"
#include "BSPNode.h"
#include "BrushTypes.h"

class CHGExporter : public IHWExporter
{
public:
	virtual bool Export(const CHWWorld &, const char *);
	virtual bool Export(const CHWBuilding &, const char *);

protected:
	void ClipPolygon(const CHWPoly &, const CPlane &, CHWPoly *, CHWPoly *);
	void SaveBSP(CBSPNode *);
	void FilterUsedTextures(const CHWBuilding &, CList<CHWPoly> *);
	void SaveTextures(const CHWBuilding &, CBSPNode *);

	CList<CHWPoly> tpolys;
	CList<char*> ttextures;
	CList<CClipBrush> clip_brushes;
	CFileBuf fb;

	// CSG stuff
	CList<CHWPoly> *CSG_Union(const CList<CHWBrush> &);
	void ClipBrushToBrush(CTBrush *, const CHWBrush &);
	bool ClipPolyToBrush(CHWPoly *, const CHWBrush &, int, bool);

	// BSP stuff
	CBSPNode *BSP(CList<CHWPoly> *);
	void BSP(CBSPNode *);
	CHWPoly *FindBestSplitter(CBSPNode *);
};

#endif