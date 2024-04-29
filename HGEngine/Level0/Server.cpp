#include "Server.h"

CServer::CServer()
{
}

CServer::~CServer()
{
	Destruct();
}

bool CServer::Init()
{
	return true;
}

void CServer::Destruct()
{
}

void CServer::ProcessClientInput2(CControl *con, const CVector2d &m, int bbx, int bby)
{
	//gui.ProcessInput(con, m, bbx, bby);
}