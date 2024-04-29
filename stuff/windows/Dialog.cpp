#include "Dialog.h"

CDialog::CDialog(WORD dlg_id, HWND parent)
{
	Create(dlg_id, parent);
}

CDialog::~CDialog()
{
	Close();
}

void CDialog::Create(WORD dlg_id, HWND parent)
{
	hwnd = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(dlg_id), parent, DialogProc);
	Show();

	created = true;
}

void CDialog::Close()
{
	DestroyWindow(hwnd);

	created = false;
}

void CDialog::Show()
{
	ShowWindow(hwnd, SW_SHOW);
}

void CDialog::Hide()
{
	ShowWindow(hwnd, SW_HIDE);
}

bool CDialog::EDITBOX_GetString(int id, char *str, int size)
{
	HWND h = GetDlgItem(hwnd, id);
	if(!h)
		return false;

	SendMessage(h, WM_GETTEXT, size, (LPARAM)str);

	return true;
}

bool CDialog::EDITBOX_SetString(int id, const char *str)
{
	HWND h = GetDlgItem(hwnd, id);
	if(!h)
		return false;

	SendMessage(h, WM_SETTEXT, 0, (LPARAM)str);

	return true;
}

bool CDialog::EDITBOX_AddString(int id, const char *str)
{
	HWND h = GetDlgItem(hwnd, id);
	if(!h)
		return false;

	int l = (int)SendMessage(h, WM_GETTEXTLENGTH, 0, 0) + 1;
	char *tmp = new char[l + strlen(str)];
	SendMessage(h, WM_GETTEXT, l, (LPARAM)tmp);
	strcat(tmp, str);
	SendMessage(h, WM_SETTEXT, 0, (LPARAM)tmp);
	delete [] tmp;

	return true;
}

int CDialog::LISTBOX_GetSelIndex(int id)
{
	HWND h = GetDlgItem(hwnd, id);
	if(!h)
		return -1;

	return (int)SendMessage(h, LB_GETCURSEL, 0, 0);
}

void CDialog::LISTBOX_GetSelString(int id, char *str)
{
	HWND h = GetDlgItem(hwnd, id);
	if(!h)
		return;

	SendMessage(h, LB_GETTEXT, LISTBOX_GetSelIndex(id), (LPARAM)str);
}

void CDialog::LISTBOX_AddText(int id, const char *text)
{
	HWND h = GetDlgItem(hwnd, id);
	SendMessage(h, LB_ADDSTRING, strlen(text), (LPARAM)text);
}

void CDialog::LISTBOX_Clear(int id)
{
	HWND h = GetDlgItem(hwnd, id);
	SendMessage(h, LB_RESETCONTENT, 0, 0);
}

void CDialog::Update()
{
	MSG msg;

	while(PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
	{
		switch(msg.message)
		{
		case WM_COMMAND:
			switch(LOWORD(msg.wParam))
			{
			case IDCANCEL:
				Close();
				return;
			}
			ProcessCommand(msg.wParam, msg.lParam);
			return;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

INT_PTR CALLBACK CDialog::DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_COMMAND:
		if(LOWORD(wParam) != IDCANCEL)
			PostMessage(hwnd, msg, wParam, lParam);
		break;
	}

	return 0;
}