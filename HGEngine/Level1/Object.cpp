#include "Object.h"
#include "../Level0/Engine.h"

CQuaternion CObject::quat;
CMatrix3x3 CObject::m;
CMeshHandler *CObject::mh;

CObject::CObject()// : right((CVector3f&)omt.m[0]), up((CVector3f&)omt.m[3]), dir((CVector3f&)omt.m[6])
{
	id = -1;
	onground = false;
	noclip = false;
	force.Set(0, 0, 0);
}

CObject::~CObject()
{
}