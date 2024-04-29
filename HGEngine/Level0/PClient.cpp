#include "PClient.h"

CPClient::CPClient(CTimer *t) : p1buffer(128)
{
	connected = false;
	has_new_data0 = false;
	has_new_data1 = false;
	rec_thread_running = false;
	t_thread_running = false;
	locked0 = false;
	locked1 = false;
	timer = t;
	id = 0;
	pi = 0;
	dynpacket.id = DYNDATA;
}

CPClient::~CPClient()
{
	Disconnect();
}

bool CPClient::Connect(const char *host, unsigned short port1, unsigned short port2)
{
	peer_port = port2;

	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(s == INVALID_SOCKET)
	{
		Disconnect();
		return false;
	}

	HOSTENT *hs = gethostbyname(host);
	if(!hs)
		return false;
	remote.sin_family = AF_INET;
	remote.sin_port = htons(port1);
	remote.sin_addr = *((in_addr*)hs->h_addr_list[0]);
	if(connect(s, (sockaddr*)&remote, sizeof(remote)) == SOCKET_ERROR)
	{
		Disconnect();
		return false;
	}

	rec_thread = CreateThread(&RecThread, this);
	if(!rec_thread)
		return false;
	t_thread = CreateThread(&TimerThread, this);
	if(!t_thread)
		return false;
	while(!rec_thread_running);
	while(!t_thread_running);

	SendID(CONNECT, -1);

	unsigned int start = timer->GetTimeInt();
	do
	{
		if(timer->GetTimeInt() - start >= TIMEOUT)
		{
			Disconnect();
			return false;
		}
	}
	while(!connected);

	srv_timed_out = false;

	return true;
}

void CPClient::Disconnect()
{
	connected = false;
	/*if(rec_thread_running)
	{
		TerminateThread(rec_thread);
		rec_thread_running = false;
	}
	t_thread_running = false;*/

	peer.Disconnect();

	SendID(DISCONNECT, -1);

	if(rec_thread_running)
	{
		TerminateThread(rec_thread);
		rec_thread_running = false;
	}
	if(t_thread_running)
	{
		TerminateThread(t_thread);
		t_thread_running = false;
	}

	has_new_data0 = false;
	has_new_data1 = false;
	locked0 = false;
	locked1 = false;
	id = 0;

	shutdown(s, SD_BOTH);
	closesocket(s);
}

void CPClient::Send(void *d, int size)
{
	//send(s, (char*)d, size, 0);
	sendto(s, (char*)d, size, 0, (sockaddr*)&remote, sizeof(remote));
	last_send = cur_time;
}

void CPClient::SendPacket(short p_id, void *d, int size, char pnum)
{
	tp.cl_id = id;
	tp.id = p_id;
	tp.pnum = pnum;
	memcpy(tp.data, d, size);

	Send(&tp, size + sizeof(packet_header_t));
}

void CPClient::SendID(short p_id, char pnum)
{
	tp.cl_id = id;
	tp.id = p_id;
	tp.pnum = pnum;

	Send(&tp, sizeof(packet_header_t));
}

bool CPClient::GetData0(packet_t *p)
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

bool CPClient::GetData1(packet_t *p)
{
	if(has_new_data1)
	{
		while(locked1);
		locked1 = true;

		//*p = packet1;
		p1buffer.Get(p);
		has_new_data1 = false;

		locked1 = false;

		return true;
	}

	return has_new_data1;
}

DWORD WINAPI CPClient::TimerThread(LPVOID params)
{
	CPClient *client = (CPClient*)params;

	client->t_thread_running = true;

	while(client->t_thread_running)
	{
		client->cur_time = client->timer->GetTimeInt();

		if(client->connected)
		{
			if(client->cur_time - client->last_send >= TIMEOUT / 2)
				client->SendID(OK);
			if(client->cur_time - client->last_send >= TIMEOUT)
				client->srv_timed_out = true;

			if(client->peer.stream == 2)
			{
				client->peer.Disconnect();
			}
		}
		Sleep(100);
	}

	return 0;
}

DWORD WINAPI CPClient::RecThread(LPVOID params)
{
	CPClient *client = (CPClient*)params;
	packet_t packet;
	int addr_size = sizeof(sockaddr_in), ret;
	bool loop = true;

	client->rec_thread_running = true;

	while(loop)
	{
		//ret = recv(client->s, (char*)&lpacket, sizeof(packet_t), 0);
		ret = recvfrom(client->s, (char*)&packet, sizeof(packet), 0, (sockaddr*)&client->remote, &addr_size);
		if(ret == 0 || ret == SOCKET_ERROR)
			break;

		switch(packet.pnum)
		{
		case -1:
			switch(packet.id)
			{
			case CONNECT:
				if(client->id == 0)
				{
					client->id = packet.cl_id;
					client->connected = true;
					client->SendID(CONNECT_CLOK, -1);
				}
				break;

			case DISCONNECT:
				loop = false;
				break;
			}
			break;

		case 0:
			client->ProcessPacket0(packet);
			break;

		case 1:
			client->ProcessPacket1(packet);
			break;
		}
	}

	client->Disconnect();

	return 0;
}

void CPClient::ProcessPacket0(const packet_t &p)
{
	last_rec = cur_time;

	if(p.id == OK)
		return;

	while(locked0);
	locked0 = true;

	packet0 = p;
	has_new_data0 = true;

	locked0 = false;
}

void CPClient::ProcessPacket1(const packet_t &p)
{
	while(locked1);
	locked1 = true;

	//packet1 = p;
	p1buffer.Add(p);
	has_new_data1 = true;

	locked1 = false;
}

bool CPClient::DownloadFile(const char *src, const char *dst)
{
	streamdata_t hdr;

	hdr.id = STREAM_FIRST_GET;
	strcpy(hdr.data, src);

	peer.RecFile(dst, remote.sin_addr, peer_port, true);

	SendPacket(STREAMDATA, &hdr, sizeof(hdr));

	return true;
}

bool CPClient::SendFile(const char *src, const char *dst)
{
	streamdata_t hdr;

	hdr.id = STREAM_FIRST_PUT;
	strcpy(hdr.data, dst);

	peer.SendFile(src, remote.sin_addr, peer_port, true);

	SendPacket(STREAMDATA, &hdr, sizeof(hdr));

	return true;
}

void CPClient::CleanupStreamConnection()
{
	peer.Disconnect();
}

bool CPClient::Streaming()
{
	return peer.stream == 1;
}

bool CPClient::StreamDone()
{
	if(peer.stream == 2)
	{
		peer.stream = 0;
		return true;
	}

	return false;
}

int &CPClient::GetPackets()
{
	return peer.packets;
}

void CPClient::AddPacketID(short id)
{
	if(pi + sizeof(id) > PACKET_SIZE + 1)
		return;
	memcpy(&dynpacket.data[pi], &id, sizeof(id));
	pi += sizeof(id);
}

void CPClient::AddPacketDataCount(unsigned short count)
{
	if(pi + sizeof(count) > PACKET_SIZE + 1)
		return;
	memcpy(&dynpacket.data[pi], &count, sizeof(count));
	pi += sizeof(count);
}

void CPClient::AddPacketData(void *data, int len)
{
	if(pi + len > PACKET_SIZE + 1)
		return;
	memcpy(&dynpacket.data[pi], data, len);
	pi += len;
}

void CPClient::AddPacket(short id, void *data, int len)
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

void CPClient::AddPacketEnd()
{
	if(pi + sizeof(OK) > PACKET_SIZE + 1)
		return;
	memcpy(&dynpacket.data[pi], &OK, sizeof(OK));
	pi += sizeof(OK);
}

void CPClient::SetPacketBufferStream(char stream)
{
	dynpacket.pnum = stream;
}

void CPClient::SendPacketData()
{
	dynpacket.cl_id = id;
	Send(&dynpacket, sizeof(packet_header_t) + pi);
}

void CPClient::ClearPacketData()
{
	pi = 0;
}