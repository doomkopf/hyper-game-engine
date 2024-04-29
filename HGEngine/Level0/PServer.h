#ifndef PSERVER_H_
#define PSERVER_H_

#include "thread.h"
#include <winsock2.h>
#include "Timer.h"
#include "../../stuff/FileBuf.h"
#include "../../stuff/FastQueue.h"

// MAX = 65507
#define PACKET_SIZE 65502
#define STREAMDATA_SIZE 65490

const short OK = -1;
const short CONNECT = -2;
const short CONNECT_CLOK = -3;
const short DISCONNECT = -4;
const short STREAMDATA = -5;
const short DYNDATA = -6;

const short STREAM_OK = -1000;
const short STREAM_FIRST_GET = -1001;
const short STREAM_FIRST_PUT = -1002;
const short STREAM_END = -1003;

#define TIMEOUT 5000

struct packet_header_t
{
	unsigned short cl_id;
	short id;
	char pnum; // stream number
};

struct packet_t
{
	unsigned short cl_id;
	short id;
	char pnum; // stream number
	char data[PACKET_SIZE];
};

struct streamdata_header_t
{
	short id;
	unsigned int len;
};

struct streamdata_t
{
	short id;
	unsigned int len;
	char data[STREAMDATA_SIZE];
};

void GetLocalAddress(unsigned char, char *);

class CNetObject
{
public:
	CNetObject();
	virtual ~CNetObject();

private:
	static WSADATA wsa;
	static int num_obj;
};

class CPSClient;

class CPServer : public CNetObject // Packet Server
{
	friend class CPSClient;
public:
	CPServer(CTimer *);
	virtual ~CPServer();
	bool Start(int, unsigned short, unsigned short, CPSClient **p_cl = NULL);
	void Shutdown();
	void SendToClients(void *, int);
	void SendPacketToClients(short, void *, int, char pnum = 0);
	void SendIDToClients(short);
	CPSClient **GetClientsPtr(int *);
	void AddPacketID(short);
	void AddPacketDataCount(unsigned short);
	void AddPacketData(void *, int);
	void AddPacket(short, void *, int);
	void AddPacketEnd();
	void SetPacketBufferStream(char);
	void SendPacketDataToClients();
	void ClearPacketData();

protected:
	static DWORD WINAPI RecThread(LPVOID);
	static DWORD WINAPI TimerThread(LPVOID);
	CPSClient *FindClient(const sockaddr_in &, unsigned short);
	bool PreConnectClient(const sockaddr_in &, unsigned short);
	void CompleteConnectClient(const sockaddr_in &, unsigned short);
	bool DisconnectClient(CPSClient *);
	void ProcessPacket0(const packet_t &, CPSClient *);
	void ProcessPacket1(const packet_t &, CPSClient *);
	unsigned short GenerateClientID();

	unsigned short idc;

	SOCKET s;
	sockaddr_in addr;
	HANDLE c_thread, t_thread;
	CPSClient **client;
	//int num_cl;

	unsigned short port1, port2;
	int max_cl;

	volatile bool c_thread_running, t_thread_running;
	unsigned int cur_time;

	packet_t tp;

	CTimer *timer;

	// packet buffering data
	packet_t dynpacket;
	int pi;
};

class CPeer
{
public:
	CPeer();
	~CPeer();
	bool SendFile(const char *, const in_addr &, unsigned short, bool);
	bool RecFile(const char *, const in_addr &, unsigned short, bool);
	void Disconnect();

	unsigned char stream;
	int packets;

protected:
	static DWORD WINAPI RecThread(LPVOID);
	static DWORD WINAPI SendThread(LPVOID);
	bool Accept(unsigned short);
	bool Connect(const in_addr &, unsigned short);
	void Send(void *, int);
	void SendPacket(void *, short, int);
	void SendID(short);

	HANDLE send_thread, rec_thread;
	volatile bool send_thread_running, rec_thread_running;

	//char filename[128];

	streamdata_t tp;

	bool requester;

	CFileBuf fb;

	SOCKET s, r;
	sockaddr_in addr, raddr;
};

class CPSClient
{
	friend class CPServer;
public:
	CPSClient();
	virtual ~CPSClient();
	void Send(void *, int);
	void SendPacket(short, void *, int, char pnum = 0);
	void SendID(short, char pnum = 0);
	void SendPacketBuffer();
	bool SendFile(const char *);
	bool DownloadFile(const char *);
	virtual void Disconnect();
	bool GetData0(packet_t *);
	bool GetData1(packet_t *);
	bool NewConnected();
	void CleanupStreamConnection();
	bool Streaming();
	bool StreamDone();
	int &GetPackets();

	bool connected, timed_out;
	unsigned short id;

protected:
	CPServer *server;
	sockaddr_in addr;

	packet_t packet01, packet0/*, packet1*/;
	CFastQueue<packet_t> p1buffer;
	bool has_new_data0, has_new_data1;
	bool locked0, locked1;

	unsigned char state;
	unsigned int last_send, last_rec;

	CPeer peer;
};

#endif