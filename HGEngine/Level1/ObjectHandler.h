#ifndef OBJECTHANDLER_H_
#define OBJECTHANDLER_H_

class CMap;
#include "Map.h"
#include "Object.h"
#include "Avatar.h"
#include "PhysicObject.h"
#include "../Level0/Renderable.h"

class CObjectHandler : public CPhysicObject, IRenderable
{
public:
	CObjectHandler(CMap *);
	~CObjectHandler();
	void Destruct();
	void SpawnPlayer(const CVector3f &);
	void SpawnObject(CObject *);
	void DeleteObject(CObject *);
	void DeleteObject(int);
	CObject *GetObject(int);
	void Update();
	virtual void Render(CRenderDevice *);

	CAvatar player;
	bool player_spawned, draw_player;
	CObject *object[128];
	const int MAX_OBJECTS;

protected:
	typedef void (*sp)(CPlane *, bool, void *);
	void ClipMovement(CObject *);
	void ClipObjects(CObject *, CObject *);
	void DPS(CObject *, CBSPNode *);
	bool ClipToBrush(CObject *, CPlane *, int, int);
	bool ClipToBrush_back(CObject *, CPlane *, int, int);
	bool CheckToBrush(CObject *, CPlane *, int, int);
	void DoResponse(CObject *, const CPlane &);
	static sp ShiftPlane;
	static void ShiftPlaneCylinder(CPlane *, bool, void *);
	static void ShiftPlaneSphere(CPlane *, bool, void *);
	static void ShiftPlaneBox(CPlane *, bool, void *);
	//void UpdatePhysics(CObject *);

	CMap *map;
};

#endif