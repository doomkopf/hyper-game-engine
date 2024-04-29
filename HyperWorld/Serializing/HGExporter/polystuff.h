#include "HWPoly.h"

int ClassifyPolygon(const CHWPoly &, const CPlane &, float);
bool MergePolys(const CHWPoly &, const CHWPoly &, CHWPoly *);
void OptimizePoly(CHWPoly *);