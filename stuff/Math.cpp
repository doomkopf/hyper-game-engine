#include "stdafx.h"
#include "Math.h"

void swap(float *f1, float *f2)
{
	float t;

	t = *f1;
	*f1 = *f2;
	*f2 = t;
}

void abs_f(float *f)
{
	if(*f < 0)
		*f = -*f;
}

float abs_f(float f)
{
	return f < 0 ? -f : f;
}

float DotProduct(const CVector3f &v1, const CVector3f &v2)
{
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

float DotProduct(float v1x, float v1y, float v1z, float v2x, float v2y, float v2z)
{
	return (v1x * v2x) + (v1y * v2y) + (v1z * v2z);
}

CVector3f CrossProduct(const CVector3f &v1, const CVector3f &v2)
{
	CVector3f v;

	v.x = (v1.y * v2.z) - (v1.z * v2.y);
	v.y = (v1.z * v2.x) - (v1.x * v2.z);
	v.z = (v1.x * v2.y) - (v1.y * v2.x);

	return v;
}

bool GetIntersectionPoint_lineseg_plane(const CVector3f &p1, const CVector3f &p2, const CPlane &plane, CVector3f *intersection)
{
	float f1, f2;
	CVector3f d;

	f1 = DotProduct(plane.normal, p1) + plane.d;
	f2 = DotProduct(plane.normal, p2) + plane.d;

	if((f1 >= 0 && f2 >= 0) ||
		(f1 <= 0 && f2 <= 0))
		return false; // lineseg is not spanning the plane

	d = p2 - p1;
	d.Normalize();
	f1 = DotProduct(plane.normal, d);

	if(f1 == 0)
		return false; // lineseg is parallel to plane

	d *= -(DotProduct(plane.normal, p1) + plane.d) / f1;
	*intersection = p1 + d;

	return true;
}

void VectorRotate(CVector3f *v, const CMatrix3x3 &m)
{
	CVector3f t;

	t.x = v->x * m.m[0] + v->y * m.m[3] + v->z * m.m[6];
	t.y = v->x * m.m[1] + v->y * m.m[4] + v->z * m.m[7];
	t.z = v->x * m.m[2] + v->y * m.m[5] + v->z * m.m[8];

	*v = t;
}

void VectorRotateAxis(float angle, const CVector3f &axis, CVector3f *v, bool deg)
{
	CMatrix3x3 matrix;
	float c, s;

	if(deg)
	{
		c = COS(angle);
		s = SIN(angle);
	}
	else
	{
		c = cosf(angle);
		s = sinf(angle);
	}

	matrix.m[0] = (axis.x * axis.x) * (1.0f - c) + c;
	matrix.m[1] = (axis.y * axis.x) * (1.0f - c) + (axis.z * s);
	matrix.m[2] = (axis.z * axis.x) * (1.0f - c) - (axis.y * s);
	matrix.m[3] = (axis.x * axis.y) * (1.0f - c) - (axis.z * s);
	matrix.m[4] = (axis.y * axis.y) * (1.0f - c) + c;
	matrix.m[5] = (axis.z * axis.y) * (1.0f - c) + (axis.x * s);
	matrix.m[6] = (axis.x * axis.z) * (1.0f - c) + (axis.y * s);
	matrix.m[7] = (axis.y * axis.z) * (1.0f - c) - (axis.x * s);
	matrix.m[8] = (axis.z * axis.z) * (1.0f - c) + c;

	VectorRotate(v, matrix);
}

void VectorRotateX(float angle, CVector3f *v, bool deg)
{
	float bak = v->y;

	if(deg)
		angle *= PIOVER180;

	v->y = (v->y * cosf(angle)) - (v->z * sinf(angle));
	v->z = (v->z * cosf(angle)) + (bak * sinf(angle));
}

void VectorRotateY(float angle, CVector3f *v, bool deg)
{
	float bak = v->z;

	if(deg)
		angle *= PIOVER180;

	v->z = (v->z * cosf(angle)) - (v->x * sinf(angle));
	v->x = (v->x * cosf(angle)) + (bak * sinf(angle));
}

void VectorRotateZ(float angle, CVector3f *v, bool deg)
{
	float bak = v->x;

	if(deg)
		angle *= PIOVER180;

	v->x = (v->x * cosf(angle)) - (v->y * sinf(angle));
	v->y = (v->y * cosf(angle)) + (bak * sinf(angle));
}

bool RectIntersectsCircle(CVector2d rmin, CVector2d rmax, const CVector2d &pc, float r)
{
	rmin.x -= (int)r;
	rmin.y -= (int)r;
	rmax.x += (int)r;
	rmax.y += (int)r;

	return (pc.x >= rmin.x &&
		pc.y >= rmin.y &&
		pc.x <= rmax.x &&
		pc.y <= rmax.y);

	/*if(pc.x > rmin.x &&
		pc.y > rmin.y &&
		pc.x < rmax.x &&
		pc.y < rmax.y)
		return true;

	return false;*/
}