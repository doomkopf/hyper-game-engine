#ifndef HWVIEW3D_H_
#define HWVIEW3D_H_

#include "HWView.h"
#include "HWTextureManager.h"

class CHWView3D : public CHWView
{
public:
	CHWView3D(CHWWorld *, CHWTools *, CHWTextureManager *);
	virtual void ProcessInput(CHWWindow *);
	virtual void Render3D(CRenderDevice *);
	virtual void Render2D(CRenderDevice *);

protected:
	void MoveFw();
	void MoveBw();
	void MoveL();
	void MoveR();
	void RotateX(float);
	void RotateY(float);
	void UpdateCamera(CCamera *);
	void SelectBrushes();
	void MoveSelectedBrushes();
	void UpdateTmpMovPoint();
	void RenderBrush(const CHWBrush &, CRenderDevice *);

	CVector3f right, up, dir;
	CPlane projection_plane;
	float speed;
	CHWTextureManager *textures;
	static CQuaternion quat;
	static CMatrix3x3 m;
};

#endif