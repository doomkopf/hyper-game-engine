#include "VertexBuffer.h"
#include "RenderDevice.h"

CRenderDevice *CVertexBuffer::rd;

CVertexBuffer::CVertexBuffer()
{
	vb = NULL;
}

CVertexBuffer::~CVertexBuffer()
{
	Destruct();
}

bool CVertexBuffer::Create(unsigned int s, bool dyn)
{
	if(FAILED(rd->dev->CreateVertexBuffer(
		s,
		dyn ? D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY : D3DUSAGE_WRITEONLY,
		0,
		dyn ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED,
		&vb,
		NULL)))
		return false;

	return true;
}

void CVertexBuffer::Lock(void **p)
{
	vb->Lock(0, 0, p, 0);
}

void CVertexBuffer::Unlock()
{
	vb->Unlock();
}

void CVertexBuffer::Destruct()
{
	if(vb)
	{
		vb->Release();
		vb = NULL;
	}
}