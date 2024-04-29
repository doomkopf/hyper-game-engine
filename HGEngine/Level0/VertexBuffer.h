#ifndef VERTEXBUFFER_H_
#define VERTEXBUFFER_H_

#include <d3d9.h>

class CRenderDevice;

class CVertexBuffer
{
	friend class CRenderDevice;
public:
	CVertexBuffer();
	~CVertexBuffer();
	bool Create(unsigned int, bool);
	void Lock(void **);
	void Unlock();
	void Destruct();

protected:
	static CRenderDevice *rd;
	IDirect3DVertexBuffer9 *vb;
};

#endif