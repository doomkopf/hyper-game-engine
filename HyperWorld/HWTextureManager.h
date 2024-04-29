#ifndef HWTEXTUREMANAGER_H_
#define HWTEXTUREMANAGER_H_

#include "../HGEngine/Level0/TextureManager.h"

class CHWTextureManager : public CTextureManager
{
public:
	CHWTextureManager();
	bool Init();
	virtual ~CHWTextureManager();
	virtual void Destruct();
	virtual bool LoadFromFile(const char *);

	char name[MAX_TEXTURES][64];
	int cur_tex;
	CTexture *def_tex;
};

#endif