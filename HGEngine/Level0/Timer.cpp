#include "Timer.h"

bool CTimer::Init()
{
	__int64 freq;

	if(!QueryPerformanceFrequency((LARGE_INTEGER*)&freq))
		return false;

	QueryPerformanceCounter((LARGE_INTEGER*)&start);

	resolution = 1.0 / freq;

	return true;
}

float CTimer::GetTime()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&count);

	return float((count - start) * resolution);
}

unsigned int CTimer::GetTimeInt()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&count);

	return unsigned int(((count - start) * resolution) * 1000);
}