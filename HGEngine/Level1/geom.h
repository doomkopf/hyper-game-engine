#include "../../stuff/Math.h"

#define FRONT 0
#define BACK 1
#define COPLANAR 2
#define COPLANAR_F 3
#define COPLANAR_B 4
#define SPANNING 5
#define SPANNING_FB 6
#define SPANNING_BF 7

bool Equal(float, float, float);
int ClassifyPoint(const CVector3f &, const CPlane &, float);
int ClassifyLineseg(const CVector3f &, const CVector3f &, const CPlane &, float);
float GetDistance_ray_point(const CVector3f &, const CVector3f &, const CVector3f &);