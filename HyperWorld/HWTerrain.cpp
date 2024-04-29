#include "HWTerrain.h"
#include "HWTerrainPatch.h"
#include "../stuff/FileBuf.h"

int CHWTerrain::node_pool_idx = 0;
CTriNode *CHWTerrain::node_pool;

CHWTerrain::CHWTerrain()
{
	patch = NULL;
	heightmap = NULL;
	hm_normal = NULL;
	lightmap = NULL;
	node_pool = new CTriNode[NODEPOOL_SIZE];
	tex = new CTexture;

	ZeroMemory(node_pool, sizeof(CTriNode) * NODEPOOL_SIZE);

	min_v = 0;
}

CHWTerrain::~CHWTerrain()
{
	Destruct();
}

CTriNode *CHWTerrain::GetNodeFromPool()
{
	static bool b = false;
	if(node_pool_idx < NODEPOOL_SIZE)
	{
		//node_pool[node_pool_idx].Destruct();
		return &node_pool[node_pool_idx++];
	}
	if(!b)
		MessageBox(NULL, "Node Pool Overrun", NULL, MB_OK);
	b = true;
	return NULL;
	//return node_pool_idx < NODEPOOL_SIZE ? &node_pool[node_pool_idx++] : NULL;
}

void CHWTerrain::New()
{
	// temp hard coded values
	num_heights_per_side = 512;
	patch_size = 32;
	heights_space = 8;
	max_variance_depth = 512;

	heightmap = new short[num_heights_per_side * num_heights_per_side];
	hm_normal = new CHMNormal[num_heights_per_side * num_heights_per_side];
	/*CFileBuf fb;
	fb.Open("test.tga", true);
	fb.Jump(18, J_START);
	for(int i = 0; i < num_heights_per_side * num_heights_per_side; i++)
	{
		unsigned char c;
		fb.Read(&c, 1);
		heightmap[i] = c;
	}
	fb.Close();*/
	ZeroMemory(heightmap, num_heights_per_side * num_heights_per_side * sizeof(short));

	num_patches_per_side = num_heights_per_side / patch_size;
	patch = new CHWTerrainPatch[num_patches_per_side * num_patches_per_side];

	tex->LoadFromFile("grass1.tga");

	int num_patches = num_patches_per_side * num_patches_per_side;
	for(int i = 0, xmin, xmax, ymin = -patch_size, ymax; i < num_patches; i++)
	{
		if(((i + num_patches_per_side) % num_patches_per_side) == 0)
		{
			xmin = 0;
			xmax = patch_size;

			ymin += patch_size;
			ymax = ymin + patch_size;
		}
		else
		{
			xmin += patch_size;
			xmax += patch_size;
		}

		patch[i].Init(heightmap, hm_normal,
			((i + num_patches_per_side) % num_patches_per_side) == 0 ? NULL : &patch[i - 1],
			((i + num_patches_per_side + 1) % num_patches_per_side) == 0 ? NULL : &patch[i + 1],
			i < num_patches_per_side ? NULL : &patch[i - num_patches_per_side],
			i >= num_patches_per_side * (num_patches_per_side - 1) ? NULL : &patch[i + num_patches_per_side],
			num_heights_per_side, max_variance_depth, xmin, ymin,
			((i + 1 + num_patches_per_side) % num_patches_per_side) == 0 ? xmax - 1 : xmax,
			i >= (num_patches_per_side * (num_patches_per_side - 1)) ? ymax - 1 : ymax);
	}

	CreateVarianceTrees();
}

void CHWTerrain::CreateVarianceTrees()
{
	for(int i = 0, num_patches = num_patches_per_side * num_patches_per_side; i < num_patches; i++)
	{
		patch[i].CreateVarianceTree();
	}
}

void CHWTerrain::ResetPatches()
{
	node_pool_idx = 0;
	ZeroMemory(node_pool, sizeof(CTriNode) * NODEPOOL_SIZE);
	ZeroMemory(hm_normal, sizeof(CHMNormal) * num_heights_per_side * num_heights_per_side);

	for(int i = 0, num_patches = num_patches_per_side * num_patches_per_side; i < num_patches; i++)
	{
		patch[i].Reset();
	}
}

void CHWTerrain::Tesselate(const CVector3f &cam_pos)
{
	for(int i = 0, num_patches = num_patches_per_side * num_patches_per_side; i < num_patches; i++)
	{
		patch[i].Tessellate(cam_pos, heights_space, min_v);
	}
}

void CHWTerrain::CalcNormals()
{
	for(int i = 0, num_patches = num_patches_per_side * num_patches_per_side; i < num_patches; i++)
	{
		patch[i].CalcNormals();
	}
}

void CHWTerrain::CalcFlatShades()
{
	for(int i = 0, num_patches = num_patches_per_side * num_patches_per_side; i < num_patches; i++)
	{
		patch[i].CalcFlatShade();
	}
}

void CHWTerrain::NormalizeVertexNormals()
{
	for(int i = 0, num_heights = num_heights_per_side * num_heights_per_side; i < num_heights; i++)
	{
		if(hm_normal[i].c > 0)
			hm_normal[i].n.Normalize();
	}
}

void CHWTerrain::CalcGouraudShades()
{
	for(int i = 0, num_patches = num_patches_per_side * num_patches_per_side; i < num_patches; i++)
		patch[i].FindVertexNormals();

	NormalizeVertexNormals();

	for(i = 0, num_patches = num_patches_per_side * num_patches_per_side; i < num_patches; i++)
		patch[i].CalcGouraudShade();
}

void CHWTerrain::Destruct()
{
	delete tex;

	if(patch)
	{
		delete [] patch;
		patch = NULL;
	}

	if(lightmap)
	{
		delete [] lightmap;
		lightmap = NULL;
	}

	if(hm_normal)
	{
		delete [] hm_normal;
		hm_normal = NULL;
	}

	if(heightmap)
	{
		delete [] heightmap;
		heightmap = NULL;
	}

	delete [] node_pool;
}