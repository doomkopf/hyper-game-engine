#ifndef HWTERRAIN_H_
#define HWTERRAIN_H_

#include "../HGEngine/Level0/Texture.h"
#include "../stuff/Mathtypes.h"

class CTriNode;
class CHWTerrainPatch;

#define NODEPOOL_SIZE 16384//131072

class CHMNormal
{
public:
	CVector3f n;
	unsigned char light;
	char c;
	bool calced;
};

class CHWTerrain
{
public:
	CHWTerrain();
	~CHWTerrain();
	void New();
	void Destruct();
	void CreateVarianceTrees();
	void ResetPatches();
	void Tesselate(const CVector3f &);
	void CalcNormals();
	void CalcFlatShades();
	void CalcGouraudShades();
	static CTriNode *GetNodeFromPool();

	int heights_space;
	int num_heights_per_side;
	int patch_size;
	int max_variance_depth;
	short *heightmap;
	CHMNormal *hm_normal;
	unsigned char *lightmap;

	int num_patches_per_side;
	CHWTerrainPatch *patch;

	short min_v;

	CTexture *tex;

protected:
	void NormalizeVertexNormals();

	static CTriNode *node_pool;
	static int node_pool_idx;
};

#endif