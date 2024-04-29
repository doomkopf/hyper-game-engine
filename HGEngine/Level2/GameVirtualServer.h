#ifndef GAMEVIRTUALSERVER_H_
#define GAMEVIRTUALSERVER_H_

#include "../Level1/HGVirtualServer.h"

class CGameVirtualServer : public CHGVirtualServer
{
public:
	CGameVirtualServer(CTimer *, const char *, unsigned short);
	virtual bool Init();
};

#endif