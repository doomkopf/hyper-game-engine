#ifndef VERTEXSHADER_H_
#define VERTEXSHADER_H_

#include <d3dx9.h>

enum vertex_element_type_t
{
	VET_POSITION = 0,
	VET_COLOR = 1,
	VET_TEX0 = 2,
	VET_NORMAL = 3
};

struct vertex_element_t
{
	int stream;
	vertex_element_type_t type;
};

class CRenderDevice;

class CVertexShader
{
	friend class CRenderDevice;
public:
	CVertexShader();
	~CVertexShader();
	bool Create(const char *, const vertex_element_t *, int);
	void Destruct();

protected:
	static CRenderDevice *rd;
	IDirect3DVertexShader9 *vs;
	IDirect3DVertexDeclaration9 *decl;
};

#endif