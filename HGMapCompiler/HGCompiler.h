#ifndef HGCOMPILER_H_
#define HGCOMPILER_H_

#include "../stuff/FileBuf.h"
#include "BSP.h"

class CHGCompiler
{
public:
	CHGCompiler();
	~CHGCompiler();

	bool LoadRAWFromFile(const char *);
	void SaveFile(const char *);
	void DoStuff();

protected:
	void ReadPolys();
	void ReadTextures();

	void SaveBSP(CBSPNode *);
	void SaveTextures();

	CBSP bsp;
	int num_textures;
	unsigned char *texture_seg;
	CFileBuf fb;
};

#endif