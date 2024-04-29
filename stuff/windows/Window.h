#ifndef WINDOW_H_
#define WINDOW_H_

#include "../small_windows.h"
#include "../../HGEngine/Level0/keys.h"

class CWindow
{
public:
	CWindow(const char *, int, int, int, int, DWORD, HWND, WORD menu_id = -1);
	virtual ~CWindow();
	virtual void Close();
	virtual void Update();
	void GetClientSize(int *, int *);
	void GetWindowSize(int *, int *);
	bool IsResized();
	void SetFocus();

	virtual void ProcessMessage(const MSG &) {}
	virtual void ProcessCommand(WPARAM, LPARAM) {}
	virtual void KeyDown(UINT) {}

	bool created;
	HWND hwnd;
	bool key[NUM_KEYS], hit[NUM_KEYS], release[NUM_KEYS];
	int mx, my, lmx, lmy;

protected:
	static LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
	void TranslateInput();

	const char *cl_name;
	unsigned char converts[256], kdown[256], lm, rm, mm;
	bool resized;
};

#endif