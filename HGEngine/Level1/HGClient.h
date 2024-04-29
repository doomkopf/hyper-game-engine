#ifndef HGCLIENT_H_
#define HGCLIENT_H_

#include "../Level0/Client.h"

class CHGClient : public CClient
{
public:
	CHGClient(CServer **, CRenderDevice *, CControl *, CWindow *, CTimer *);
	virtual bool Init();
	virtual void Update1();
	virtual void ExecCommand(const char *);

protected:
	virtual void UpdateInput1();
	virtual void UpdateInput2();
};

#endif