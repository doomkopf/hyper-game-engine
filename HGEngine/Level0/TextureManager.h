#ifndef TEXTUREMANAGER_H_
#define TEXTUREMANAGER_H_

#include "Texture.h"
#include "RenderDevice.h"

#define MAX_TEXTURES 256

class CTextureManager
{
	friend class CRenderDevice;
public:
	CTextureManager();
	~CTextureManager();
	virtual bool LoadFromFile(const char *);
	void Destruct();

	int num_tex;
	CTexture *tex[MAX_TEXTURES];

protected:
	static CRenderDevice *rd;
};

#endif