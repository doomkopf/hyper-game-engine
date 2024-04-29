#include "Window.h"

#define HW_SIZING 0x00010000

CWindow::CWindow(const char *title, int x, int y, int cx, int cy, DWORD styles, HWND parent, WORD menu_id) : cl_name("cl_name")
{
	converts[KEY_UP] = VK_UP;
	converts[KEY_DOWN] = VK_DOWN;
	converts[KEY_LEFT] = VK_LEFT;
	converts[KEY_RIGHT] = VK_RIGHT;
	converts[KEY_RETURN] = VK_RETURN;
	converts[KEY_RSHIFT] = VK_RSHIFT;
	converts[KEY_LSHIFT] = VK_LSHIFT;
	converts[KEY_RCONTROL] = VK_RCONTROL;
	converts[KEY_LCONTROL] = VK_LCONTROL;
	converts[KEY_SPACE] = VK_SPACE;
	converts[KEY_TAB] = VK_TAB;
	converts[KEY_BACKSPACE] = VK_BACK;
	converts[KEY_LALT] = VK_LMENU;
	converts[KEY_RALT] = VK_RMENU;
	converts[KEY_ESCAPE] = VK_ESCAPE;
	converts[KEY_SHIFT] = VK_SHIFT;
	converts[KEY_CONTROL] = VK_CONTROL;
	converts[KEY_ALT] = VK_MENU;
	converts[KEY_F1] = VK_F1;
	converts[KEY_F2] = VK_F2;
	converts[KEY_F3] = VK_F3;
	converts[KEY_F4] = VK_F4;
	converts[KEY_F5] = VK_F5;
	converts[KEY_F6] = VK_F6;
	converts[KEY_F7] = VK_F7;
	converts[KEY_F8] = VK_F8;
	converts[KEY_F9] = VK_F9;
	converts[KEY_F10] = VK_F10;
	converts[KEY_F11] = VK_F11;
	converts[KEY_F12] = VK_F12;
	converts[KEY_1] = '1';
	converts[KEY_2] = '2';
	converts[KEY_3] = '3';
	converts[KEY_4] = '4';
	converts[KEY_5] = '5';
	converts[KEY_6] = '6';
	converts[KEY_7] = '7';
	converts[KEY_8] = '8';
	converts[KEY_9] = '9';
	converts[KEY_0] = '0';
	converts[KEY_A] = 'A';
	converts[KEY_B] = 'B';
	converts[KEY_C] = 'C';
	converts[KEY_D] = 'D';
	converts[KEY_E] = 'E';
	converts[KEY_F] = 'F';
	converts[KEY_G] = 'G';
	converts[KEY_H] = 'H';
	converts[KEY_I] = 'I';
	converts[KEY_J] = 'J';
	converts[KEY_K] = 'K';
	converts[KEY_L] = 'L';
	converts[KEY_M] = 'M';
	converts[KEY_N] = 'N';
	converts[KEY_O] = 'O';
	converts[KEY_P] = 'P';
	converts[KEY_Q] = 'Q';
	converts[KEY_R] = 'R';
	converts[KEY_S] = 'S';
	converts[KEY_T] = 'T';
	converts[KEY_U] = 'U';
	converts[KEY_V] = 'V';
	converts[KEY_W] = 'W';
	converts[KEY_X] = 'X';
	converts[KEY_Y] = 'Y';
	converts[KEY_Z] = 'Z';
	converts[KEY_INSERT] = VK_INSERT;
	converts[KEY_DELETE] = VK_DELETE;
	converts[KEY_POS1] = VK_HOME;
	converts[KEY_END] = VK_END;
	converts[KEY_BUP] = VK_PRIOR;
	converts[KEY_BDOWN] = VK_NEXT;
	converts[KEY_BRACKET] = VK_OEM_102;
	converts[KEY_COMMA] = VK_SEPARATOR;//
	//converts[KEY_DOT] = VK_DOT;
	converts[KEY_PLUS] = VK_ADD;
	converts[KEY_MINUS] = VK_SUBTRACT;
	converts[KEY_STAR] = VK_MULTIPLY;
	//converts[KEY_SLASH] = VK_SLASH;

	for(int i = 0; i < NUM_KEYS; i++)
		key[i] = hit[i] = release[i] = false;

	mx = lmx = 0;
	my = lmy = 0;

	WNDCLASS wc;
	wc.style = 0;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = cl_name;

	RegisterClass(&wc);

	HMENU menu = menu_id > -1 ? LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(menu_id)) : NULL;

	hwnd = CreateWindow(
		cl_name,
		title,
		styles,
		x, y,
		cx, cy,
		parent,
		menu,
		GetModuleHandle(NULL),
		NULL
		);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	SetFocus();

	created = true;
}

CWindow::~CWindow()
{
	Close();
}

void CWindow::Close()
{
	DestroyWindow(hwnd);

	created = false;
}

void CWindow::GetClientSize(int *x, int *y)
{
	RECT r;

	GetClientRect(hwnd, &r);
	*x = r.right - r.left;
	*y = r.bottom - r.top;
}

void CWindow::GetWindowSize(int *x, int *y)
{
	RECT r;

	GetWindowRect(hwnd, &r);
	*x = r.right - r.left;
	*y = r.bottom - r.top;
}

bool CWindow::IsResized()
{
	if(resized)
	{
		resized = false;
		return true;
	}

	return resized;
}

void CWindow::SetFocus()
{
	::SetFocus(hwnd);
}

void CWindow::Update()
{
	MSG msg;

	while(PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
	{
		switch(msg.message)
		{
		case WM_QUIT:
			Close();
			break;

		case WM_KEYDOWN:
			KeyDown((UINT)msg.wParam);
			if(msg.wParam == VK_SHIFT)
				msg = msg;
			kdown[msg.wParam] = true;
			break;

		case WM_KEYUP:
			kdown[msg.wParam] = false;
			break;

		case WM_LBUTTONDOWN:
			lmx = mx = LOWORD(msg.lParam);
			lmy = my = HIWORD(msg.lParam);
			lm = true;
			break;

		case WM_LBUTTONUP:
			lm = false;
			break;

		case WM_RBUTTONDOWN:
			lmx = mx = LOWORD(msg.lParam);
			lmy = my = HIWORD(msg.lParam);
			rm = true;
			break;

		case WM_RBUTTONUP:
			rm = false;
			break;

		case WM_MBUTTONDOWN:
			mm = true;
			break;

		case WM_MBUTTONUP:
			mm = false;
			break;

		case WM_MOUSEMOVE:
			lmx = mx;
			lmy = my;
			mx = LOWORD(msg.lParam);
			my = HIWORD(msg.lParam);
			break;

		case HW_SIZING:
			resized = true;
			break;

		case WM_COMMAND:
			ProcessCommand(msg.wParam, msg.lParam);
			break;

		default:
			ProcessMessage(msg);
			break;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	TranslateInput();
}

void CWindow::TranslateInput()
{
	hit[KEY_MLEFT] = false;
	release[KEY_MLEFT] = false;
	if(lm)
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
	if(rm)
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
	if(mm)
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
		if(kdown[converts[i]])
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

LRESULT CALLBACK CWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_DESTROY:
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_SIZING:
	//case WM_SIZE:
		PostMessage(hwnd, HW_SIZING, wParam, lParam);
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}