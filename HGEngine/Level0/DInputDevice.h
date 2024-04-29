#ifndef DINPUTDEVICE_H_
#define DINPUTDEVICE_H_

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

class CDInputDevice
{
public:
	CDInputDevice();
	virtual ~CDInputDevice();
	virtual bool Init(HWND) = 0;
	virtual void GetState() = 0;

protected:
	static LPDIRECTINPUT8 dinput;
	LPDIRECTINPUTDEVICE8 dev;
};

#endif