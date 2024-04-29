#ifndef PHYSICOBJECT_H_
#define PHYSICOBJECT_H_

#include "../Level0/PObject.h"

class CPhysicObject : public CPObject
{
public:
	CPhysicObject();

protected:
	static float gravity;
};

#endif