#ifndef GAMEREALSERVER_H_
#define GAMEREALSERVER_H_

#include "../Level1/HGRealServer.h"

class CGameRealServer : public CHGRealServer
{
public:
	CGameRealServer(CTimer *, unsigned short);
	virtual bool Init();
	virtual void ProcessClientInput1(CControl *);
	virtual void ProcessClientInput2(CControl *, const CVector2d &, int, int);

protected:
};

#endif