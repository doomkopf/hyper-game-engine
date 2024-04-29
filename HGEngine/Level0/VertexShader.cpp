#include "VertexShader.h"
#include "RenderDevice.h"

CRenderDevice *CVertexShader::rd;

CVertexShader::CVertexShader()
{
	vs = NULL;
	decl = NULL;
}

CVertexShader::~CVertexShader()
{
	Destruct();
}

bool CVertexShader::Create(const char *vs_source, const vertex_element_t *ve, int ve_count)
{
	D3DVERTEXELEMENT9 *v = new D3DVERTEXELEMENT9[ve_count + 1];
	LPD3DXBUFFER code;

	int i = 0;
	for(; i < ve_count; i++)
	{
		v[i].Stream = ve[i].stream;
		v[i].Offset = 0;
		v[i].Method = D3DDECLMETHOD_DEFAULT;
		v[i].UsageIndex = 0;

		switch(ve[i].type)
		{
		case VET_POSITION:
			v[i].Type = D3DDECLTYPE_FLOAT3;
			v[i].Usage = D3DDECLUSAGE_POSITION;
			break;

		case VET_COLOR:
			v[i].Type = D3DDECLTYPE_D3DCOLOR;
			v[i].Usage = D3DDECLUSAGE_COLOR;
			break;

		case VET_TEX0:
			v[i].Type = D3DDECLTYPE_FLOAT2;
			v[i].Usage = D3DDECLUSAGE_TEXCOORD;
			break;

		case VET_NORMAL:
			v[i].Type = D3DDECLTYPE_FLOAT3;
			v[i].Usage = D3DDECLUSAGE_NORMAL;
			break;
		}
	}

	v[i].Stream = 0xff;
	v[i].Offset = 0;
	v[i].Method = 0;
	v[i].UsageIndex = 0;
	v[i].Usage = 0;
	v[i].Type = D3DDECLTYPE_UNUSED;

	if(FAILED(rd->dev->CreateVertexDeclaration(v, &decl)))
		return false;

	if(FAILED(D3DXAssembleShader(vs_source, (int)strlen(vs_source), NULL, NULL, 0, &code, NULL)))
	{
		code->Release();
		return false;
	}

	if(FAILED(rd->dev->CreateVertexShader((DWORD*)code->GetBufferPointer(), &vs)))
	{
		code->Release();
		return false;
	}

	code->Release();

	delete [] v;

	return true;
}

void CVertexShader::Destruct()
{
	if(decl)
	{
		decl->Release();
		decl = NULL;
	}

	if(vs)
	{
		vs->Release();
		vs = NULL;
	}
}