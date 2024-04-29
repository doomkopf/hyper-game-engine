#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <d3d9.h>

class CTexture
{
	friend class CRenderDevice;
public:
	CTexture();
	~CTexture();
	bool LoadFromFile(const char *);
	void *Lock(int *, int *);
	void Unlock();
	void Destruct();

protected:
	IDirect3DTexture9 *tex;
	static IDirect3DDevice9 *dev;
};

#endif