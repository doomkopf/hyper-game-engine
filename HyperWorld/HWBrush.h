#ifndef HWBRUSH_H_
#define HWBRUSH_H_

#include "../stuff/Mathtypes.h"
#include "../stuff/List.h"
#include "../HGEngine/Level0/RenderDevice.h"

#ifndef NULL
#define NULL 0
#endif

#define EPSILON 0.001f

enum brush_type
{
	BT_POLYHEDRON,
	BT_BOX,
	BT_SPHERE
};

enum creationresult_t
{
	CRRES_NOTYET,
	CRRES_IMPOSSIBLE,
	CRRES_OK
};

class CHWBrushSide
{
public:
	int point[3];
	texcoord_t texcoord[3];
	CPlane plane;
};

class CHWBrush
{
public:
	CHWBrush();
	CHWBrush(const CHWBrush &);
	~CHWBrush();
	void DefConstructor();
	void Copy(const CHWBrush &);
	void Destruct();
	void CalcPlanes();
	void CalcTexCoords();
	bool CheckIfConvex() const;
	int ClassifySide(int, const CPlane &) const;
	bool Intersects(const CHWBrush &) const;
	void ClipToPlane(const CPlane &, CHWBrush *, CHWBrush *) const;
	creationresult_t CreateBlock(const CList<CVector3f> &, float, int);
	creationresult_t CreatePolyhedron(CList<CVector3f> &, float, int);

	CVector3f *point;
	bool *point_selected;
	int num_points;
	CHWBrushSide *side;
	int num_sides;
	int tex;

	bool selected;

protected:
	bool SideInSolid(int, const CHWBrush &, int) const;
	bool CheckDoublePoints(const CList<CVector3f> &, CNode<CVector3f> *, bool) const;
	bool CheckPointsConvex(const CList<CVector3f> &, bool, int) const;
};

#endif