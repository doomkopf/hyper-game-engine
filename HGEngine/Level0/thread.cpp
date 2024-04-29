#include "thread.h"

HANDLE CreateThread(LPTHREAD_START_ROUTINE proc, LPVOID params, priority_t prio)
{
	DWORD d;
	//return CreateThread(NULL, 0, proc, params, 0, &d);
	HANDLE h = CreateThread(NULL, 0, proc, params, 0, &d);
	if(!h)
		return h;

	switch(prio)
	{
	case PRIO_LOW:
		SetThreadPriority(h, THREAD_PRIORITY_BELOW_NORMAL);
		break;
	}

	return h;
}

void TerminateThread(HANDLE hThread)
{
	TerminateThread(hThread, 0);
}