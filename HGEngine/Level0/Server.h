#ifndef SERVER_H_
#define SERVER_H_

#include "Control.h"
#include "GUI/GUI.h"

class CServer
{
public:
	CServer();
	virtual ~CServer();
	virtual bool Init();
	virtual void Destruct();
	virtual void Update() = 0;
	//virtual void *GetAttribute(const int &) = 0;
	virtual void ProcessClientInput1(CControl *) = 0;
	virtual void ProcessClientInput2(CControl *, const CVector2d &, int, int);

	CGUI gui;
	//bool run;

protected:
	/*
	in inherited classes declare a CPServer
	for the real server class or a CPClient
	for the virtual server class
	*/
};

#endif