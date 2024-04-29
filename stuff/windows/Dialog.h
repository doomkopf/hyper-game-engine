#ifndef DIALOG_H_
#define DIALOG_H_

#include "../small_windows.h"

class CDialog
{
public:
	CDialog(WORD, HWND);
	virtual ~CDialog();
	void Create(WORD, HWND);
	virtual void Close();
	void Show();
	void Hide();
	void Update();
	bool EDITBOX_GetString(int, char *, int);
	bool EDITBOX_SetString(int, const char *);
	bool EDITBOX_AddString(int, const char *);
	int LISTBOX_GetSelIndex(int);
	void LISTBOX_GetSelString(int, char *);
	void LISTBOX_AddText(int, const char *);
	void LISTBOX_Clear(int);

	HWND hwnd;
	bool created;

protected:
	static INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
	virtual bool ProcessCommand(WPARAM, LPARAM) = 0;
};

#endif