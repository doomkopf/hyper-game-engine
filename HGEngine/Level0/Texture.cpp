#include "Texture.h"
#include <d3dx9.h>

IDirect3DDevice9 *CTexture::dev;

CTexture::CTexture()
{
	tex = NULL;
}

CTexture::~CTexture()
{
	Destruct();
}

bool CTexture::LoadFromFile(const char *file)
{
	D3DXIMAGE_INFO io;
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
		&tex)))
		return false;

	return true;
}

void *CTexture::Lock(int *x, int *y)
{
	D3DLOCKED_RECT rect;
	tex->LockRect(0, &rect, NULL, 0);
	if(x && y)
		*x = *y = rect.Pitch;

	return rect.pBits;
}

void CTexture::Unlock()
{
	tex->UnlockRect(0);
}

void CTexture::Destruct()
{
	if(tex)
	{
		tex->Release();
		tex = NULL;
	}
}