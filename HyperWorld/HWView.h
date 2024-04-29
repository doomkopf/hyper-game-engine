#ifndef HWVIEW_H_
#define HWVIEW_H_

#include "../HGEngine/Level0/Renderable.h"
#include "../HGEngine/Level0/PObject.h"
#include "HWWindow.h"
#include "HWWorld.h"

class CHWView : public IRenderable2D3D, public CPObject
{
public:
	CHWView(CHWWorld *, CHWTools *);
	virtual void ProcessInput(CHWWindow *);
	virtual void Render3D(CRenderDevice *rd);

protected:
	bool Project3DTo2D(const CVector3f &, CVector2d *);
	void Project2DTo3D(const CVector2d &, CVector3f *, CVector3f *);
	void SetToGrid(CVector3f *);

	CHWWorld *world;
	CHWTools *tools;

	CVector3f pos;
	int gridsize;
	bool gridlock;

	CVector2d mpos, mpos_down;
	bool mdown;

	//viewport stuff
	CVector2d vp_min, vp_max;
	CMatrix4x4 matrix_view, matrix_projection;

	// brush movement stuff
	static CVector3f tmp_mov;
};

#endif