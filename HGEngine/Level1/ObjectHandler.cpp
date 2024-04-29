#include "ObjectHandler.h"
#include "geom.h"
#include "../Level0/Engine.h"

#define EPSILON 0.03125f

inline bool ez(float f) // ez = equals zero
{
	return (f <= EPSILON && f >= -EPSILON);
}

CObjectHandler::sp CObjectHandler::ShiftPlane = NULL;

CObjectHandler::CObjectHandler(CMap *m) : MAX_OBJECTS(128)
{
	map = m;
	player_spawned = false;
	draw_player = false;

	for(int i = 0; i < MAX_OBJECTS; i++)
		object[i] = NULL;
}

CObjectHandler::~CObjectHandler()
{
	Destruct();
}

void CObjectHandler::SpawnPlayer(const CVector3f &pos)
{
	player.pos = player.dest = pos;
	player_spawned = true;
}

void CObjectHandler::SpawnObject(CObject *obj)
{
	for(int i = 0; i < MAX_OBJECTS; i++)
	{
		if(!object[i])
		{
			obj->dest = obj->pos;
			object[i] = obj;
			break;
		}
	}
}

void CObjectHandler::DeleteObject(CObject *obj)
{
	for(int i = 0; i < MAX_OBJECTS; i++)
	{
		if(object[i] == obj)
		{
			delete object[i];
			object[i] = NULL;
		}
	}
}

void CObjectHandler::DeleteObject(int id)
{
	for(int i = 0; i < MAX_OBJECTS; i++)
	{
		if(object[i])
		{
			if(object[i]->id == id)
			{
				delete object[i];
				object[i] = NULL;
			}
		}
	}
}

CObject *CObjectHandler::GetObject(int id)
{
	for(int i = 0; i < MAX_OBJECTS; i++)
	{
		if(object[i])
		{
			if(object[i]->id == id)
				return object[i];
		}
	}

	return NULL;
}

void CObjectHandler::Update()
{
	//UpdatePhysics(&player);
	player.Update();
	ClipMovement(&player);
	player.pos = player.dest;

	for(int i = 0; i < MAX_OBJECTS; i++)
	{
		if(object[i])
		{
			object[i]->Update();
			ClipMovement(object[i]);
			object[i]->pos = object[i]->dest;
		}
	}
}

void CObjectHandler::ClipMovement(CObject *obj)
{
	if(!obj->noclip)
	{
		// objects
		for(int i = 0; i < MAX_OBJECTS; i++)
		{
			if(!object[i] || object[i] == obj)
				continue;

			ClipObjects(obj, object[i]);
		}

		// world
		switch(obj->cv_type)
		{
		case CV_CYLINDER:
			ShiftPlane = ShiftPlaneCylinder;
			break;

		case CV_SPHERE:
			ShiftPlane = ShiftPlaneSphere;
			break;

		case CV_BOX:
			ShiftPlane = ShiftPlaneBox;
			break;

		default:
			return;
		}

		DPS(obj, map->bsp.root_node);
	}
}

void CObjectHandler::ClipObjects(CObject *obj1, CObject *obj2)
{
	CVector3f dir = obj2->dest - obj1->dest;

	if(dir.Length() < 36)
	{
		dir.Normalize();
		obj2->force += dir * 384;
	}
}

int CL(const CVector3f &p1, const CVector3f &p2, const CPlane &plane, const float &eps)
{
	int d1, d2;

	d1 = ClassifyPoint(p1, plane, eps);
	d2 = ClassifyPoint(p2, plane, eps);

	if(d1 == FRONT && d2 == FRONT)
		return FRONT;

	if(d1 == BACK && d2 == BACK)
		return BACK;

	if(d1 == COPLANAR && d2 == COPLANAR)
		return SPANNING_FB; // or BF

	if(d1 == COPLANAR && d2 == FRONT)
		return SPANNING_BF;

	if(d1 == COPLANAR && d2 == BACK)
		return SPANNING_FB;

	if(d2 == COPLANAR && d1 == FRONT)
		return SPANNING_FB;

	if(d2 == COPLANAR && d1 == BACK)
		return SPANNING_BF;

	if(d1 == FRONT && d2 == BACK)
		return SPANNING_FB;

	// if(d1 == BACK && d2 == FRONT)
	return SPANNING_BF;
}

bool CObjectHandler::ClipToBrush(CObject *obj, CPlane *plane, int pl, int num_planes)
{
	CPlane splane = plane[pl];
	ShiftPlane(&splane, true, obj->GetCV());

	switch(CL(obj->pos, obj->dest, splane, EPSILON))
	{
	case SPANNING_BF:
	case BACK:
		if(pl >= num_planes - 1)
			return true;
		return ClipToBrush(obj, plane, pl + 1, num_planes);

	case SPANNING_FB:
		if(pl >= num_planes - 1)
			DoResponse(obj, splane);
		else if(ClipToBrush(obj, plane, pl + 1, num_planes))
			DoResponse(obj, splane);
		break;
	}

	return false;
}

bool CObjectHandler::ClipToBrush_back(CObject *obj, CPlane *plane, int pl, int num_planes)
{
	CPlane splane = plane[pl];
	ShiftPlane(&splane, true, obj->GetCV());

	switch(CL(obj->pos, obj->dest, splane, EPSILON))
	{
	case SPANNING_BF:
	case BACK:
	case SPANNING_FB:
		if(pl >= num_planes - 1)
			DoResponse(obj, splane);
		else if(ClipToBrush_back(obj, plane, pl + 1, num_planes))
			DoResponse(obj, splane);
		break;
	}

	return false;
}

bool CObjectHandler::CheckToBrush(CObject *obj, CPlane *plane, int pl, int num_planes)
{
	CPlane splane = plane[pl];
	ShiftPlane(&splane, true, obj->GetCV());

	switch(ClassifyPoint(obj->dest, splane, EPSILON))
	{
	case COPLANAR:
	case BACK:
	case SPANNING_FB:
	case SPANNING_BF:
		if(pl >= num_planes - 1)
			return true;
		return CheckToBrush(obj, plane, pl + 1, num_planes);
	}

	return false;
}

void CObjectHandler::DPS(CObject *obj, CBSPNode *n)
{
	if(n->isleaf)
	{
		for(int i = 0; i < n->num_brushes; i++)
		{
			if(ClipToBrush(
				obj,
				map->clipbrush[n->brush_idx[i]].plane,
				0,
				map->clipbrush[n->brush_idx[i]].num_planes))
			{
				/*CVector3f tmp = (obj->pos - obj->dest) / 16;
				obj->dest = obj->pos;
				while(CheckToBrush(
					obj,
					map->clipbrush[n->brush_idx[i]].plane,
					0,
					map->clipbrush[n->brush_idx[i]].num_planes))
				{
					//obj->dest += tmp;
					ClipToBrush_back(obj, map->clipbrush[n->brush_idx[i]].plane, 0, map->clipbrush[n->brush_idx[i]].num_planes);
				}*/
			}
		}

		return;
	}


	CPlane plane = n->splitter;
	ShiftPlane(&plane, true, obj->GetCV());

	switch(CL(obj->pos, obj->dest, plane, EPSILON))
	{
	case FRONT:
		DPS(obj, n->front);
		break;

	case BACK:
		DPS(obj, n->back);
		break;

	case SPANNING_FB:
		DPS(obj, n->front);
		DPS(obj, n->back);
		break;

	case SPANNING_BF:
		DPS(obj, n->back);
		DPS(obj, n->front);
		break;
	}

	plane = n->splitter;
	ShiftPlane(&plane, false, obj->GetCV());

	switch(CL(obj->pos, obj->dest, plane, EPSILON))
	{
	case FRONT:
		DPS(obj, n->front);
		break;

	case BACK:
		DPS(obj, n->back);
		break;

	case SPANNING_FB:
		DPS(obj, n->front);
		DPS(obj, n->back);
		break;

	case SPANNING_BF:
		DPS(obj, n->back);
		DPS(obj, n->front);
		break;
	}
}

void CObjectHandler::DoResponse(CObject *obj, const CPlane &plane)
{
	float d = DotProduct(obj->dest, plane.normal) + plane.d;

	//if(d >= EPSILON)
		//return;

	if(DotProduct(CVector3f(0, 1, 0), plane.normal) > 0.1f)
		obj->onground = true;

	obj->dest += plane.normal * (-d + EPSILON + EPSILON);
}

void CObjectHandler::ShiftPlaneCylinder(CPlane *plane, bool front, void *vol)
{
	static cylinder_t *cyl;
	static float f;
	static CVector3f p;

	cyl = (cylinder_t*)vol;

	f = sqrtf((plane->normal.x * plane->normal.x) + (plane->normal.z * plane->normal.z));

	if(f == 0)
	{
		p.x = 0;
		p.z = 0;
	}
	else
	{
		p.x = cyl->r * plane->normal.x / f;
		p.z = cyl->r * plane->normal.z / f;
	}
	//p.y = (plane->normal.y > 0) ? 0 : -cyl->h;

	if(front)
	{
		p.y = (plane->normal.y > 0) ? 0 : -cyl->h;
		plane->d -= DotProduct(p, plane->normal);
	}
	else
	{
		p.y = (plane->normal.y > 0) ? cyl->h : 0;
		plane->d += DotProduct(p, plane->normal);
	}
}

void CObjectHandler::ShiftPlaneSphere(CPlane *plane, bool front, void *vol)
{
	if(front)
		plane->d -= *(float*)vol;
	else
		plane->d += *(float*)vol;
}

void CObjectHandler::ShiftPlaneBox(CPlane *plane, bool front, void *vol)
{
	static box_t *box;

	box = (box_t*)vol;

	if(front)
	{
		plane->d -= DotProduct(
		ABS(plane->normal.x),
		ABS(plane->normal.y),
		ABS(plane->normal.z),
		box->right,
		box->up,
		box->dir);
	}
	else
	{
		plane->d += DotProduct(
		ABS(plane->normal.x),
		ABS(plane->normal.y),
		ABS(plane->normal.z),
		box->right,
		box->up,
		box->dir);
	}
}

/*void CObjectHandler::UpdatePhysics(CObject *obj)
{
	if(obj->onground)
	{
		if(obj->force.y < 0)
			obj->force.y = 0;

		obj->onground = false;
	}
	else // gravitation can pull it down
	{
		obj->force.y -= GRAVITY * sf;
	}

	obj->force.x -= (obj->force.x - (obj->force.x * 0.1f)) * sf * 4.0f;
	obj->force.z -= (obj->force.z - (obj->force.z * 0.1f)) * sf * 4.0f;

	obj->dest += obj->force * sf;
}*/

void CObjectHandler::Destruct()
{
	for(int i = 0; i < MAX_OBJECTS; i++)
	{
		if(object[i])
		{
			delete object[i];
			object[i] = NULL;
		}
	}
}

void CObjectHandler::Render(CRenderDevice *rd)
{
	if(draw_player)
		player.Render(rd);

	for(int i = 0; i < MAX_OBJECTS; i++)
	{
		if(object[i])
			object[i]->Render(rd);
	}
}