#ifndef HWVIEWTERRAIN_H_
#define HWVIEWTERRAIN_H_

#include "HWView.h"
#include "HWTextureManager.h"

class CHWViewTerrain : public CHWView
{
public:
	CHWViewTerrain(CHWWorld *, CHWTools *, CHWTextureManager *);
	virtual void ProcessInput(CHWWindow *);
	virtual void Render3D(CRenderDevice *);
	virtual void Render2D(CRenderDevice *);

protected:
	void AddDetail(int);
	void ShiftHeightmap(bool);
	void MoveFw();
	void MoveBw();
	void MoveL();
	void MoveR();
	void RotateX(float);
	void RotateY(float);
	void UpdateCamera(CCamera *);
	void RenderTriNodes(CTriNode *,
		const CVector2d &,
		const CVector2d &,
		const CVector2d &,
		const short *,
		int,
		CRenderDevice *);

	bool wireframe;

	CVector3f right, up, dir;
	float speed;
	float shift_radius, shift_step;
	static CQuaternion quat;
	static CMatrix3x3 m;

	CHWTextureManager *textures;
};

#endif