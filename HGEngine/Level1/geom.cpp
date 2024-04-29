#include "geom.h"

bool Equal(float f1, float f2, float epsilon)
{
	return abs_f(f1 - f2) <= epsilon;
}

int ClassifyPoint(const CVector3f &p, const CPlane &plane, float epsilon)
{
	float d = DotProduct(p, plane.normal) + plane.d;

	if(d > epsilon)
		return FRONT;
	if(d < -epsilon)
		return BACK;

	return COPLANAR;
}

int ClassifyLineseg(const CVector3f &p1, const CVector3f &p2, const CPlane &plane, float epsilon)
{
	int d1, d2;

	d1 = ClassifyPoint(p1, plane, epsilon);
	d2 = ClassifyPoint(p2, plane, epsilon);

	if(d1 == FRONT && d2 == BACK)
		return SPANNING_FB;

	if(d1 == BACK && d2 == FRONT)
		return SPANNING_BF;

	if(((d1 == FRONT || d1 == COPLANAR) && d2 == FRONT) ||
		((d2 == FRONT || d2 == COPLANAR) && d1 == FRONT))
		return FRONT;

	if(d1 == COPLANAR && d2 == COPLANAR)
		return COPLANAR;

	return BACK;
}

float GetDistance_ray_point(const CVector3f &ray_orig, const CVector3f &ray_vec, const CVector3f &p)
{
	CVector3f v;
	CPlane plane;
	float dist;

	v = p - ray_orig;
	dist = v.Length();
	v.Normalize(dist);
	if(DotProduct(v, ray_vec) <= 0)
	{
		return dist;
	}

	v = CrossProduct(v, ray_vec);
	v.Normalize();

	plane.normal = CrossProduct(v, ray_vec);
	plane.d= -DotProduct(plane.normal, ray_orig);
	dist = DotProduct(plane.normal, p) + plane.d;

	return abs_f(dist);
}