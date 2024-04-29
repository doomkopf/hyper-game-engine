#ifndef OBJECT_H_
#define OBJECT_H_

#include "../../stuff/Math.h"
#include "collision_volumes.h"
#include "MeshHandler.h"
#include "PhysicObject.h"
#include "../Level0/Renderable.h"

class CObject : public CPhysicObject, IRenderable
{
	friend class CObjectHandler;
public:
	CObject();
	virtual ~CObject();
	virtual int GetType() = 0;
	virtual void *GetCV() = 0;
	virtual void Update() = 0;
	virtual CMatrix3x3 *GetRotation() = 0;

	int id;
	cv_t cv_type;
	CVector3f pos, dest;
	//CVector3f &right, &up, &dir;
	//CMatrix3x3 omt;

	bool noclip;

	// physics stuff
	CVector3f force;
	float weight; // in kilogram
	bool onground;

	static CMeshHandler *mh;

protected:
	static CQuaternion quat;
	static CMatrix3x3 m;
};

#endif