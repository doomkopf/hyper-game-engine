#ifndef CAMERA_H_
#define CAMERA_H_

#include "PObject.h"
#include "../../stuff/Math.h"
#include <d3d9.h>

class CCamera : public CPObject
{
	friend class CRender;
public:
	CCamera();
	~CCamera();

	void RotateX(float);
	void RotateY(float);
	void RotateZ(float);
	void MoveFW(float);
	void MoveBW(float);
	void MoveL(float);
	void MoveR(float);
	void MoveUp(float);
	void MoveDown(float);

	bool AABBInViewFrustum(const CVector3f &, const CVector3f &);

	void CalcViewFrustum();
	void UpdateViewMatrix();

	CVector3f pos;
	CVector3f &right, &up, &dir;
	CMatrix3x3 omt;
	CMatrix4x4 mt_view, mt_perspective; // 3D
	CMatrix4x4 mt_view2d, mt_ortho; // 2D

protected:
	CPlane frustum[6];
	static CQuaternion quat;
	static CMatrix3x3 m;
};

#endif