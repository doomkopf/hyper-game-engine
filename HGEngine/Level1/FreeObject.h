#ifndef FREEOBJECT_H_
#define FREEOBJECT_H_

#include "Object.h"

class CFreeObject : public CObject
{
public:
	CFreeObject();
	virtual ~CFreeObject();
	virtual int GetType();
	virtual void *GetCV();
	virtual void Update();
	virtual CMatrix3x3 *GetRotation();
	virtual void Render(CRenderDevice *);

	void *cv;
	CStaticMesh *mesh;

protected:
	CMatrix3x3 rm;
};

#endif