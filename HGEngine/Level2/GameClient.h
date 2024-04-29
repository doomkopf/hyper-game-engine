#ifndef GAMECLIENT_H_
#define GAMECLIENT_H_

#include "../Level1/HGClient.h"

class CGameClient : public CHGClient
{
public:
	CGameClient(CServer **, CRenderDevice *, CControl *, CWindow *, CTimer *);
	virtual bool Init();

protected:
	void InitGUI();
	virtual void Update1();
	virtual void UpdateInput1();
	virtual void UpdateInput2();
};

#endif