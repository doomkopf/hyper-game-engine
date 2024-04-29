#include "HGVirtualServer.h"
#include "packets.h"

CHGVirtualServer::CHGVirtualServer(CTimer *t, const char *h, unsigned short prt) : cl(t)
{
	strcpy(host, h);
	port = prt;
}

bool CHGVirtualServer::Init()
{
	if(!CHGServer::Init())
		return false;

	if(!cl.Connect(host, port, 23024))
		return false;

	return true;
}

void CHGVirtualServer::ProcessClientInput1(CControl *con)
{
	CHGServer::ProcessClientInput1(con);

	if(con->hit[KEY_MLEFT])
	{
		cl.SendID(HG_TESTHIT, 1);
	}
}

void CHGVirtualServer::Update()
{
	CHGServer::Update();

	SyncServer();
}

void CHGVirtualServer::SyncServer()
{
	packet_t pk;

	// process reveived data
	if(cl.GetData0(&pk))
	{
		switch(pk.id)
		{
		case DYNDATA:
			ParseData(pk.data);
			break;
		}
	}
	if(cl.GetData1(&pk))
	{
		pk.id = pk.id;
		switch(pk.id)
		{
		case DYNDATA:
			ParseData(pk.data);
			break;

		case HG_DELETEOBJECT:
			oh.DeleteObject(*((int*)pk.data));
			break;

		case HG_TESTHIT:
			oh.player.force += *((CVector3f*)pk.data) * 384;
			break;
		}
	}
	if(!cl.connected)
	{
		// server disconnected
		active = false;
	}

	// send data
	// send current player state (position, force, ...)
	cl.AddPacketID(HG_PLAYERSTATUS);
	cl.AddPacketData(&oh.player.pos, sizeof(CVector3f));
	cl.AddPacketData(oh.player.GetRotation(), sizeof(CMatrix3x3));
	cl.AddPacketData(&oh.player.force, sizeof(CVector3f));
	cl.AddPacketEnd();
	cl.SetPacketBufferStream(0);
	cl.SendPacketData();
	cl.ClearPacketData();
}

void CHGVirtualServer::ParseData(char *data)
{
	short *pid;
	int idx = 0;

	do
	{
		pid = (short*)&data[idx];
		idx += sizeof(short);
		CAvatar *av;
		switch(*pid)
		{
		case HG_PLAYERSTATUS:
			av = (CAvatar*)oh.GetObject(*((int*)&data[idx]));
			idx += sizeof(int);
			if(av)
			av->pos = av->dest = *((CVector3f*)&data[idx]);
			idx += sizeof(CVector3f);
			if(av)
			av->omt = *((CMatrix3x3*)&data[idx]);
			idx += sizeof(CMatrix3x3);
			if(av)
			av->force = *((CVector3f*)&data[idx]);
			idx += sizeof(CVector3f);
			break;

		case HG_NEWOBJECT:
			av = new CAvatar;
			av->id = *((int*)&data[idx]);
			av->mesh = mh.GetAnimatedMesh("../madpear"); // temp
			av->SetAnimationState(RUN);
			av->UpdateAnimationState();
			idx += sizeof(int);
			idx += sizeof(int); // temp - jumping over object type
			av->pos = av->dest = *((CVector3f*)&data[idx]);
			idx += sizeof(CVector3f);
			av->omt = *((CMatrix3x3*)&data[idx]);
			idx += sizeof(CMatrix3x3);
			av->force = *((CVector3f*)&data[idx]);
			idx += sizeof(CVector3f);
			oh.SpawnObject(av);
			break;

		case HG_SPAWNPOS:
			oh.SpawnPlayer(*((CVector3f*)&data[idx]));
			idx += sizeof(CVector3f);
			break;
		}
	}
	while(*pid != OK);
}