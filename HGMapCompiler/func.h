#define FRONT 0
#define BACK 1
#define COPLANAR 2
#define SPANNING 3
#define SPANNING_FB 4
#define SPANNING_BF 5

#include "Poly.h"

bool Equal(const float &,const float &,const float &);
int ClassifyPoint(const CVector3f &,const CPlane &,const float &);
int ClassifyLineseg(const CVector3f &,const CVector3f &,const CPlane &,const float &);
int ClassifyPolygon(const CPoly &,const CPlane &,const float &);
void ClipPolygon(const CPoly &,const CPlane &,CPoly *,CPoly *);
//void ClipPolyToPoly(const CPoly &,const CPoly &,CList<CPoly> *,const float &);