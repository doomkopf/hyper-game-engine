#include "GameVirtualServer.h"

CGameVirtualServer::CGameVirtualServer(CTimer *t, const char *ip, unsigned short port) : CHGVirtualServer(t, ip, port)
{
}

bool CGameVirtualServer::Init()
{
	if(!CHGVirtualServer::Init())
		return false;

	if(!map.Load("../test1.hgb"))
		return false;

	CAnimatedMesh *a = new CMD2Mesh;
	if(!a->Load("../madpear"))
		MessageBox(NULL, "Error loading mesh", NULL, MB_OK);
	mh.a_meshes.Add(a);

	return true;
}