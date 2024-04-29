#include "HGServer.h"

CHGServer::CHGServer() : map(&oh), oh(&map)
{
	CAvatar::mh = &mh;
	active = false;
}

CHGServer::~CHGServer()
{
	Destruct();
}

bool CHGServer::Init()
{
	if(!CServer::Init())
		return false;

	active = true;
	return true;
}

void CHGServer::Update()
{
	oh.Update();
}

#define SPEED 800.0f
#define ROT_SPEED 0.002f

void CHGServer::ProcessClientInput1(CControl *control)
{
	if(control->key[KEY_W])
		oh.player.MoveFW(SPEED);
	if(control->key[KEY_A])
		oh.player.MoveL(SPEED);
	if(control->key[KEY_D])
		oh.player.MoveR(SPEED);
	if(control->key[KEY_S])
		oh.player.MoveBW(SPEED);
	if(control->hit[KEY_SPACE])
		oh.player.Jump();

	oh.player.RotateX(-control->mouse_y * ROT_SPEED);
	oh.player.RotateY(-control->mouse_x * ROT_SPEED);

	oh.player.noclip = control->key[KEY_MRIGHT];
}

void CHGServer::ProcessClientInput2(CControl *control, const CVector2d &m, int bbx, int bby)
{
	CServer::ProcessClientInput2(control, m, bbx, bby);
}

void CHGServer::Destruct()
{
	CServer::Destruct();
}