#ifndef HGVIRTUALSERVER_H_
#define HGVIRTUALSERVER_H_

#include "HGServer.h"
#include "../Level0/PClient.h"

class CHGVirtualServer : public CHGServer
{
public:
	CHGVirtualServer(CTimer *, const char *, unsigned short);
	virtual bool Init();
	virtual void ProcessClientInput1(CControl *);
	virtual void Update();

protected:
	void SyncServer();
	void ParseData(char *);

	CPClient cl;
	char host[64];
	unsigned short port;
};

#endif