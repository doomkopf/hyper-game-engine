#include "MD2Mesh.h"
#include "../../stuff/FileBuf.h"
#include <string.h>

CMD2Mesh::CMD2Mesh()
{
	index_list = NULL;
	frame_list = NULL;
	tex = new CTexture;
}

CMD2Mesh::~CMD2Mesh()
{
	Destruct();
}

bool CMD2Mesh::Load(const char *file)
{
	CFileBuf fb;
	char g_skins[MAX_MD2SKINS][64];
	dstvert_t base_st[MAX_VERTS];
	unsigned char buffer[MAX_VERTS * 4 + 128];
	MD2HEADER modelheader;
	dtriangle_t tri;
	daliasframe_t *out;
	char temp[64];
	strcpy(name, file);

	strcpy(temp, name);
	strcat(temp, ".tga");
	if(!tex->LoadFromFile(temp))
		return false;

	strcpy(temp, name);
	strcat(temp, ".md2");
	if(!fb.Open(temp, true))
		return false;

	fb.Read(&modelheader, sizeof(MD2HEADER));

	num_frames = modelheader.num_frames;
	num_vertices = modelheader.num_xyz;
	num_triangles = modelheader.num_tris;
	index_list = new t_index_list[num_triangles];
	frame_list = new t_frame_list[num_frames];

	for(int i = 0; i < num_frames; i++)
		frame_list[i].vertex = new t_vertex_list[num_vertices];

	fb.Read(g_skins, modelheader.num_skins * MAX_SKINNAME);
	fb.Read(base_st, modelheader.num_st * sizeof(base_st[0]));

	for(i = 0; i < num_triangles; i++)
	{
		fb.Read(&tri, sizeof(dtriangle_t));
		index_list[i].a = tri.index_xyz[2];
		index_list[i].b = tri.index_xyz[1];
		index_list[i].c = tri.index_xyz[0];

		index_list[i].as = base_st[tri.index_st[2]].s;
		index_list[i].at = base_st[tri.index_st[2]].t;
		index_list[i].bs = base_st[tri.index_st[1]].s;
		index_list[i].bt = base_st[tri.index_st[1]].t;
		index_list[i].cs = base_st[tri.index_st[0]].s;
		index_list[i].ct = base_st[tri.index_st[0]].t;
	}

	for(i = 0; i < num_triangles; i++)
	{
		index_list[i].as /= modelheader.skinwidth;
		index_list[i].bs /= modelheader.skinwidth;
		index_list[i].cs /= modelheader.skinwidth;
		index_list[i].at /= modelheader.skinheight;
		index_list[i].bt /= modelheader.skinheight;
		index_list[i].ct /= modelheader.skinheight;
	}

	for(i = 0; i < num_frames; i++)
	{
		out = (daliasframe_t*)buffer;
		fb.Read(out, modelheader.framesize);
		for(int j = 0; j < num_vertices; j++)
		{
			frame_list[i].vertex[j].x = out->verts[j].v[0] * out->scale[0] + out->translate[0];
			frame_list[i].vertex[j].y = out->verts[j].v[1] * out->scale[1] + out->translate[1];
			frame_list[i].vertex[j].z = out->verts[j].v[2] * out->scale[2] + out->translate[2];
			swap(&frame_list[i].vertex[j].y, &frame_list[i].vertex[j].z);
			swap(&frame_list[i].vertex[j].x, &frame_list[i].vertex[j].z);

			// temp scaling
			frame_list[i].vertex[j].x *= 2;
			frame_list[i].vertex[j].y *= 2;
			frame_list[i].vertex[j].z *= 2;
		}
	}

	return true;
}

void CMD2Mesh::SetState(void *s)
{
	frame = *(int*)s;
}

void CMD2Mesh::Render(CRenderDevice *rd)
{
	rd->SetTexture(0, tex);
	rd->BeginTriangles(RND_TEX0);
	for(int i = 0; i < num_triangles; i++)
	{
		rd->Texcoord(index_list[i].as, index_list[i].at);
		rd->Vertex(frame_list[frame].vertex[index_list[i].a].x,
			frame_list[frame].vertex[index_list[i].a].y,
			frame_list[frame].vertex[index_list[i].a].z);
		rd->Texcoord(index_list[i].bs, index_list[i].bt);
		rd->Vertex(frame_list[frame].vertex[index_list[i].b].x,
			frame_list[frame].vertex[index_list[i].b].y,
			frame_list[frame].vertex[index_list[i].b].z);
		rd->Texcoord(index_list[i].cs, index_list[i].ct);
		rd->Vertex(frame_list[frame].vertex[index_list[i].c].x,
			frame_list[frame].vertex[index_list[i].c].y,
			frame_list[frame].vertex[index_list[i].c].z);
	}
	rd->EndTriangles();
}

void CMD2Mesh::Destruct()
{
	if(index_list)
	{
		delete [] index_list;
		num_triangles = 0;
	}

	if(frame_list)
	{
		for(int i = 0; i < num_frames; i++)
		{
			if(frame_list[i].vertex)
			{
				delete [] frame_list[i].vertex;
				frame_list[i].vertex = NULL;
			}
		}
		delete [] frame_list;
		frame_list = NULL;
		num_frames = 0;
	}

	delete tex;
}