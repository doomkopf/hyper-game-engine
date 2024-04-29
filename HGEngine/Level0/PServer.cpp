#include "PServer.h"

void GetLocalAddress(unsigned char c, char *ip)
{
	char hn[64];
	hostent *hs;

	gethostname(hn, sizeof(hn));
	hs = gethostbyname(hn);

	if(c == 1)
	{
		strcpy(ip, hn);
		return;
	}

	strcpy(ip, inet_ntoa(*((in_addr*)hs->h_addr_list[0])));

	if(c == 2)
	{
		strcat(ip, " ");
		strcat(ip, hn);
	}
}

// -----------------------------------------------------------------------

int CNetObject::num_obj = 0;
WSADATA CNetObject::wsa;

CNetObject::CNetObject()
{
	if(num_obj < 1)
		WSAStartup(MAKEWORD(2, 0), &wsa);

	num_obj++;
}

CNetObject::~CNetObject()
{
	num_obj--;

	if(num_obj < 1)
		WSACleanup();
}

// -----------------------------------------------------------------------

CPServer::CPServer(CTimer *t)
{
	client = NULL;
	c_thread_running = false;
	t_thread_running = false;
	timer = t;
	pi = 0;
	dynpacket.id = DYNDATA;
}

CPServer::~CPServer()
{
	Shutdown();
}

bool CPServer::Start(int mcl, unsigned short p1, unsigned short p2, CPSClient **p_cl)
{
	int i;

	port1 = p1;
	port2 = p2;
	max_cl = mcl;

	idc = 1;

	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(s == INVALID_SOCKET)
	{
		Shutdown();
		return false;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port1);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(s, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		Shutdown();
		return false;
	}

	if(p_cl)
		client = p_cl;
	else
	{
		client = new CPSClient*[max_cl];
		for(i = 0; i < max_cl; i++)
			client[i] = new CPSClient;
	}

	for(i = 0; i < max_cl; i++)
		client[i]->server = this;

	c_thread = CreateThread(&RecThread, this);
	if(!c_thread)
		return false;
	t_thread = CreateThread(&TimerThread, this);
	if(!t_thread)
		return false;
	while(!c_thread_running);
	while(!t_thread_running);

	return true;
}

void CPServer::Shutdown()
{
	//TerminateThread(c_thread);
	c_thread_running = false;

	t_thread_running = false;
	TerminateThread(t_thread);

	if(client)
	{
		for(int i = 0; i < max_cl; i++)
			delete client[i];
		delete [] client;
		client = NULL;
	}

	shutdown(s, SD_BOTH);
	closesocket(s);
}

DWORD WINAPI CPServer::TimerThread(LPVOID params)
{
	CPServer *server = (CPServer*)params;

	server->t_thread_running = true;

	while(server->t_thread_running)
	{
		server->cur_time = server->timer->GetTimeInt();
		for(int i = 0; i < server->max_cl; i++)
		{
			if(server->client[i]->connected)
			{
				if(server->cur_time - server->client[i]->last_send >= TIMEOUT / 2)
					server->client[i]->SendID(OK);
				if(server->cur_time - server->client[i]->last_rec > TIMEOUT)
					server->client[i]->timed_out = true;

				if(server->client[i]->peer.stream == 2)
				{
					server->client[i]->peer.Disconnect();
				}
			}
		}
		Sleep(100);
	}

	return 0;
}

DWORD WINAPI CPServer::RecThread(LPVOID params)
{
	CPServer *server = (CPServer*)params;
	int addr_size = sizeof(sockaddr_in), ret;
	packet_t packet;
	sockaddr_in addr;

	server->c_thread_running = true;

	while(server->c_thread_running)
	{
		ret = recvfrom(server->s, (char*)&packet, sizeof(packet_t), 0, (sockaddr*)&addr, &addr_size);
		if(ret > 1 && ret <= sizeof(packet))
		{
			switch(packet.pnum)
			{
			case -1: // important internal communication data
				switch(packet.id)
				{
				case CONNECT: // client requesting connection
					server->PreConnectClient(addr, packet.cl_id);
					break;

				case CONNECT_CLOK: // client received my connection packet
					server->CompleteConnectClient(addr, packet.cl_id);
					break;

				case DISCONNECT: // client requesting disconnection
					server->DisconnectClient(server->FindClient(addr, packet.cl_id));
					break;
				}
				break;

			case 0:
				server->ProcessPacket0(packet, server->FindClient(addr, packet.cl_id));
				break;

			case 1:
				server->ProcessPacket1(packet, server->FindClient(addr, packet.cl_id));
				break;
			}
		}
	}

	return 0;
}

CPSClient *CPServer::FindClient(const sockaddr_in &addr, unsigned short id)
{
	for(int i = 0; i < max_cl; i++)
	{
		if(client[i]->connected &&
			(client[i]->addr.sin_addr.s_addr == addr.sin_addr.s_addr) &&
			(client[i]->id == id))
			return client[i];
	}

	return NULL;
}

unsigned short CPServer::GenerateClientID()
{
	return idc++;
}

bool CPServer::PreConnectClient(const sockaddr_in &cl_addr, unsigned short id)
{
	if(FindClient(cl_addr, id))
		return false;

	bool ok = false;

	for(int i = 0; i < max_cl; i++)
	{
		if(!client[i]->connected)
		{
			client[i]->last_rec = cur_time;
			client[i]->addr = cl_addr;
			client[i]->timed_out = false;
			client[i]->has_new_data0 = false;
			client[i]->has_new_data1 = false;
			client[i]->state = 1;
			client[i]->id = GenerateClientID();
			client[i]->SendPacket(CONNECT, &client[i]->id, sizeof(client[i]->id), -1);

			ok = true;
			break;
		}
	}

	return ok;
}

void CPServer::CompleteConnectClient(const sockaddr_in &cl_addr, unsigned short id)
{
	for(int i = 0; i < max_cl; i++)
	{
		if(client[i]->addr.sin_addr.s_addr == cl_addr.sin_addr.s_addr &&
			client[i]->id == id &&
			client[i]->state == 1)
		{
			client[i]->connected = true;
			client[i]->state = 2;
		}
	}
}

bool CPServer::DisconnectClient(CPSClient *cl)
{
	if(!cl)
		return false;

	cl->Disconnect();

	return true;
}

void CPServer::ProcessPacket0(const packet_t &packet, CPSClient *cl)
{
	if(!cl)
		return;

	cl->last_rec = cur_time;

	switch(packet.id)
	{
	case OK:
		return;

	case STREAMDATA:
		streamdata_t *shdr = (streamdata_t*)packet.data;
		switch(shdr->id)
		{
		case STREAM_FIRST_GET:
			cl->SendFile(shdr->data);
			break;

		case STREAM_FIRST_PUT:
			cl->DownloadFile(shdr->data);
			break;
		}
		return;
	}

	while(cl->locked0);
	cl->locked0 = true;

	cl->packet0 = packet;
	cl->has_new_data0 = true;

	cl->locked0 = false;
}

void CPServer::ProcessPacket1(const packet_t &packet, CPSClient *cl)
{
	if(!cl)
		return;

	while(cl->locked1);
	cl->locked1 = true;

	//cl->packet1 = packet;
	cl->p1buffer.Add(packet);
	cl->has_new_data1 = true;

	cl->locked1 = false;
}

void CPServer::SendToClients(void *d, int size)
{
	for(int i = 0; i < max_cl; i++)
	{
		if(client[i]->connected)
			client[i]->Send(d, size);
	}
}

void CPServer::SendPacketToClients(short p_id, void *d, int size, char pnum)
{
	for(int i = 0; i < max_cl; i++)
	{
		if(client[i]->connected)
		{
			client[i]->SendPacket(p_id, d, size, pnum);
		}
	}
}

void CPServer::SendIDToClients(short p_id)
{
	packet_header_t p;

	p.id = p_id;

	for(int i = 0; i < max_cl; i++)
	{
		if(client[i]->connected)
		{
			p.cl_id = client[i]->id;
			client[i]->Send(&p, sizeof(packet_header_t));
		}
	}
}

void CPServer::AddPacketID(short id)
{
	if(pi + sizeof(id) > PACKET_SIZE + 1)
		return;
	memcpy(&dynpacket.data[pi], &id, sizeof(id));
	pi += sizeof(id);
}

void CPServer::AddPacketDataCount(unsigned short count)
{
	if(pi + sizeof(count) > PACKET_SIZE + 1)
		return;
	memcpy(&dynpacket.data[pi], &count, sizeof(count));
	pi += sizeof(count);
}

void CPServer::AddPacketData(void *data, int len)
{
	if(pi + len > PACKET_SIZE + 1)
		return;
	memcpy(&dynpacket.data[pi], data, len);
	pi += len;
}

void CPServer::AddPacket(short id, void *data, int len)
{
	if(pi + sizeof(id) > PACKET_SIZE + 1)
		return;
	memcpy(&dynpacket.data[pi], &id, sizeof(id));
	pi += sizeof(id);

	if(pi + len > PACKET_SIZE + 1)
		return;
	memcpy(&dynpacket.data[pi], data, len);
	pi += len;
}

void CPServer::SetPacketBufferStream(char stream)
{
	dynpacket.pnum = stream;
}

void CPServer::AddPacketEnd()
{
	if(pi + sizeof(OK) > PACKET_SIZE + 1)
		return;
	memcpy(&dynpacket.data[pi], &OK, sizeof(OK));
	pi += sizeof(OK);
}

void CPServer::SendPacketDataToClients()
{
	for(int i = 0; i < max_cl; i++)
	{
		if(client[i]->connected)
		{
			client[i]->SendPacketBuffer();
		}
	}
}

void CPServer::ClearPacketData()
{
	pi = 0;
}

CPSClient **CPServer::GetClientsPtr(int *size)
{
	*size = max_cl;

	return client;
}

// ----------------------------------------------------------------------

CPSClient::CPSClient() : p1buffer(128)
{
	server = NULL;
	connected = false;
	has_new_data0 = false;
	has_new_data1 = false;
	timed_out = false;
	state = 0;
	locked0 = false;
	locked1 = false;
	id = 0;
}

CPSClient::~CPSClient()
{
	Disconnect();
}

void CPSClient::Send(void *d, int size)
{
	sendto(server->s, (char*)d, size, 0, (sockaddr*)&addr, sizeof(sockaddr));
	last_send = server->cur_time;
}

void CPSClient::SendPacket(short p_id, void *d, int size, char pnum)
{
	server->tp.cl_id = id;
	server->tp.id = p_id;
	server->tp.pnum = pnum;
	memcpy(server->tp.data, d, size);

	Send(&server->tp, size + sizeof(packet_header_t));
}

void CPSClient::SendID(short p_id, char pnum)
{
	server->tp.cl_id = id;
	server->tp.id = p_id;
	server->tp.pnum = pnum;

	Send(&server->tp, sizeof(packet_header_t));
}

void CPSClient::SendPacketBuffer()
{
	Send(&server->dynpacket, sizeof(packet_header_t) + server->pi);
}

void CPSClient::Disconnect()
{
	connected = false;
	peer.Disconnect();

	state = 0;
	locked0 = false;
	locked1 = false;
	//id = 0;

	SendID(DISCONNECT, -1);
}

bool CPSClient::GetData0(packet_t *p)
{
	if(has_new_data0)
	{
		while(locked0);
		locked0 = true;

		*p = packet0;
		has_new_data0 = false;

		locked0 = false;

		return true;
	}

	return has_new_data0;
}

bool CPSClient::GetData1(packet_t *p)
{
	if(has_new_data1)
	{
		while(locked1);
		locked1 = true;

		//*p = packet1;
		p1buffer.Get(p);
		if(p1buffer.count < 1)
			has_new_data1 = false;

		locked1 = false;

		return true;
	}

	return has_new_data1;
}

bool CPSClient::NewConnected()
{
	if(state == 2)
	{
		state = 0;
		return true;
	}

	return false;
}

bool CPSClient::SendFile(const char *file)
{
	peer.SendFile(file, addr.sin_addr, server->port2, false);

	return true;
}

bool CPSClient::DownloadFile(const char *file)
{
	peer.RecFile(file, addr.sin_addr, server->port2, false);

	return true;
}

void CPSClient::CleanupStreamConnection()
{
	peer.Disconnect();
}

bool CPSClient::Streaming()
{
	return peer.stream == 1;
}

bool CPSClient::StreamDone()
{
	if(peer.stream == 2)
	{
		peer.stream = 0;
		return true;
	}

	return false;
}

int &CPSClient::GetPackets()
{
	return peer.packets;
}

// ------------------------------------------------------------

CPeer::CPeer()
{
	send_thread_running = false;
	rec_thread_running = false;
	stream = 0;
	packets = 0;
}

CPeer::~CPeer()
{
	Disconnect();
}

bool CPeer::Connect(const in_addr &ip, unsigned short port)
{
	r = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(r == INVALID_SOCKET)
		return false;

	raddr.sin_family = AF_INET;
	raddr.sin_port = htons(port);
	raddr.sin_addr = ip;

	if(connect(r, (sockaddr*)&raddr, sizeof(raddr)) == SOCKET_ERROR)
	{
		Disconnect();
		return false;
	}

	return true;
}

bool CPeer::Accept(unsigned short port)
{
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s == INVALID_SOCKET)
		return false;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(s, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		Disconnect();
		return false;
	}

	if(listen(s, 5) == SOCKET_ERROR)
	{
		Disconnect();
		return false;
	}

	return true;
}

bool CPeer::SendFile(const char *file, const in_addr &ip, unsigned short port, bool r)
{
	requester = r;

	requester ? Accept(port) : Connect(ip, port);

	//strcpy(filename, file);
	if(!fb.Open(file, true, false))
		return false;

	packets = 0;
	stream = 1;

	send_thread = CreateThread(&SendThread, this);
	if(!send_thread)
		return false;
	while(!send_thread_running);

	return true;
}

bool CPeer::RecFile(const char *file, const in_addr &ip, unsigned short port, bool r)
{
	requester = r;

	requester ? Accept(port) : Connect(ip, port);

	//strcpy(filename, file);
	if(!fb.Open(file, false))
		return false;

	packets = 0;
	stream = 1;

	rec_thread = CreateThread(&RecThread, this);
	if(!rec_thread)
		return false;
	while(!rec_thread_running);

	return true;
}

DWORD WINAPI CPeer::SendThread(LPVOID params)
{
	CPeer *peer = (CPeer*)params;
	streamdata_t sd;
	int len = sizeof(sockaddr);

	peer->send_thread_running = true;

	if(peer->requester)
		peer->r = accept(peer->s, (sockaddr*)&peer->raddr, &len);

	sd.id = STREAM_OK;

	while(!peer->fb._EOF())
	{
		recv(peer->r, (char*)&sd.id, sizeof(sd.id), 0);
		sd.len = peer->fb.Read(sd.data, STREAMDATA_SIZE);
		peer->Send(&sd, sizeof(streamdata_header_t) + sd.len);
		peer->packets++;
	}

	recv(peer->r, (char*)&sd.id, sizeof(sd.id), 0);
	peer->SendID(STREAM_END);

	peer->fb.Close();
	peer->send_thread_running = false;
	peer->stream = 2;

	return 0;
}

DWORD WINAPI CPeer::RecThread(LPVOID params)
{
	CPeer *peer = (CPeer*)params;
	streamdata_t sd;
	int rec, len = sizeof(sockaddr);
	bool loop = true;

	peer->rec_thread_running = true;

	if(peer->requester)
		peer->r = accept(peer->s, (sockaddr*)&peer->raddr, &len);

	while(loop)
	{
		peer->SendID(STREAM_OK);
		rec = recv(peer->r, (char*)&sd, sizeof(sd), 0);
		if(rec == 0 || rec == SOCKET_ERROR)
			break;

		peer->packets++;

		switch(sd.id)
		{
		case STREAM_OK:
			peer->fb.Write(sd.data, sd.len);
			break;

		case STREAM_END:
			loop = false;
			break;
		}
	}

	peer->fb.Close();
	peer->rec_thread_running = false;
	peer->stream = 2;

	return 0;
}

void CPeer::Send(void *data, int size)
{
	send(r, (char*)data, size, 0);
}

void CPeer::SendPacket(void *data, short id, int size)
{
	tp.id = id;
	tp.len = size;
	memcpy(tp.data, data, size);
	Send(&tp, size + sizeof(streamdata_header_t));
}

void CPeer::SendID(short id)
{
	tp.id = id;
	Send(&tp.id, sizeof(tp.id));
}

void CPeer::Disconnect()
{
	shutdown(s, SD_BOTH);
	shutdown(r, SD_BOTH);
	closesocket(s);
	closesocket(r);

	TerminateThread(send_thread);
	TerminateThread(rec_thread);

	send_thread_running = false;
	rec_thread_running = false;
}