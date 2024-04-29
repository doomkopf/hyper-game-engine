#ifndef MATHSTUFF_H_
#define MATHSTUFF_H_

#include "Mathtypes.h"

#ifndef PI
#define PI         3.1415926535897932384626433832795f
#endif
#define PIOVER180  0.0174532925199432957692369076848861f
#define PIUNDER180 57.2957795130823208767981548141052f

#define SIN(f) (sinf(f * PIOVER180))
#define COS(f) (cosf(f * PIOVER180))

#define DEG_TO_RAD(f) (f * PIOVER180)

#define ABS(x) (x < 0 ? -x : x)

void swap(float *, float *);
void abs_f(float *);
float abs_f(float);

float DotProduct(const CVector3f &, const CVector3f &);
float DotProduct(float, float, float, float, float, float);
CVector3f CrossProduct(const CVector3f &, const CVector3f &);
bool GetIntersectionPoint_lineseg_plane(const CVector3f &, const CVector3f &, const CPlane &, CVector3f *);
void VectorRotate(CVector3f *, const CMatrix3x3 &);
void VectorRotateAxis(float, const CVector3f &, CVector3f *, bool);
void VectorRotateX(float, CVector3f *, bool);
void VectorRotateY(float, CVector3f *, bool);
void VectorRotateZ(float, CVector3f *, bool);
bool RectIntersectsCircle(CVector2d, CVector2d, const CVector2d &, float);

#endif