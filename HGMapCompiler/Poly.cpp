#include "stdafx.h"
#include "Poly.h"

CPoly::CPoly()
{
	issplitter = false;
	tex = -1;
	bevel = true;
}

CPoly::CPoly(const CPoly &poly)
{
	Copy(poly);
}

CPoly &CPoly::operator=(const CPoly &poly)
{
	Copy(poly);

	return *this;
}

void CPoly::Copy(const CPoly &poly)
{
	issplitter = poly.issplitter;
	bevel = poly.bevel;
	plane = poly.plane;
	tex = poly.tex;
	vertices = poly.vertices;
}

void CPoly::CalcPlane()
{
	plane.Create(
		vertices.GFDN()->GetData()->p,
		vertices.GFDN()->next->GetData()->p,
		vertices.GFDN()->next->next->GetData()->p);
}