#include "DInputDevice.h"

LPDIRECTINPUT8 CDInputDevice::dinput = NULL;

CDInputDevice::CDInputDevice()
{
	dev = NULL;
}

bool CDInputDevice::Init(HWND hwnd)
{
	if(!dinput)
		DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, NULL);

	return true;
}

CDInputDevice::~CDInputDevice()
{
	if(dev)
	{
		dev->Unacquire();
		dev->Release();
		dev = NULL;
	}

	if(dinput)
	{
		dinput->Release();
		dinput = NULL;
	}
}