#ifndef AVATAR_H_
#define AVATAR_H_

#include "Object.h"
#include "MD2Mesh.h"

#define AVATAR_EYE_HEIGHT 70.0f

class CAvatar : public CObject
{
public:
	CAvatar();
	void RotateX(float);
	void RotateY(float);
	//void RotateZ(float);
	void MoveFW(float);
	void MoveBW(float);
	void MoveL(float);
	void MoveR(float);
	void MoveUp(float);
	void MoveDown(float);
	void Jump();
	void SetAnimationState(int);
	void UpdateAnimationState();
	virtual int GetType();
	virtual void *GetCV();
	virtual void Update();
	virtual CMatrix3x3 *GetRotation();
	virtual void Render(CRenderDevice *);

	CVector3f &right, &up, &dir;
	CMatrix3x3 omt;
	CAnimatedMesh *mesh;
	int frame_start, frame_end, frame;
	float mesh_speed, mesh_time;

protected:
	CVector3f mov_dir;
	cylinder_t cv;
};

#endif