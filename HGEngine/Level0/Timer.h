#ifndef TIMER_H_
#define TIMER_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class CTimer
{
public:
	bool Init();
	float GetTime();
	unsigned int GetTimeInt();

protected:
	__int64 count, start;
	double resolution;
};

#endif