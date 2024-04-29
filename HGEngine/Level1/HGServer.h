#ifndef HGSERVER_H_
#define HGSERVER_H_

#include "../Level0/Server.h"
#include "Map.h"
#include "ObjectHandler.h"
#include "MeshHandler.h"

class CHGServer : public CServer
{
public:
	CHGServer();
	virtual ~CHGServer();
	virtual bool Init();
	virtual void Update();
	virtual void Destruct();
	virtual void ProcessClientInput1(CControl *);
	virtual void ProcessClientInput2(CControl *, const CVector2d &, int, int);

	CMap map;
	CObjectHandler oh;
	CMeshHandler mh;
	bool active;
};

#endif