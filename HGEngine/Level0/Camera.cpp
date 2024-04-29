#include "Camera.h"

CQuaternion CCamera::quat;
CMatrix3x3 CCamera::m;

CCamera::CCamera() :
right((CVector3f&)omt.m[0]),
up((CVector3f&)omt.m[3]),
dir((CVector3f&)omt.m[6])
{
	pos.Set(0, 0, 0);
	right.Set(1, 0, 0);
	up.Set(0, 1, 0);
	dir.Set(0, 0, 1);
}

CCamera::~CCamera()
{
}

void CCamera::RotateX(float f)
{
	quat.AxisAngle(right, f);
	quat.GetMatrix(&m);

	VectorRotate(&up, m);
	VectorRotate(&dir, m);
}

void CCamera::RotateY(float f)
{
	quat.AxisAngle(CVector3f(0, 1, 0), f);
	quat.GetMatrix(&m);

	VectorRotate(&right, m);
	VectorRotate(&up, m);
	VectorRotate(&dir, m);
}

void CCamera::RotateZ(float f)
{
	quat.AxisAngle(CVector3f(0, 0, 1), f);
	quat.GetMatrix(&m);

	VectorRotate(&right, m);
	VectorRotate(&up, m);
	VectorRotate(&dir, m);
}

void CCamera::MoveFW(float f)
{
	pos += dir * f * sf;
}

void CCamera::MoveBW(float f)
{
	pos -= dir * f * sf;
}

void CCamera::MoveL(float f)
{
	pos -= right * f * sf;
}

void CCamera::MoveR(float f)
{
	pos += right * f * sf;
}

void CCamera::MoveUp(float f)
{
	pos += up * f * sf;
}

void CCamera::MoveDown(float f)
{
	pos -= up * f * sf;
}

void CCamera::CalcViewFrustum()
{
	CMatrix4x4 m;

	m.Multiply(mt_view, mt_perspective);

	frustum[0].normal.x = -(m.m[12] + m.m[0]);
	frustum[0].normal.y = -(m.m[13] + m.m[1]);
	frustum[0].normal.z = -(m.m[14] + m.m[2]);
	frustum[0].d        = -(m.m[15] + m.m[3]);

	frustum[1].normal.x = -(m.m[12] - m.m[0]);
	frustum[1].normal.y = -(m.m[13] - m.m[1]);
	frustum[1].normal.z = -(m.m[14] - m.m[2]);
	frustum[1].d        = -(m.m[15] - m.m[3]);

	frustum[2].normal.x = -(m.m[12] - m.m[4]);
	frustum[2].normal.y = -(m.m[13] - m.m[5]);
	frustum[2].normal.z = -(m.m[14] - m.m[6]);
	frustum[2].d        = -(m.m[15] - m.m[7]);

	frustum[3].normal.x = -(m.m[12] + m.m[4]);
	frustum[3].normal.y = -(m.m[13] + m.m[5]);
	frustum[3].normal.z = -(m.m[14] + m.m[6]);
	frustum[3].d        = -(m.m[15] + m.m[7]);

	frustum[4].normal.x = -(m.m[12] + m.m[8]);
	frustum[4].normal.y = -(m.m[13] + m.m[9]);
	frustum[4].normal.z = -(m.m[14] + m.m[10]);
	frustum[4].d        = -(m.m[15] + m.m[11]);

	frustum[5].normal.x = -(m.m[12] - m.m[8]);
	frustum[5].normal.y = -(m.m[13] - m.m[9]);
	frustum[5].normal.z = -(m.m[14] - m.m[10]);
	frustum[5].d        = -(m.m[15] - m.m[11]);

	// normalizing
	float scale;
	for(int i = 0; i < 6; i++)
	{
		scale = 1 / sqrtf(DotProduct(frustum[i].normal, frustum[i].normal));

		frustum[i].normal.x *= scale;
		frustum[i].normal.y *= scale;
		frustum[i].normal.z *= scale;
		frustum[i].d *= scale;
	}
}

bool CCamera::AABBInViewFrustum(const CVector3f &bbox_min, const CVector3f &bbox_max)
{
	static CVector3f box[8];
	int i;

	box[0] = bbox_min;
	box[1].Set(bbox_min.x, bbox_min.y, bbox_max.z);
	box[2].Set(bbox_min.x, bbox_max.y, bbox_max.z);
	box[3].Set(bbox_max.x, bbox_min.y, bbox_max.z);
	box[4].Set(bbox_max.x, bbox_max.y, bbox_min.z);
	box[5].Set(bbox_min.x, bbox_max.y, bbox_min.z);
	box[6].Set(bbox_max.x, bbox_min.y, bbox_min.z);
	box[7] = bbox_max;

	for(i = 0; i < 6; i++)
	{
		if(DotProduct(frustum[i].normal, box[0]) + frustum[i].d < 0)
			continue;

		if(DotProduct(frustum[i].normal, box[1]) + frustum[i].d < 0)
			continue;

		if(DotProduct(frustum[i].normal, box[2]) + frustum[i].d < 0)
			continue;

		if(DotProduct(frustum[i].normal, box[3]) + frustum[i].d < 0)
			continue;

		if(DotProduct(frustum[i].normal, box[4]) + frustum[i].d < 0)
			continue;

		if(DotProduct(frustum[i].normal, box[5]) + frustum[i].d < 0)
			continue;

		if(DotProduct(frustum[i].normal, box[6]) + frustum[i].d < 0)
			continue;

		if(DotProduct(frustum[i].normal, box[7]) + frustum[i].d < 0)
			continue;

		return false;
	}

	return true;
}

void CCamera::UpdateViewMatrix()
{
	mt_view.m[0] = right.x;           mt_view.m[4] = up.x;           mt_view.m[8] = dir.x;            mt_view.m[12] = 0;//-DotProduct(pos, right);
	mt_view.m[1] = right.y;           mt_view.m[5] = up.y;           mt_view.m[9] = dir.y;            mt_view.m[13] = 0;//-DotProduct(pos, up);
	mt_view.m[2] = right.z;           mt_view.m[6] = up.z;           mt_view.m[10] = dir.z;           mt_view.m[14] = 0;// DotProduct(pos, dir);
	mt_view.m[3] = -DotProduct(pos, right); mt_view.m[7] = -DotProduct(pos, up); mt_view.m[11] = -DotProduct(pos, dir); mt_view.m[15] = 1;
}