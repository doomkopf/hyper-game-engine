// HGMapCompiler.h : Hauptheaderdatei f�r die HGMapCompiler-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"		// Hauptsymbole


// CHGMapCompilerApp:
// Siehe HGMapCompiler.cpp f�r die Implementierung dieser Klasse
//

class CHGMapCompilerApp : public CWinApp
{
public:
	CHGMapCompilerApp();

// �berschreibungen
	public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CHGMapCompilerApp theApp;
