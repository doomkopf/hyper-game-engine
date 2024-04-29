#include "Avatar.h"
#define OBJTYPE_AVATAR 2

CAvatar::CAvatar() : right((CVector3f&)omt.m[0]), up((CVector3f&)omt.m[3]), dir((CVector3f&)omt.m[6])
{
	pos.Set(0, 0, 0);
	dest = pos;
	right.Set(1, 0, 0);
	up.Set(0, 1, 0);
	dir.Set(0, 0, 1);
	mov_dir = dir;

	cv_type = CV_CYLINDER;
	cv.h = 100;
	cv.r = 20;

	weight = 80;

	mesh = NULL;

	mesh_speed = 60.0f;
	mesh_time = 0;
}

int CAvatar::GetType()
{
	return OBJTYPE_AVATAR;
}

void CAvatar::RotateX(float f)
{
	quat.AxisAngle(right, f);
	quat.GetMatrix(&m);

	VectorRotate(&up, m);
	VectorRotate(&dir, m);
}

void CAvatar::RotateY(float f)
{
	quat.AxisAngle(CVector3f(0, 1, 0), f);
	quat.GetMatrix(&m);

	VectorRotate(&right, m);
	VectorRotate(&up, m);
	VectorRotate(&dir, m);
	VectorRotate(&mov_dir, m);
}

/*void CAvatar::RotateZ(float f)
{
	quat.AxisAngle(CVector3f(0, 0, 1), f);
	quat.GetMatrix(&m);

	VectorRotate(&right, m);
	VectorRotate(&up, m);
	VectorRotate(&dir, m);
}*/

void CAvatar::MoveFW(float f)
{
	//force += mov_dir * f * sf;
	force += dir * f * sf;
}

void CAvatar::MoveBW(float f)
{
	//force -= mov_dir * f * sf;
	force -= dir * f * sf;
}

void CAvatar::MoveL(float f)
{
	force -= right * f * sf;
}

void CAvatar::MoveR(float f)
{
	force += right * f * sf;
}

void CAvatar::MoveUp(float f)
{
}

void CAvatar::MoveDown(float f)
{
}

void CAvatar::Jump()
{
	if(onground)
	{
		force.y += 360;
	}
}

void *CAvatar::GetCV()
{
	return &cv;
}

CMatrix3x3 *CAvatar::GetRotation()
{
	return &omt;
}

void CAvatar::SetAnimationState(int s)
{
	switch(s)
	{
	case IDLE1:
		frame = frame_start = IDLE1_START;
		frame_end = IDLE1_END;
		break;

	case RUN:
		frame = frame_start = RUN_START;
		frame_end = RUN_END;
		break;

	case SHOT_STAND:
		frame = frame_start = SHOT_STAND_START;
		frame_end = SHOT_STAND_END;
		break;

	case SHOT_SHOULDER:
		frame = frame_start = SHOT_SHOULDER_START;
		frame_end = SHOT_SHOULDER_END;
		break;

	case JUMP:
		frame = frame_start = JUMP_START;
		frame_end = JUMP_END;
		break;

	case IDLE2:
		frame = frame_start = IDLE2_START;
		frame_end = IDLE2_END;
		break;

	case SHOT_FALLDOWN:
		frame = frame_start = SHOT_FALLDOWN_START;
		frame_end = SHOT_FALLDOWN_END;
		break;

	case IDLE3:
		frame = frame_start = IDLE3_START;
		frame_end = IDLE3_END;
		break;

	case IDLE4:
		frame = frame_start = IDLE4_START;
		frame_end = IDLE4_END;
		break;

	case CROUCH:
		frame = frame_start = CROUCH_START;
		frame_end = CROUCH_END;
		break;

	case CROUCH_CRAWL:
		frame = frame_start = CROUCH_CRAWL_START;
		frame_end = CROUCH_CRAWL_END;
		break;

	case CROUCH_IDLE:
		frame = frame_start = CROUCH_IDLE_START;
		frame_end = CROUCH_IDLE_END;
		break;

	case CROUCH_DEATH:
		frame = frame_start = CROUCH_DEATH_START;
		frame_end = CROUCH_DEATH_END;
		break;

	case DEATH_FALLBACK:
		frame = frame_start = DEATH_FALLBACK_START;
		frame_end = DEATH_FALLBACK_END;
		break;

	case DEATH_FALLFORWARD:
		frame = frame_start = DEATH_FALLFORWARD_START;
		frame_end = DEATH_FALLFORWARD_END;
		break;

	case DEATH_FALLBACKSLOW:
		frame = frame_start = DEATH_FALLBACKSLOW_START;
		frame_end = DEATH_FALLBACKSLOW_END;
		break;
	}
}

void CAvatar::UpdateAnimationState()
{
	mesh_time += mesh_speed * sf;
	if(mesh_time > 10)
	{
		frame++;
		if(frame > frame_end)
			frame = frame_start;
		mesh_time = 0;
	}

	mesh->SetState(&frame);
}

void CAvatar::Update()
{
	if(onground)
	{
		if(force.y < 0)
			force.y = 0;

		onground = false;
	}
	else // gravitation can pull it down
	{
		force.y -= gravity * sf;
	}

	force.x -= (force.x - (force.x * 0.1f)) * sf * 4.0f;
	force.z -= (force.z - (force.z * 0.1f)) * sf * 4.0f;
	//force.y -= (force.y - (force.y * 0.1f)) * sf * 4.0f; // temp

	dest += force * sf;

	if(mesh)
		UpdateAnimationState();
}

void CAvatar::Render(CRenderDevice *rd)
{
	CMatrix3x3 *r = GetRotation();

	static CMatrix4x4 m4x4;
	m4x4.Translation(pos.x, pos.y + 45, pos.z);
	m4x4.Multiply(rd->cam.mt_view);
	if(r)
	{
		CMatrix4x4 m = m4x4;
		m4x4.From3x3(*r);
		m4x4.Transpose();
		m4x4.Multiply(m);
	}
	m4x4.Multiply(rd->cam.mt_perspective);
	rd->SetTransform(m4x4);

	rd->SetVertexShader(rd->vs_tex0);
	mesh->Render(rd);
}