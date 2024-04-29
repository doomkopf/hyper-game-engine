#include "RenderDevice.h"
#include "TextureManager.h"

CMatrix4x4 CRenderDevice::m4x4;

CRenderDevice::CRenderDevice(CWindow *w)
{
	d3d = NULL;
	dev = NULL;

	wnd = w;

	vb_vertex = new CVertexBuffer;
	vb_diffuse = new CVertexBuffer;
	vb_tex0 = new CVertexBuffer;
	vb_normal = new CVertexBuffer;

	//vb_points = new CVertexBuffer;
	//vb_pdiffuse = new CVertexBuffer;

	vs_diffuse = new CVertexShader;
	vs_tex0 = new CVertexShader;
	vs_diffuse_tex0 = new CVertexShader;
	vs_normal = new CVertexShader;

	v_diffuse.a = 255;
	v_diffuse.r = 255;
	v_diffuse.g = 255;
	v_diffuse.b = 255;
	v_texcoord.u = 0;
	v_texcoord.v = 0;
	v_normal.Set(0, 0, 0);

	clear_r = 58;
	clear_g = 110;
	clear_b = 165;

	fullscreen = false;
}

CRenderDevice::~CRenderDevice()
{
	Destruct();
}

bool CRenderDevice::Init(int p_bpp, bool fulls)
{
	fullscreen = fulls;
	bpp = p_bpp;

	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if(!d3d)
		return false; // error initializing d3d9

	fullscreen ? wnd->GetWindowSize(&bb_x, &bb_y) : wnd->GetClientSize(&bb_x, &bb_y);
	d3dpp.BackBufferWidth = bb_x;
	d3dpp.BackBufferHeight = bb_y;

	if(bpp == 16)
		d3dpp.BackBufferFormat = D3DFMT_R5G6B5;
	else if(bpp == 32)
		d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	else
	{
		Destruct();
		return false; // unsupported color depth
	}

	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = wnd->hwnd;
	d3dpp.Windowed = fullscreen ? FALSE : TRUE;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = fullscreen ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;

	if(FAILED(d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		wnd->hwnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, //D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE,
		&d3dpp,
		&dev
		)))
	{
		Destruct();
		return false;
	}

	InitStatic();

	// bilinear filtering
	dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	dev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	dev->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
	dev->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
	dev->SetRenderState(D3DRS_POINTSIZE, 10);

	D3DVIEWPORT9 viewport;
	dev->GetViewport(&viewport);
	cam.mt_perspective.PerspectiveLH(PI / 3, (float)viewport.Width / (float)viewport.Height, 8.0f, 8192.0f);
	cam.mt_ortho.OrthographicLH((float)viewport.Width, (float)viewport.Height, -1, 1);
	cam.mt_view2d.View2D(viewport.Width, viewport.Height);

	if(!CreateVertexBuffers())
	{
		Destruct();
		return false;
	}
	if(!CreateVertexShaders())
	{
		Destruct();
		return false;
	}

	return true;
}

void CRenderDevice::Reset()
{
	d3dpp.BackBufferWidth = bb_x;
	d3dpp.BackBufferHeight = bb_y;
	d3dpp.Windowed = fullscreen ? FALSE : TRUE;
	d3dpp.PresentationInterval = fullscreen ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;

	/*int i = 10;
	switch(dev->Reset(&d3dpp))
	{
	case D3DERR_DEVICELOST:
		i = 0;
		break;

	case D3DERR_DRIVERINTERNALERROR:
		i = 0;
		break;

	case D3DERR_INVALIDCALL:
		i = 0;
		break;

	case D3DERR_OUTOFVIDEOMEMORY:
		i = 0;
		break;

	case E_OUTOFMEMORY:
		i = 0;
		break;

	case D3D_OK:
		i = 0;
		break;

	default:
		i = 0;
		break;
	}*/
}

void CRenderDevice::InitStatic()
{
	CVertexBuffer::rd = this;
	CVertexShader::rd = this;
	CTextureManager::rd = this;
	CTexture::dev = dev;
}

void CRenderDevice::Destruct()
{
	// vertex buffers
	if(vb_vertex)
	{
		delete vb_vertex;
		vb_vertex = NULL;
	}

	if(vb_diffuse)
	{
		delete vb_diffuse;
		vb_diffuse = NULL;
	}

	if(vb_tex0)
	{
		delete vb_tex0;
		vb_tex0 = NULL;
	}

	if(vb_normal)
	{
		delete vb_normal;
		vb_normal = NULL;
	}

	/*if(vb_points)
	{
		delete vb_points;
		vb_points = NULL;
	}*/

	/*if(vb_pdiffuse)
	{
		delete vb_pdiffuse;
		vb_pdiffuse = NULL;
	}*/

	// vertex shaders
	if(vs_diffuse)
	{
		delete vs_diffuse;
		vs_diffuse = NULL;
	}

	if(vs_tex0)
	{
		delete vs_tex0;
		vs_tex0 = NULL;
	}

	if(vs_diffuse_tex0)
	{
		delete vs_diffuse_tex0;
		vs_diffuse_tex0 = NULL;
	}

	if(vs_normal)
	{
		delete vs_normal;
		vs_normal = NULL;
	}

	// other
	if(dev)
	{
		dev->Release();
		dev = NULL;
	}

	if(d3d)
	{
		d3d->Release();
		d3d = NULL;
	}
}

CTexture *CRenderDevice::CreateTexture(int x, int y, const rgb_t *data)
{
	IDirect3DTexture9 *t;

	if(FAILED(dev->CreateTexture(
		x,
		y,
		0,
		D3DUSAGE_AUTOGENMIPMAP,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		&t,
		NULL)))
		return NULL;

	CTexture *tex = new CTexture;
	tex->tex = t;

	argb_t *d = (argb_t*)tex->Lock(NULL, NULL);
	for(int i = 0, c = x * y; i < c; i++)
	{
		d[i].a = 0;
		d[i].r = data[i].r;
		d[i].g = data[i].g;
		d[i].b = data[i].b;
	}
	tex->Unlock();

	return tex;
}

CTexture *CRenderDevice::CreateTextureFromFile(const char *file)
{
	IDirect3DTexture9 *t;
	D3DXIMAGE_INFO io;

	/*if(FAILED(D3DXCreateTextureFromFile(
		dev,
		file,
		&t)))*/
	if(FAILED(D3DXCreateTextureFromFileEx(
		dev,
		file,
		0,
		0,
		0,
		0,
		D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		&io,
		NULL,
		&t)))
		return NULL;

	CTexture *tex = new CTexture;
	tex->tex = t;

	return tex;
}

#define VB_SIZE 262144

bool CRenderDevice::CreateVertexBuffers()
{
	if(!vb_vertex->Create(VB_SIZE * sizeof(CVector3f), false))
		return false;
	if(!vb_diffuse->Create(VB_SIZE * sizeof(DWORD), true))
		return false;
	if(!vb_tex0->Create(VB_SIZE * sizeof(texcoord_t), true))
		return false;
	if(!vb_normal->Create(VB_SIZE * sizeof(CVector3f), true))
		return false;

	//if(!vb_points->Create(VB_SIZE * sizeof(CVector3f), true, true))
		//return false;
	//if(!vb_pdiffuse->Create(VB_SIZE * sizeof(DWORD), true, true))
		//return false;

	return true;
}

bool CRenderDevice::CreateVertexShaders()
{
	vertex_element_t ve_diffuse[] =
	{
		{ 0, VET_POSITION },
		{ 1, VET_COLOR }
	};

	char *src_diffuse =
	{
		"vs_1_1\n"

		"dcl_position v0\n"
		"dcl_color0 v1\n"

		"dp4 oPos.x, v0, c0\n"
		"dp4 oPos.y, v0, c1\n"
		"dp4 oPos.z, v0, c2\n"
		"dp4 oPos.w, v0, c3\n"

		"mov oD0, v1"
	};

	vertex_element_t ve_tex0[] =
	{
		{ 0, VET_POSITION },
		{ 2, VET_TEX0 }
	};

	char *src_tex0 =
	{
		"vs_1_1\n"

		"dcl_position v0\n"
		"dcl_texcoord0 v1\n"

		"dp4 oPos.x, v0, c0\n"
		"dp4 oPos.y, v0, c1\n"
		"dp4 oPos.z, v0, c2\n"
		"dp4 oPos.w, v0, c3\n"

		"mov oT0, v1"
	};

	vertex_element_t ve_diffuse_tex0[] =
	{
		{ 0, VET_POSITION },
		{ 1, VET_COLOR },
		{ 2, VET_TEX0 }
	};

	char *src_diffuse_tex0 =
	{
		"vs_1_1\n"

		"dcl_position v0\n"
		"dcl_color0 v1\n"
		"dcl_texcoord0 v2\n"

		"dp4 oPos.x, v0, c0\n"
		"dp4 oPos.y, v0, c1\n"
		"dp4 oPos.z, v0, c2\n"
		"dp4 oPos.w, v0, c3\n"

		"mov oD0, v1\n"
		"mov oT0, v2"
	};

	vertex_element_t ve_normal[] =
	{
		{ 0, VET_POSITION },
		{ 1, VET_NORMAL }
	};

	char *src_normal =
	{
		"vs_1_1\n"

		"def c4, 0.0, 0.0, 1.0, 0.0\n"

		"dcl_position v0\n"
		"dcl_normal v1\n"

		"dp4 oPos.x, v0, c0\n"
		"dp4 oPos.y, v0, c1\n"
		"dp4 oPos.z, v0, c2\n"
		"dp4 oPos.w, v0, c3\n"

		//"mov oD0, v1"
		"mov r0, v1\n"
		"m3x3 r1, r0, <viewmatrix>\n"
		"dp3 oD0, -r1, c4"
	};

	if(!vs_diffuse->Create(src_diffuse, ve_diffuse, 2))
		return false;
	if(!vs_tex0->Create(src_tex0, ve_tex0, 2))
		return false;
	if(!vs_diffuse_tex0->Create(src_diffuse_tex0, ve_diffuse_tex0, 3))
		return false;
	//if(!vs_normal->Create(src_normal, ve_normal, 2))
		//return false;

	return true;
}

void CRenderDevice::ALBL_Enable()
{
	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	dev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
}

void CRenderDevice::ALBL_Disable()
{
	dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CRenderDevice::ALBL_SetMode(const alblmode_t &mode)
{
	switch(mode)
	{
	case ALBL_TRANSPARENCE:
		dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		break;

	case ALBL_BLACK:
		break;
	}
}

void CRenderDevice::SetViewport(int min_x, int min_y, int max_x, int max_y)
{
	D3DVIEWPORT9 viewport;
	viewport.MinZ = 0;
	viewport.MaxZ = 1;
	viewport.X = min_x;
	viewport.Y = min_y;
	viewport.Width = max_x;
	viewport.Height = max_y;
	dev->SetViewport(&viewport);

	cam.mt_perspective.PerspectiveLH(PI / 3, (float)viewport.Width / (float)viewport.Height, 8.0f, 8192.0f);
	cam.mt_ortho.OrthographicLH((float)viewport.Width, (float)viewport.Height, -1, 1);
	cam.mt_view2d.View2D(viewport.Width, viewport.Height);
}

void CRenderDevice::ZBuffer_Enable()
{
	dev->SetRenderState(D3DRS_ZENABLE, TRUE);
}

void CRenderDevice::ZBuffer_Disable()
{
	dev->SetRenderState(D3DRS_ZENABLE, FALSE);
}

void CRenderDevice::WIREFRAME_Enable()
{
	dev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
}

void CRenderDevice::WIREFRAME_Disable()
{
	dev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

void CRenderDevice::SetTexture(int stage, CTexture *tex)
{
	dev->SetTexture(stage, tex ? tex->tex : NULL);
}

void CRenderDevice::SetVertexShader(CVertexShader *vs)
{
	dev->SetVertexDeclaration(vs->decl);
	dev->SetVertexShader(vs->vs);
}

void CRenderDevice::BeginTriangles(DWORD lb)
{
	lock_bits = lb;
	v_count = 0;

	vb_vertex->Lock((void**)&p_vertex);

	if(lock_bits & RND_DIFFUSE)
		vb_diffuse->Lock((void**)&p_diffuse);
	if(lock_bits & RND_TEX0)
		vb_tex0->Lock((void**)&p_tex0);
}

void CRenderDevice::BeginPoly(DWORD lb)
{
	BeginTriangles(lb);
}

void CRenderDevice::BeginLines(DWORD lb)
{
	BeginTriangles(lb);
}

void CRenderDevice::BeginPoints(DWORD lb)
{
	BeginTriangles(lb);
}

void CRenderDevice::EndPrimitive()
{
	vb_vertex->Unlock();
	dev->SetStreamSource(0, vb_vertex->vb, 0, sizeof(CVector3f));

	if(lock_bits & RND_DIFFUSE)
	{
		vb_diffuse->Unlock();
		dev->SetStreamSource(1, vb_diffuse->vb, 0, sizeof(DWORD));
	}
	if(lock_bits & RND_TEX0)
	{
		vb_tex0->Unlock();
		dev->SetStreamSource(2, vb_tex0->vb, 0, sizeof(texcoord_t));
	}
}

void CRenderDevice::EndTriangles()
{
	EndPrimitive();

	dev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, v_count / 3);
}

void CRenderDevice::EndPoly()
{
	EndPrimitive();

	dev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, v_count - 2);
}

void CRenderDevice::EndLines()
{
	EndPrimitive();

	dev->DrawPrimitive(D3DPT_LINELIST, 0, v_count / 2);
}

void CRenderDevice::EndPoints()
{
	EndPrimitive();

	dev->DrawPrimitive(D3DPT_POINTLIST, 0, v_count);
}

void CRenderDevice::Vertex(const CVector3f &v)
{
	p_vertex[v_count] = v;

	if(lock_bits & RND_DIFFUSE)
		memcpy(&p_diffuse[v_count], &v_diffuse, sizeof(DWORD));
	if(lock_bits & RND_TEX0)
		p_tex0[v_count] = v_texcoord;

	v_count++;
}

void CRenderDevice::Vertex(float x, float y, float z)
{
	p_vertex[v_count].x = x;
	p_vertex[v_count].y = y;
	p_vertex[v_count].z = z;

	if(lock_bits & RND_DIFFUSE)
		memcpy(&p_diffuse[v_count], &v_diffuse, sizeof(DWORD));
	if(lock_bits & RND_TEX0)
		p_tex0[v_count] = v_texcoord;

	v_count++;
}

void CRenderDevice::Vertex(float x, float y)
{
	p_vertex[v_count].x = x;
	p_vertex[v_count].y = y;
	p_vertex[v_count].z = 0;

	if(lock_bits & RND_DIFFUSE)
		memcpy(&p_diffuse[v_count], &v_diffuse, sizeof(DWORD));
	if(lock_bits & RND_TEX0)
		p_tex0[v_count] = v_texcoord;

	v_count++;
}

void CRenderDevice::Vertex(int x, int y)
{
	p_vertex[v_count].x = (float)x;
	p_vertex[v_count].y = (float)y;
	p_vertex[v_count].z = 0;

	if(lock_bits & RND_DIFFUSE)
		memcpy(&p_diffuse[v_count], &v_diffuse, sizeof(DWORD));
	if(lock_bits & RND_TEX0)
		p_tex0[v_count] = v_texcoord;

	v_count++;
}

void CRenderDevice::Vertex_perc(float x, float y)
{
	p_vertex[v_count].x = x * bb_x;
	p_vertex[v_count].y = y * bb_y;
	p_vertex[v_count].z = 0;

	if(lock_bits & RND_DIFFUSE)
		memcpy(&p_diffuse[v_count], &v_diffuse, sizeof(DWORD));
	if(lock_bits & RND_TEX0)
		p_tex0[v_count] = v_texcoord;

	v_count++;
}

void CRenderDevice::Diffuse_RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	v_diffuse.a = a;
	v_diffuse.r = r;
	v_diffuse.g = g;
	v_diffuse.b = b;
}

void CRenderDevice::Diffuse_RGB(unsigned char r, unsigned char g, unsigned char b)
{
	v_diffuse.r = r;
	v_diffuse.g = g;
	v_diffuse.b = b;
}

void CRenderDevice::Diffuse_A(unsigned char a)
{
	v_diffuse.a = a;
}

void CRenderDevice::Texcoord(const texcoord_t &tc)
{
	v_texcoord = tc;
}

void CRenderDevice::Texcoord(float u, float v)
{
	v_texcoord.u = u;
	v_texcoord.v = v;
}

void CRenderDevice::SetTransform3D()
{
	m4x4.Multiply(cam.mt_view, cam.mt_perspective);
	//dev->SetVertexShaderConstantF(0, (float*)&m4x4, 4);
	SetTransform(m4x4);
}

void CRenderDevice::SetTransform2D()
{
	m4x4.Multiply(cam.mt_view2d, cam.mt_ortho);
	//dev->SetVertexShaderConstantF(0, (float*)&m4x4, 4);
	SetTransform(m4x4);
}

void CRenderDevice::SetTransform(const CMatrix4x4 &m)
{
	dev->SetVertexShaderConstantF(0, (float*)&m, 4);
}

void CRenderDevice::BeginFrame()
{
	if(wnd->IsResized())
	{
		wnd->GetClientSize(&bb_x, &bb_y);
		Reset();
	}

	dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(clear_r, clear_g, clear_b), 1, 0x00);
	dev->BeginScene();
}

void CRenderDevice::EndFrame()
{
	dev->EndScene();
	dev->Present(NULL, NULL, NULL, NULL);
}