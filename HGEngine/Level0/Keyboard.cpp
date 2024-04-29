#include "Keyboard.h"

bool CKeyboard::Init(HWND hwnd)
{
	if(!CDInputDevice::Init(hwnd))
		return false;

	dinput->CreateDevice(GUID_SysKeyboard, &dev, NULL);
	dev->SetDataFormat(&c_dfDIKeyboard);
	dev->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	dev->Acquire();

	return true;
}

void CKeyboard::GetState()
{
	dev->GetDeviceState(sizeof(key), key);
}