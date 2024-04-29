#include "GameRealServer.h"
#include "../Level1/FreeObject.h"

CGameRealServer::CGameRealServer(CTimer *t, unsigned short port) : CHGRealServer(t, port)
{
}

bool CGameRealServer::Init()
{
	if(!CHGRealServer::Init())
		return false;

	if(!map.Load("../test1.hgb"))
		return false;

	CAnimatedMesh *a = new CMD2Mesh;
	if(!a->Load("../madpear"))
		MessageBox(NULL, "Error loading mesh", NULL, MB_OK);
	mh.a_meshes.Add(a);
	/*CAvatar *av = new CAvatar;
	av->pos.Set(600, 200, 0);
	av->mesh = a;
	av->SetAnimationState(RUN);
	oh.SpawnObject(av);*/

	oh.SpawnPlayer(CVector3f(500, 100, 0));

	//CStaticMesh *m = new CStaticMesh;
	/*m->CreateCylinder(CVector3f(0, 0, 0), CVector3f(0, 100, 0), 20, 16, 1, true);
	mh.AddMesh(m);*/

	//m = new CStaticMesh;
	//m->CreateSphere(CVector3f(0, 0, 0), 16, 16, 16);
	//m->CreateBlock(CVector3f(0, 0, 0), CVector3f(-16, -4, -8), CVector3f(16, 4, 8));
	//mh.AddMesh(m);

	/*box_t *box;
	CFreeObject *obj = new CFreeObject;
	obj->mesh = mh.GetStaticMesh("default_block");
	obj->pos.Set(700, 100, 100);
	box = new box_t;
	box->right = 16;
	box->up = 4;
	box->dir = 8;
	obj->cv = box;
	obj->cv_type = CV_BOX;
	oh.SpawnObject(obj);

	obj = new CFreeObject;
	obj->mesh = mh.GetStaticMesh("default_block");
	obj->pos.Set(0, 500, 0);
	box = new box_t;
	box->right = 16;
	box->up = 4;
	box->dir = 8;
	obj->cv = box;
	obj->cv_type = CV_BOX;
	oh.SpawnObject(obj);*/

	return true;
}

void CGameRealServer::ProcessClientInput1(CControl *con)
{
	CHGRealServer::ProcessClientInput1(con);
}

void CGameRealServer::ProcessClientInput2(CControl *con, const CVector2d &mpos, int bbx, int bby)
{
	CHGRealServer::ProcessClientInput2(con, mpos, bbx, bby);
}