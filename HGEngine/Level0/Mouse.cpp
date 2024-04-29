#include "Mouse.h"

bool CMouse::Init(HWND hwnd)
{
	if(!CDInputDevice::Init(hwnd))
		return false;

	dinput->CreateDevice(GUID_SysMouse, &dev, NULL);
	dev->SetDataFormat(&c_dfDIMouse);
	dev->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	dev->Acquire();

	return true;
}

void CMouse::GetState()
{
	dev->GetDeviceState(sizeof(mstate), &mstate);
}