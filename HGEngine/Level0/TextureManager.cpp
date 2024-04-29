#include "TextureManager.h"

CRenderDevice *CTextureManager::rd = NULL;

CTextureManager::CTextureManager()
{
	for(int i = 0; i < MAX_TEXTURES; i++)
		tex[i] = NULL;

	num_tex = 0;
}

CTextureManager::~CTextureManager()
{
	Destruct();
}

bool CTextureManager::LoadFromFile(const char *file)
{
	if(num_tex >= MAX_TEXTURES - 1)
		return false;

	tex[num_tex] = rd->CreateTextureFromFile(file);
	if(!tex[num_tex])
		return false;

	num_tex++;

	return true;
}

void CTextureManager::Destruct()
{
	for(int i = 0; i < MAX_TEXTURES; i++)
	{
		if(tex[i])
		{
			delete tex[i];
			tex[i] = NULL;
		}
	}

	num_tex = 0;
}