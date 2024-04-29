#include "HWView.h"
#include "../stuff/proc.h"

CVector3f CHWView::tmp_mov;

CHWView::CHWView(CHWWorld *w, CHWTools *t)
{
	world = w;
	tools = t;

	gridsize = 32;
	gridlock = true;
	mdown = false;
}

void CHWView::ProcessInput(CHWWindow *win)
{
	mpos.x = win->mx;
	mpos.y = win->my;
	mdown = win->key[KEY_MLEFT];

	if(win->hit[KEY_MLEFT])
		mpos_down = mpos;
}

bool CHWView::Project3DTo2D(const CVector3f &point, CVector2d *point2d)
{
	D3DXVECTOR3 pd3d(point.x, point.y, point.z);
	D3DVIEWPORT9 vp;
	D3DXVECTOR3 pout;

	vp.X = vp_min.x;
	vp.Y = vp_min.y;
	vp.Width = vp_max.x;
	vp.Height = vp_max.y;
	vp.MinZ = 0;
	vp.MaxZ = 1;

	D3DXVec3Project(&pout, &pd3d, &vp, (D3DXMATRIX*)&matrix_projection, (D3DXMATRIX*)&matrix_view, NULL);

	point2d->x = (int)pout[0];
	point2d->y = (int)pout[1];

	return point2d->x >= 0 && point2d->x <= vp_max.x && point2d->y >= 0 && point2d->y <= vp_max.y;
}

void CHWView::Project2DTo3D(const CVector2d &point, CVector3f *pnear, CVector3f *pfar)
{
	D3DXVECTOR3 pd3d;
	D3DVIEWPORT9 vp;

	vp.X = vp_min.x;
	vp.Y = vp_min.y;
	vp.Width = vp_max.x;
	vp.Height = vp_max.y;
	vp.MinZ = 0;
	vp.MaxZ = 1;

	pd3d[0] = (float)point.x;
	pd3d[1] = (float)point.y;
	pd3d[2] = 0;

	D3DXVec3Unproject((D3DXVECTOR3*)pnear, &pd3d, &vp, (D3DXMATRIX*)&matrix_projection, (D3DXMATRIX*)&matrix_view, NULL);

	pd3d[2] = 1;

	D3DXVec3Unproject((D3DXVECTOR3*)pfar, &pd3d, &vp, (D3DXMATRIX*)&matrix_projection, (D3DXMATRIX*)&matrix_view, NULL);
}

void CHWView::SetToGrid(CVector3f *p)
{
	if(gridlock)
	{
		NormValueF(&p->x, gridsize);
		NormValueF(&p->y, gridsize);
		NormValueF(&p->z, gridsize);
	}
}

void CHWView::Render3D(CRenderDevice *rd)
{
	D3DVIEWPORT9 vp;
	rd->dev->GetViewport(&vp);

	vp_min.x = vp.X;
	vp_min.y = vp.Y;
	vp_max.x = vp.Width;
	vp_max.y = vp.Height;

	matrix_view = rd->cam.mt_view;
	matrix_view.Transpose(); // for D3DX order
	matrix_projection = rd->cam.mt_perspective;
	matrix_projection.Transpose();
}