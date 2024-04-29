#ifndef HWVIEW2D_H_
#define HWVIEW2D_H_

#include "HWView.h"
#include "../stuff/List.h"

class CHWView2D : public CHWView
{
public:
	CHWView2D(CHWWorld *, CHWTools *, int, int);
	virtual void ProcessInput(CHWWindow *);
	void Zoom(int);
	virtual void Render3D(CRenderDevice *);
	virtual void Render2D(CRenderDevice *);

protected:
	void Project2DOnGridPlane(const CVector2d &, CVector3f *);
	void SetBrushPoints();
	bool PointInSelectionRect(const CVector3f &);
	void SelectBrushes();
	void SelectBrushPoints();
	void MoveSelectedBrushes();
	void MoveSelectedBrushPoints();
	void RenderPoint(const CVector2f &, CRenderDevice *);
	void RenderGrid(CRenderDevice *);
	void RenderSelectionRect(CRenderDevice *);
	void RenderDrawPoints(CRenderDevice *);
	void RenderBrush(const CHWBrush &, CRenderDevice *);

	int cx, cy, cz;
	int move_speed, zoom_speed;

	CList<CVector3f> draw_points;
};

#endif