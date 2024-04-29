#ifndef PCLIENT_H_
#define PCLIENT_H_

//ERROR: only works with no optimization compiler option
//has something to do with the this pointer(inheritation from netobject) given to the thread

//#include <winsock2.h>
#include "PServer.h"

class CPClient : public CNetObject // Packet Client
{
public:
	CPClient(CTimer *);
	virtual ~CPClient();
	bool Connect(const char *, unsigned short, unsigned short);
	void Disconnect();
	void Send(void *, int);
	void SendPacket(short, void *, int, char pnum = 0);
	void SendID(short, char pnum = 0);
	bool DownloadFile(const char *, const char *);
	bool SendFile(const char *, const char *);
	bool GetData0(packet_t *);
	bool GetData1(packet_t *);
	void CleanupStreamConnection();
	bool Streaming();
	bool StreamDone();
	int &GetPackets();
	void AddPacketID(short);
	void AddPacketDataCount(unsigned short);
	void AddPacketData(void *, int);
	void AddPacket(short, void *, int);
	void AddPacketEnd();
	void SetPacketBufferStream(char);
	void ClearPacketData();
	void SendPacketData();

	bool connected, srv_timed_out;
	unsigned short id;

protected:
	static DWORD WINAPI RecThread(LPVOID);
	static DWORD WINAPI TimerThread(LPVOID);
	void ProcessPacket0(const packet_t &);
	void ProcessPacket1(const packet_t &);

	SOCKET s;
	sockaddr_in remote;
	unsigned short peer_port;
	HANDLE rec_thread, t_thread;

	volatile bool rec_thread_running, t_thread_running;
	unsigned int cur_time, last_send, last_rec;
	packet_t packet01, packet0, /*packet1,*/ tp;
	CFastQueue<packet_t> p1buffer;
	bool has_new_data0, has_new_data1;
	bool locked0, locked1;

	CPeer peer;
	CTimer *timer;

	// packet buffering data
	packet_t dynpacket;
	int pi;
};

#endif