#include "Control.h"
#include <stdio.h>

CControl::CControl(CWindow *w)
{
	wnd = w;

	/*strcpy(keys[KEY_UP], "k_up");
	strcpy(keys[KEY_DOWN], "k_down");
	strcpy(keys[KEY_LEFT], "k_left");
	strcpy(keys[KEY_RIGHT], "k_right");
	strcpy(keys[KEY_RETURN], "k_return");
	strcpy(keys[KEY_RSHIFT], "k_rshift");
	strcpy(keys[KEY_LSHIFT], "k_lshift");
	strcpy(keys[KEY_RCONTROL], "k_rcontrol");
	strcpy(keys[KEY_LCONTROL], "k_lcontrol");
	strcpy(keys[KEY_SPACE], "k_space");
	strcpy(keys[KEY_TAB], "k_tab");
	strcpy(keys[KEY_MLEFT], "k_mleft");
	strcpy(keys[KEY_MRIGHT], "k_mright");
	strcpy(keys[KEY_MMIDDLE], "k_mmiddle");
	strcpy(keys[KEY_MWUP], "k_mwup");
	strcpy(keys[KEY_MWDOWN], "k_mwdown");
	strcpy(keys[KEY_BACKSPACE], "k_backspace");
	strcpy(keys[KEY_LALT], "k_lalt");
	strcpy(keys[KEY_RALT], "k_ralt");*/
	//strcpy(keys[KEY_], "k_");

	converts[KEY_UP] = DIK_UP;
	converts[KEY_DOWN] = DIK_DOWN;
	converts[KEY_LEFT] = DIK_LEFT;
	converts[KEY_RIGHT] = DIK_RIGHT;
	converts[KEY_RETURN] = DIK_RETURN;
	converts[KEY_RSHIFT] = DIK_RSHIFT;
	converts[KEY_LSHIFT] = DIK_LSHIFT;
	converts[KEY_RCONTROL] = DIK_RCONTROL;
	converts[KEY_LCONTROL] = DIK_LCONTROL;
	converts[KEY_SPACE] = DIK_SPACE;
	converts[KEY_TAB] = DIK_TAB;
	converts[KEY_BACKSPACE] = DIK_BACK;
	converts[KEY_LALT] = DIK_LMENU;
	converts[KEY_RALT] = DIK_RMENU;
	converts[KEY_ESCAPE] = DIK_ESCAPE;
	converts[KEY_F1] = DIK_F1;
	converts[KEY_F2] = DIK_F2;
	converts[KEY_F3] = DIK_F3;
	converts[KEY_F4] = DIK_F4;
	converts[KEY_F5] = DIK_F5;
	converts[KEY_F6] = DIK_F6;
	converts[KEY_F7] = DIK_F7;
	converts[KEY_F8] = DIK_F8;
	converts[KEY_F9] = DIK_F9;
	converts[KEY_F10] = DIK_F10;
	converts[KEY_F11] = DIK_F11;
	converts[KEY_F12] = DIK_F12;
	converts[KEY_1] = DIK_1;
	converts[KEY_2] = DIK_2;
	converts[KEY_3] = DIK_3;
	converts[KEY_4] = DIK_4;
	converts[KEY_5] = DIK_5;
	converts[KEY_6] = DIK_6;
	converts[KEY_7] = DIK_7;
	converts[KEY_8] = DIK_8;
	converts[KEY_9] = DIK_9;
	converts[KEY_0] = DIK_0;
	converts[KEY_A] = DIK_A;
	converts[KEY_B] = DIK_B;
	converts[KEY_C] = DIK_C;
	converts[KEY_D] = DIK_D;
	converts[KEY_E] = DIK_E;
	converts[KEY_F] = DIK_F;
	converts[KEY_G] = DIK_G;
	converts[KEY_H] = DIK_H;
	converts[KEY_I] = DIK_I;
	converts[KEY_J] = DIK_J;
	converts[KEY_K] = DIK_K;
	converts[KEY_L] = DIK_L;
	converts[KEY_M] = DIK_M;
	converts[KEY_N] = DIK_N;
	converts[KEY_O] = DIK_O;
	converts[KEY_P] = DIK_P;
	converts[KEY_Q] = DIK_Q;
	converts[KEY_R] = DIK_R;
	converts[KEY_S] = DIK_S;
	converts[KEY_T] = DIK_T;
	converts[KEY_U] = DIK_U;
	converts[KEY_V] = DIK_V;
	converts[KEY_W] = DIK_W;
	converts[KEY_X] = DIK_X;
	converts[KEY_Y] = DIK_Y;
	converts[KEY_Z] = DIK_Z;
	converts[KEY_INSERT] = DIK_INSERT;
	converts[KEY_DELETE] = DIK_DELETE;
	converts[KEY_POS1] = DIK_HOME;
	converts[KEY_END] = DIK_END;
	converts[KEY_BUP] = DIK_PRIOR;
	converts[KEY_BDOWN] = DIK_NEXT;
	converts[KEY_BRACKET] = DIK_OEM_102;
	converts[KEY_COMMA] = DIK_COMMA;
	converts[KEY_DOT] = DIK_PERIOD;
	converts[KEY_PLUS] = DIK_ADD;
	converts[KEY_MINUS] = DIK_MINUS;
	converts[KEY_STAR] = DIK_MULTIPLY;
	converts[KEY_SLASH] = DIK_SLASH;
	//converts[KEY_] = DIK_;
}

CControl::~CControl()
{
	wnd = NULL;
}

bool CControl::Init()
{
	keyboard.Init(wnd->hwnd);
	mouse.Init(wnd->hwnd);

	mouse_x = mouse_y = 0;

	for(int i = 0; i < NUM_KEYS; i++)
		key[i] = hit[i] = release[i] = false;

	return true;
}

void CControl::Update()
{
	keyboard.GetState();
	mouse.GetState();

	// getting mouse movement
	mouse_x = float(mouse.mstate.lX);
	mouse_y = float(mouse.mstate.lY);

	// getting mouse keys
	hit[KEY_MLEFT] = false;
	release[KEY_MLEFT] = false;
	if(mouse.mstate.rgbButtons[0])
	{
		if(!key[KEY_MLEFT])
			hit[KEY_MLEFT] = true;
		key[KEY_MLEFT] = true;
	}
	else
	{
		if(key[KEY_MLEFT])
			release[KEY_MLEFT] = true;
		key[KEY_MLEFT] = false;
	}

	hit[KEY_MRIGHT] = false;
	release[KEY_MRIGHT] = false;
	if(mouse.mstate.rgbButtons[1])
	{
		if(!key[KEY_MRIGHT])
			hit[KEY_MRIGHT] = true;
		key[KEY_MRIGHT] = true;
	}
	else
	{
		if(key[KEY_MRIGHT])
			release[KEY_MRIGHT] = true;
		key[KEY_MRIGHT] = false;
	}

	hit[KEY_MMIDDLE] = false;
	release[KEY_MMIDDLE] = false;
	if(mouse.mstate.rgbButtons[2])
	{
		if(!key[KEY_MMIDDLE])
			hit[KEY_MMIDDLE] = true;
		key[KEY_MMIDDLE] = true;
	}
	else
	{
		if(key[KEY_MMIDDLE])
			release[KEY_MMIDDLE] = true;
		key[KEY_MMIDDLE] = false;
	}

	if(mouse.mstate.lZ > 0)
	{
		key[KEY_MWUP] = true;
	}
	else
	{
		key[KEY_MWUP] = false;
	}

	if(mouse.mstate.lZ < 0)
	{
		key[KEY_MWDOWN] = true;
	}
	else
	{
		key[KEY_MWDOWN] = false;
	}

	// getting keyboard keys
	for(int i = 0; i < NUM_KEYS; i++)
	{
		if(i == KEY_MLEFT ||
			i == KEY_MRIGHT ||
			i == KEY_MMIDDLE ||
			i == KEY_MWUP ||
			i == KEY_MWDOWN)
			continue;

		hit[i] = false;
		release[i] = false;
		if(keyboard.key[converts[i]])
		{
			if(!key[i])
				hit[i] = true;
			key[i] = true;
		}
		else
		{
			if(key[i])
				release[i] = true;
			key[i] = false;
		}
	}
}