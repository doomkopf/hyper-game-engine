#ifndef MD2MESH_H_
#define MD2MESH_H_

#include "AnimatedMesh.h"

#define MAX_TRIANGLES 4096
#define MAX_VERTS 2048
#define MAX_FRAMES 512
#define MAX_MD2SKINS 32
#define MAX_SKINNAME 64

#define IDLE1				0
#define RUN					1
#define SHOT_STAND			2
#define SHOT_SHOULDER		3
#define JUMP				4
#define IDLE2				5
#define SHOT_FALLDOWN		6
#define IDLE3				7
#define IDLE4				8
#define CROUCH				9
#define CROUCH_CRAWL		10
#define CROUCH_IDLE			11
#define CROUCH_DEATH		12
#define DEATH_FALLBACK		13
#define DEATH_FALLFORWARD	14
#define DEATH_FALLBACKSLOW	15

#define IDLE1_START					0
#define IDLE1_END					39
#define RUN_START					40
#define RUN_END						45
#define SHOT_STAND_START			46
#define SHOT_STAND_END				60
#define SHOT_SHOULDER_START			61
#define SHOT_SHOULDER_END			66
#define JUMP_START					67
#define JUMP_END					73
#define IDLE2_START					74
#define IDLE2_END					95
#define SHOT_FALLDOWN_START			96
#define SHOT_FALLDOWN_END			112
#define IDLE3_START					113
#define IDLE3_END					122
#define IDLE4_START					123
#define IDLE4_END					135
#define CROUCH_START				136
#define CROUCH_END					154
#define CROUCH_CRAWL_START			155
#define CROUCH_CRAWL_END			161
#define CROUCH_IDLE_START			162
#define CROUCH_IDLE_END				169
#define CROUCH_DEATH_START			170
#define CROUCH_DEATH_END			177
#define DEATH_FALLBACK_START		178
#define DEATH_FALLBACK_END			185
#define DEATH_FALLFORWARD_START		186
#define DEATH_FALLFORWARD_END		190
#define DEATH_FALLBACKSLOW_START	191
#define DEATH_FALLBACKSLOW_END		198

struct MD2HEADER
{
	int ident;
	int version;
	int skinwidth;
	int skinheight;
	int framesize;
	int num_skins;
	int num_xyz;
	int num_st;
	int num_tris;
	int num_glcmds;
	int num_frames;
	int ofs_skins;
	int ofs_st;
	int ofs_tris;
	int ofs_frames;
	int ofs_glcmds;
	int ofs_end;
};

struct t_index_list
{
	int a, b, c;
	float as, at, bs, bt, cs, ct;
};

struct t_vertex_list
{
	float x, y, z;
};

struct t_frame_list
{
	t_vertex_list *vertex;
};

struct dstvert_t
{
	short s, t;
};

struct dtriangle_t
{
	short index_xyz[3];
	short index_st[3];
};

struct dtrivertx_t
{
	unsigned char v[3];
	unsigned char lightnormalindex;
};

struct daliasframe_t
{
	float scale[3];
	float translate[3];
	char name[16];
	dtrivertx_t verts[1];
};


class CMD2Mesh : public CAnimatedMesh
{
public:
	CMD2Mesh();
	~CMD2Mesh();
	void Destruct();
	virtual bool Load(const char *);
	virtual void SetState(void *);
	virtual void Render(CRenderDevice *);

protected:
	t_index_list *index_list;
	t_frame_list *frame_list;
	int num_frames, num_vertices, num_triangles;
	int frame;
	CTexture *tex;
};

#endif