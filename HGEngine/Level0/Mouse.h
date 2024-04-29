#ifndef MOUSE_H_
#define MOUSE_H_

#include "DInputDevice.h"

class CMouse : public CDInputDevice
{
public:
	virtual bool Init(HWND);
	virtual void GetState();

	DIMOUSESTATE mstate;
};

#endif