#include "FreeObject.h"
#define OBJTYPE_FREEOBJECT 1

CFreeObject::CFreeObject()
{
	cv = NULL;
	cv_type = CV_NONE;
	mesh = NULL;

	rm.Identity();

	CQuaternion q;
	q.AxisAngle(CVector3f(0, 1, 0), DEG_TO_RAD(45));
	q.GetMatrix(&rm);
}

CFreeObject::~CFreeObject()
{
	if(cv)
	{
		delete cv;
		cv = NULL;
	}
}

int CFreeObject::GetType()
{
	return OBJTYPE_FREEOBJECT;
}

void *CFreeObject::GetCV()
{
	return cv;
}

CMatrix3x3 *CFreeObject::GetRotation()
{
	return &rm;
}

void CFreeObject::Update()
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

	dest += force * sf;
}

void CFreeObject::Render(CRenderDevice *rd)
{
	CMatrix3x3 *r = GetRotation();

	static CMatrix4x4 m4x4;
	m4x4.Translation(pos.x, pos.y, pos.z);
	m4x4.Multiply(rd->cam.mt_view);
	if(r)
	{
		CMatrix4x4 m = m4x4;
		m4x4.From3x3(*r);
		m4x4.Multiply(m);
	}
	m4x4.Multiply(rd->cam.mt_perspective);
	rd->SetTransform(m4x4);

	rd->SetTexture(0, NULL);
	rd->SetVertexShader(rd->vs_diffuse);
	rd->Diffuse_RGB(255, 255, 255);
	rd->BeginTriangles(RND_DIFFUSE);
	for(int i = 0; i < mesh->num_faces; i++)
	{
		rd->Vertex(mesh->vertex[mesh->face[i].v[0]].p);
		rd->Vertex(mesh->vertex[mesh->face[i].v[1]].p);
		rd->Vertex(mesh->vertex[mesh->face[i].v[2]].p);
	}
	rd->EndTriangles();
}