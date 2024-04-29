#ifndef RENDERDEVICE_H_
#define RENDERDEVICE_H_

#include <d3d9.h>
#include "../../stuff/windows/Window.h"
#include "Camera.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "VertexShader.h"

#define RND_DIFFUSE 1
#define RND_TEX0    2
#define RND_NORMAL  4

enum alblmode_t
{
	ALBL_TRANSPARENCE = 0,
	ALBL_BLACK = 1
};

struct argb_t
{
	unsigned char b, g, r, a;
};

struct rgb_t
{
	unsigned char r, g, b;
};

struct texcoord_t
{
	float u, v;
};


class CRenderDevice
{
public:
	CRenderDevice(CWindow *);
	~CRenderDevice();
	bool Init(int, bool);
	void Reset();
	void Destruct();

	CTexture *CreateTextureFromFile(const char *);
	CTexture *CreateTexture(int, int, const rgb_t *);

	void BeginFrame();
	void EndFrame();
	void SetTransform3D();
	void SetTransform2D();
	void SetTransform(const CMatrix4x4 &);
	void SetViewport(int, int, int, int);

	void SetTexture(int, CTexture *);
	void SetVertexShader(CVertexShader *);

	void ALBL_Enable();
	void ALBL_Disable();
	void ALBL_SetMode(const alblmode_t &);

	void ZBuffer_Enable();
	void ZBuffer_Disable();

	void WIREFRAME_Enable();
	void WIREFRAME_Disable();

	void BeginPoly(DWORD);
	void BeginTriangles(DWORD);
	void BeginLines(DWORD);
	void BeginPoints(DWORD);
	void Vertex(const CVector3f &);
	void Vertex(float, float, float);
	void Vertex(float, float);
	void Vertex(int, int);
	void Vertex_perc(float, float);
	void Diffuse_RGBA(unsigned char, unsigned char, unsigned char, unsigned char);
	void Diffuse_RGB(unsigned char, unsigned char, unsigned char);
	void Diffuse_A(unsigned char);
	void Texcoord(const texcoord_t &);
	void Texcoord(float, float);
	void Normal(float, float, float);
	void EndPoly();
	void EndTriangles();
	void EndLines();
	void EndPoints();


	IDirect3DDevice9 *dev;
	bool fullscreen;
	int bb_x, bb_y, bpp;

	CCamera cam;

	CVertexShader 
		*vs_diffuse,
		*vs_tex0,
		*vs_diffuse_tex0,
		*vs_normal;

	unsigned char clear_r, clear_g, clear_b;

protected:
	void InitStatic();
	bool CreateVertexBuffers();
	bool CreateVertexShaders();
	void EndPrimitive();

	CWindow *wnd;
	IDirect3D9 *d3d;
	D3DPRESENT_PARAMETERS d3dpp;

	CVertexBuffer 
		*vb_vertex,
		*vb_diffuse,
		*vb_tex0,
		*vb_normal;

	CVector3f *p_vertex;
	DWORD *p_diffuse;
	texcoord_t *p_tex0;
	CVector3f *p_normal;

	int v_count;
	DWORD lock_bits;
	argb_t v_diffuse;
	texcoord_t v_texcoord;
	CVector3f v_normal;

	static CMatrix4x4 m4x4;
};

#endif