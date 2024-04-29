#include "StaticMesh.h"

CStaticMesh::CStaticMesh()
{
	vertex = NULL;
	face = NULL;

	name[0] = 0;

	num_vertices = 0;
	num_faces = 0;
}

CStaticMesh::~CStaticMesh()
{
	Destruct();
}

void CStaticMesh::Destruct()
{
	if(vertex)
	{
		delete [] vertex;
		vertex = NULL;
		num_vertices = 0;
	}
	if(face)
	{
		delete [] face;
		face = NULL;
		num_faces = 0;
	}
}

bool CStaticMesh::LoadSMF(const char *n)
{
	CFileBuf fb;
	int i;

	if(!fb.Open(n, true))
		return false;

	fb.Read(&num_vertices, sizeof(int));
	vertex = new CMeshVertex[num_vertices];
	for(i = 0; i < num_vertices; i++)
	{
		fb.Read(&vertex[i].p, sizeof(CVector3f));
	}

	fb.Read(&num_faces, sizeof(int));
	face = new CMeshFace[num_faces];
	for(i = 0; i < num_faces; i++)
	{
		fb.Read(face[i].v, sizeof(float[3]));
	}

	fb.Close();

	//CalcVertexNormals();
	//CalcNeighborFaces();

	strcpy(name, "smf");

	return true;
}

void CStaticMesh::CreateSphere(const CVector3f &origin, float radius, int slices, int stacks)
{
	CVector3f v;
	float rot_stack, rot_slice;
	int i, j, p = 0;
	int tp[4];

	Destruct();

	// point calculation

	v.Set(0, 1, 0);
	v *= radius;

	if(slices < 4)
		slices = 4;
	if(stacks < 2)
		stacks = 2;

	rot_slice = 360.0f / slices;
	rot_stack = 180.0f / stacks;

	num_vertices = (slices * (stacks - 1)) + 2;
	vertex = new CMeshVertex[num_vertices];

	vertex[p++].p = origin + v;
	VectorRotateX(rot_stack, &v, true);

	for(i = 0; i < stacks - 1; i++)
	{
		for(j = 0; j < slices; j++)
		{
			vertex[p++].p = origin + v;
			VectorRotateY(rot_slice, &v, true);
		}
		VectorRotateX(rot_stack, &v, true);
	}

	vertex[p++].p = origin + v;


	// face calculation

	p = 0;

	num_faces = (slices * 2) + ((slices * (stacks - 2)) * 2);
	face = new CMeshFace[num_faces];

	for(i = 1, j = 1; i <= slices; i++)
	{
		face[p].v[0] = 0;
		face[p].v[1] = j++;
		if(i == slices)
			face[p].v[2] = 1;
		else
			face[p].v[2] = j;

		p++;
	}

	for(i = 1; i < num_vertices - (slices + 1); i++)
	{
		tp[0] = i;
		tp[1] = tp[0] + slices;
		if(i % slices == 0)
		{
			tp[2] = i - (slices - 1) + slices;
			tp[3] = tp[2] - slices;
		}
		else
		{
			tp[2] = tp[1] + 1;
			tp[3] = tp[0] + 1;
		}

		face[p].v[0] = tp[0];
		face[p].v[1] = tp[1];
		face[p].v[2] = tp[2];
		p++;
		face[p].v[0] = tp[0];
		face[p].v[1] = tp[2];
		face[p].v[2] = tp[3];
		p++;
	}

	for(i = 1, j = num_vertices - 2; i <= slices; i++)
	{
		face[p].v[0] = num_vertices - 1;
		face[p].v[1] = j--;
		if(i == slices)
			face[p].v[2] = num_vertices - 2;
		else
			face[p].v[2] = j;

		p++;
	}

	// other calculations

	//CalcVertexNormals();
	//CalcNeighborFaces();

	strcpy(name, "default_sphere");
}

void CStaticMesh::CreateCylinder(const CVector3f &origin, const CVector3f &p2, float radius, int slices, int stacks, bool closed)
{
	CVector3f v, p_inc, v_inc;
	float rot_slice;
	int i, j, p, tp[4];

	Destruct();

	// point calculation

	if(slices < 3)
		slices = 3;
	if(stacks < 1)
		stacks = 1;

	v.Set(0, 0, 1);
	v *= radius;
	rot_slice = 360.0f / slices;
	v_inc = p2 - origin;
	radius = v_inc.Length();
	radius /= stacks;
	v_inc.Normalize();

	num_vertices = (slices * (stacks + 1)) + (closed ? 2 : 0);
	vertex = new CMeshVertex[num_vertices];

	p = 0;
	p_inc = origin;

	for(i = 0; i < stacks + 1; i++)
	{
		for(j = 0; j < slices; j++)
		{
			vertex[p++].p = p_inc + v;
			VectorRotateAxis(-rot_slice, v_inc, &v, true);
		}

		p_inc += v_inc * radius;
	}

	if(closed)
	{
		vertex[p++].p = origin;
		vertex[p++].p = p2;
	}

	// face calculation

	num_faces = (slices * stacks * 2) + (closed ? slices * 2 : 0);
	face = new CMeshFace[num_faces];

	p = 0;

	for(i = 0; i < num_vertices - slices - (closed ? 2 : 0); i++)
	{
		tp[0] = i;
		tp[1] = tp[0] + slices;
		if((i + 1) % slices == 0)
		{
			tp[2] = i - (slices - 1) + slices;
			tp[3] = tp[2] - slices;
		}
		else
		{
			tp[2] = tp[1] + 1;
			tp[3] = tp[0] + 1;
		}

		face[p].v[0] = tp[0];
		face[p].v[1] = tp[1];
		face[p].v[2] = tp[2];
		p++;
		face[p].v[0] = tp[0];
		face[p].v[1] = tp[2];
		face[p].v[2] = tp[3];
		p++;
	}

	if(closed)
	{
		for(i = 1, j = 0; i <= slices; i++)
		{
			face[p].v[0] = num_vertices - 2;
			face[p].v[1] = j++;
			if(i == slices)
				face[p].v[2] = 0;
			else
				face[p].v[2] = j;

			p++;
		}

		for(i = 1, j = num_vertices - 3; i <= slices; i++)
		{
			face[p].v[0] = num_vertices - 1;
			face[p].v[1] = j--;
			if(i == slices)
				face[p].v[2] = num_vertices - 3;
			else
				face[p].v[2] = j;

			p++;
		}
	}

	// other calculations

	//CalcVertexNormals();
	//CalcNeighborFaces();

	strcpy(name, "default_cylinder");
}

void CStaticMesh::CreateBlock(const CVector3f &origin, const CVector3f &min, const CVector3f &max)
{
	num_vertices = 8;
	vertex = new CMeshVertex[num_vertices];
	num_faces = 12;
	face = new CMeshFace[num_faces];

	// point calculation

	vertex[0].p = min;
	vertex[0].p += origin;

	vertex[1].p.Set(min.x, min.y, max.z);
	vertex[1].p += origin;

	vertex[2].p.Set(max.x, min.y, max.z);
	vertex[2].p += origin;

	vertex[3].p.Set(max.x, min.y, min.z);
	vertex[3].p += origin;

	vertex[4].p.Set(min.x, max.y, min.z);
	vertex[4].p += origin;

	vertex[5].p.Set(min.x, max.y, max.z);
	vertex[5].p += origin;

	vertex[6].p = max;
	vertex[6].p += origin;

	vertex[7].p.Set(max.x, max.y, min.z);
	vertex[7].p += origin;


	// face calculation

	// left
	face[0].v[0] = 0;
	face[0].v[1] = 1;
	face[0].v[2] = 4;

	face[1].v[0] = 4;
	face[1].v[1] = 1;
	face[1].v[2] = 5;

	// right
	face[2].v[0] = 2;
	face[2].v[1] = 3;
	face[2].v[2] = 6;

	face[3].v[0] = 6;
	face[3].v[1] = 3;
	face[3].v[2] = 7;

	// front
	face[4].v[0] = 3;
	face[4].v[1] = 0;
	face[4].v[2] = 7;

	face[5].v[0] = 7;
	face[5].v[1] = 0;
	face[5].v[2] = 4;

	// back
	face[6].v[0] = 1;
	face[6].v[1] = 2;
	face[6].v[2] = 5;

	face[7].v[0] = 5;
	face[7].v[1] = 2;
	face[7].v[2] = 6;

	// top
	face[8].v[0] = 7;
	face[8].v[1] = 4;
	face[8].v[2] = 6;

	face[9].v[0] = 6;
	face[9].v[1] = 4;
	face[9].v[2] = 5;

	// bottom
	face[10].v[0] = 2;
	face[10].v[1] = 1;
	face[10].v[2] = 3;

	face[11].v[0] = 3;
	face[11].v[1] = 1;
	face[11].v[2] = 0;

	strcpy(name, "default_block");
}