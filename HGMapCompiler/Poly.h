#ifndef POLY_H_
#define POLY_H_

#include "../stuff/Math.h"
#include "../stuff/List.h"

class CVertex
{
public:
	CVector3f p;
	float tu, tv;
};

class CPoly
{
public:
	CPoly();
	CPoly(const CPoly &);
	CPoly &operator=(const CPoly &);
	void Copy(const CPoly &);
	void CalcPlane();

	bool issplitter, bevel;
	CPlane plane;
	int tex;
	CList<CVertex> vertices;
};

#endif