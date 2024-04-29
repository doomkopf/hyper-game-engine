#include "Poly.h"

CPoly::CPoly()
{
	vertex = NULL;
	num_v = 0;
	tex = -1;
}

CPoly::~CPoly()
{
	Destruct();
}

void CPoly::Destruct()
{
	if(vertex)
	{
		delete [] vertex;
		vertex = NULL;
		num_v = 0;
	}
}