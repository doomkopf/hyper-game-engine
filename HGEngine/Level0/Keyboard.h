#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "DInputDevice.h"

class CKeyboard : public CDInputDevice
{
public:
	virtual bool Init(HWND);
	virtual void GetState();

	BYTE key[256];
};

#endif