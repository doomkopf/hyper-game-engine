#ifndef HGREALSERVER_H_
#define HGREALSERVER_H_

#include "HGServer.h"
#include "../Level0/PServer.h"

class CHGRealServer : public CHGServer
{
public:
	CHGRealServer(CTimer *, unsigned short);
	virtual void ProcessClientInput1(CControl *);
	virtual void Update();

protected:
	void SyncClients();
	void ParseData(char *, CPSClient *);
	void InitNewClient(int);
	void CheckForDisconnected();
	void CheckForTestHit(CPSClient *);

	CPServer srv;
};

#endif