#include "../../stuff/small_windows.h"

enum priority_t
{
	PRIO_NORMAL = 0,
	PRIO_LOW = 1
};

HANDLE CreateThread(LPTHREAD_START_ROUTINE, LPVOID, priority_t prio = PRIO_NORMAL);
void TerminateThread(HANDLE);