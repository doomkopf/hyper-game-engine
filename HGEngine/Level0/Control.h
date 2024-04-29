#ifndef CONTROL_H_
#define CONTROL_H_

#include "keys.h"
#include "../../stuff/windows/Window.h"
#include "Keyboard.h"
#include "Mouse.h"

class CControl
{
public:
	CControl(CWindow *);
	~CControl();
	bool Init();
	void Update();

	bool key[NUM_KEYS];
	bool hit[NUM_KEYS];
	bool release[NUM_KEYS];
	float mouse_x, mouse_y;
	unsigned char converts[NUM_KEYS]; // array to convert to direct input key codes

protected:
	CKeyboard keyboard;
	CMouse mouse;
	CWindow *wnd;
	//char keys[NUM_KEYS][32];
};

#endif