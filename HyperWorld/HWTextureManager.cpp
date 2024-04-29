#include "HWTextureManager.h"
#include "notex.h"

CHWTextureManager::CHWTextureManager()
{
	cur_tex = 0;
	def_tex = new CTexture;
}

CHWTextureManager::~CHWTextureManager()
{
	Destruct();
}

void CHWTextureManager::Destruct()
{
	if(def_tex)
	{
		delete def_tex;
		def_tex = NULL;
	}

	CTextureManager::Destruct();
}

bool CHWTextureManager::Init()
{
	def_tex = rd->CreateTexture(NOTEX_WIDTH, NOTEX_HEIGHT, (rgb_t*)notex);
	if(!def_tex)
		return false;

	return true;
}

bool CHWTextureManager::LoadFromFile(const char *file)
{
	strcpy(name[num_tex], file);
	return CTextureManager::LoadFromFile(file);
}