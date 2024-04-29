#include "HGRealServer.h"
#include "packets.h"
#include "geom.h"

CHGRealServer::CHGRealServer(CTimer *t, unsigned short port) : srv(t)
{
	srv.Start(4, port, 23024);
}

void CHGRealServer::ProcessClientInput1(CControl *con)
{
	CHGServer::ProcessClientInput1(con);

	if(con->hit[KEY_MLEFT])
	{
		CheckForTestHit(NULL);
	}
}

void CHGRealServer::Update()
{
	CHGServer::Update();

	SyncClients();
}

void CHGRealServer::SyncClients()
{
	int num_cl;
	packet_t pk;
	CPSClient **cl = srv.GetClientsPtr(&num_cl);

	// process received data
	for(int i = 0; i < num_cl; i++)
	{
		if(cl[i]->connected)
		{
			if(cl[i]->NewConnected())
				InitNewClient(i);

			if(cl[i]->GetData0(&pk))
			{
				switch(pk.id)
				{
				case DYNDATA:
					ParseData(pk.data, cl[i]);
					break;
				}
			}
			if(cl[i]->GetData1(&pk))
			{
				switch(pk.id)
				{
				case DYNDATA:
					ParseData(pk.data, cl[i]);
					break;

				case HG_TESTHIT:
					CheckForTestHit(cl[i]);
					break;
				}
			}
		}
	}

	// send data
	for(i = 0; i < num_cl; i++)
	{
		for(int j = 0; j < oh.MAX_OBJECTS; j++)
		{
			if(oh.object[j])
			{
				if(oh.object[j]->id == cl[i]->id) // dont send the clients data to itself
					continue;

				// send state and id of object[j] to cl[i]
				srv.AddPacketID(HG_PLAYERSTATUS);
				srv.AddPacketData(&oh.object[j]->id, sizeof(int));
				srv.AddPacketData(&oh.object[j]->pos, sizeof(CVector3f));
				if(oh.object[j]->GetRotation())
					srv.AddPacketData(oh.object[j]->GetRotation(), sizeof(CMatrix3x3));
				srv.AddPacketData(&oh.object[j]->force, sizeof(CVector3f));
			}
		}
		srv.AddPacketID(HG_PLAYERSTATUS);
		int d = 0;
		srv.AddPacketData(&d, sizeof(int));
		srv.AddPacketData(&oh.player.pos, sizeof(CVector3f));
		if(oh.player.GetRotation())
			srv.AddPacketData(oh.player.GetRotation(), sizeof(CMatrix3x3));
		srv.AddPacketData(&oh.player.force, sizeof(CVector3f));
		srv.AddPacketEnd();
		srv.SetPacketBufferStream(0);
		cl[i]->SendPacketBuffer();
		srv.ClearPacketData();
	}

	CheckForDisconnected();
}

void CHGRealServer::InitNewClient(int cl_idx)
{
	int num_cl;
	CPSClient **cl = srv.GetClientsPtr(&num_cl);

	// send spawn position, and all other spawned objects
	CAvatar *av = new CAvatar;
	av->pos.Set(500, 100, 0); // tmp - later search for a spawn point in the map
	av->id = cl[cl_idx]->id;
	av->mesh = mh.GetAnimatedMesh("../madpear");
	av->SetAnimationState(RUN);
	av->UpdateAnimationState();
	srv.AddPacket(HG_SPAWNPOS, &av->pos, sizeof(av->pos));
	for(int i = 0; i < oh.MAX_OBJECTS; i++)
	{
		if(oh.object[i])
		{
			srv.AddPacketID(HG_NEWOBJECT);
			srv.AddPacketData(&oh.object[i]->id, sizeof(int));
			int d = oh.object[i]->GetType();
			srv.AddPacketData(&d, sizeof(d));
			srv.AddPacketData(&oh.object[i]->pos, sizeof(CVector3f));
			srv.AddPacketData(oh.object[i]->GetRotation(), sizeof(CMatrix3x3));
			srv.AddPacketData(&oh.object[i]->force, sizeof(CVector3f));
		}
	}
	srv.AddPacketID(HG_NEWOBJECT);
	int d = 0;
	srv.AddPacketData(&d, sizeof(int));
	d = oh.player.GetType();
	srv.AddPacketData(&d, sizeof(d));
	srv.AddPacketData(&oh.player.pos, sizeof(CVector3f));
	srv.AddPacketData(oh.player.GetRotation(), sizeof(CMatrix3x3));
	srv.AddPacketData(&oh.player.force, sizeof(CVector3f));
	srv.AddPacketEnd();
	srv.SetPacketBufferStream(1);
	cl[cl_idx]->SendPacketBuffer();
	srv.ClearPacketData();
	oh.SpawnObject(av);

	// send that there is a new object to all other clients
	srv.AddPacketID(HG_NEWOBJECT);
	srv.AddPacketData(&av->id, sizeof(int));
	d = av->GetType();
	srv.AddPacketData(&d, sizeof(d));
	srv.AddPacketData(&av->pos, sizeof(CVector3f));
	srv.AddPacketData(av->GetRotation(), sizeof(CMatrix3x3));
	srv.AddPacketData(&av->force, sizeof(CVector3f));

	srv.AddPacketEnd();
	srv.SetPacketBufferStream(1);
	for(i = 0; i < num_cl; i++)
	{
		if(cl[i]->connected && (i != cl_idx))
			cl[cl_idx]->SendPacketBuffer();
	}
	srv.ClearPacketData();
}

void CHGRealServer::ParseData(char *data, CPSClient *cl)
{
	short *pid;
	int idx = 0;

	do
	{
		pid = (short*)&data[idx];
		idx += sizeof(short);
		switch(*pid)
		{
		case HG_PLAYERSTATUS:
			CAvatar *av = (CAvatar*)oh.GetObject(cl->id);
			av->pos = av->dest = *((CVector3f*)&data[idx]);
			idx += sizeof(CVector3f);
			av->omt = *((CMatrix3x3*)&data[idx]);
			idx += sizeof(CMatrix3x3);
			av->force = *((CVector3f*)&data[idx]);
			idx += sizeof(CVector3f);
			break;
		}
	}
	while(*pid != OK);
}

void CHGRealServer::CheckForDisconnected()
{
	int num_cl;
	CPSClient **cl = srv.GetClientsPtr(&num_cl);

	for(int i = 0; i < oh.MAX_OBJECTS; i++)
	{
		if(oh.object[i])
		{
			for(int j = 0; j < num_cl; j++)
			{
				if(!cl[j]->connected && (oh.object[i]->id == cl[j]->id)) // the client owning this object has disconnected
				{
					// sending the information to all clients
					srv.SendPacketToClients(HG_DELETEOBJECT, &oh.object[i]->id, sizeof(int), 1);
					oh.DeleteObject(oh.object[i]);
					break;
				}
			}
		}
	}
}

void CHGRealServer::CheckForTestHit(CPSClient *cl)
{
	CAvatar *av = cl ? (CAvatar*)oh.GetObject(cl->id) : &oh.player;
	if(!av)
		return;

	for(int i = 0; i < oh.MAX_OBJECTS; i++)
	{
		if(oh.object[i])
		{
			if(oh.object[i] != av) // dont test the shooter against itself
			{
				if(GetDistance_ray_point(av->pos, av->dir, oh.object[i]->pos) < 100)
				{
					int num_cl;
					CPSClient **cl = srv.GetClientsPtr(&num_cl);
					for(int j = 0; j < num_cl; j++)
					{
						if(cl[j]->id == oh.object[i]->id)
						{
							cl[j]->SendPacket(HG_TESTHIT, &av->dir, sizeof(CVector3f));
							break;
						}
					}
				}
			}
		}
	}
}