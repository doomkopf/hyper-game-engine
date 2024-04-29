#include "HWViewTerrain.h"
#include "HWTerrainPatch.h"

CQuaternion CHWViewTerrain::quat;
CMatrix3x3 CHWViewTerrain::m;

CHWViewTerrain::CHWViewTerrain(CHWWorld *w, CHWTools *to, CHWTextureManager *t) : CHWView(w, to)
{
	textures = t;
	pos.Set(0, 0, 0);
	right.Set(1, 0, 0);
	up.Set(0, 1, 0);
	dir.Set(0, 0, 1);
	speed = 150;

	wireframe = false;
	shift_radius = 160;
	shift_step = 2;

	world->terrain.New(); // temp
}

void CHWViewTerrain::UpdateCamera(CCamera *cam)
{
	cam->pos = pos;
	cam->right = right;
	cam->up = up;
	cam->dir = dir;

	cam->UpdateViewMatrix();
}

void CHWViewTerrain::MoveFw()
{
	pos += dir * speed * sf;
}

void CHWViewTerrain::MoveBw()
{
	pos -= dir * speed * sf;
}

void CHWViewTerrain::MoveL()
{
	pos -= right * speed * sf;
}

void CHWViewTerrain::MoveR()
{
	pos += right * speed * sf;
}

void CHWViewTerrain::RotateX(float r)
{
	quat.AxisAngle(right, r);
	quat.GetMatrix(&m);

	VectorRotate(&up, m);
	VectorRotate(&dir, m);
}

void CHWViewTerrain::RotateY(float r)
{
	quat.AxisAngle(CVector3f(0, 1, 0), r);
	quat.GetMatrix(&m);

	VectorRotate(&right, m);
	VectorRotate(&up, m);
	VectorRotate(&dir, m);
}

#define TEX_SCALE 32

void CHWViewTerrain::ProcessInput(CHWWindow *win)
{
	CHWView::ProcessInput(win);

	if(win->key[KEY_MRIGHT])
	{
		if(win->key[KEY_W] || win->key[KEY_UP])
			MoveFw();
		if(win->key[KEY_S] || win->key[KEY_DOWN])
			MoveBw();
		if(win->key[KEY_A] || win->key[KEY_LEFT])
			MoveL();
		if(win->key[KEY_D] || win->key[KEY_RIGHT])
			MoveR();

		RotateX(-(win->my - win->lmy) * 0.005f);
		RotateY(-(win->mx - win->lmx) * 0.005f);

		win->lmx = win->mx;
		win->lmy = win->my;
	}
	else
	{
		if(win->key[KEY_W] || win->key[KEY_UP])
			MoveFw();
		if(win->key[KEY_S] || win->key[KEY_DOWN])
			MoveBw();
		if(win->key[KEY_A] || win->key[KEY_LEFT])
			RotateY(2 * sf);
		if(win->key[KEY_D] || win->key[KEY_RIGHT])
			RotateY(-2 * sf);
		if(win->key[KEY_BDOWN])
			RotateX(-2 * sf);
		if(win->key[KEY_BUP])
			RotateX(2 * sf);
	}

	if(win->hit[KEY_PLUS])
	{
		AddDetail(1);
	}
	if(win->hit[KEY_MINUS])
	{
		AddDetail(-1);
	}

	if(win->key[KEY_MLEFT])
	{
		ShiftHeightmap(!win->key[KEY_SPACE]);
	}

	if(win->hit[KEY_MMIDDLE])
		wireframe = !wireframe;
}

void CHWViewTerrain::AddDetail(int dt)
{
	world->terrain.min_v += dt;
	if(world->terrain.min_v < 0)
		world->terrain.min_v = 0;
}

void CHWViewTerrain::ShiftHeightmap(bool up)
{
	CVector3f p1, p2, p;
	Project2DTo3D(mpos, &p1, &p2);

	CPlane plane;
	plane.normal.Set(0, 1, 0);
	plane.d = 0;
	if(GetIntersectionPoint_lineseg_plane(p1, p2, plane, &p))
	{
		CVector2d ph, pi((int)p.x, (int)-p.z);
		for(int i = 0, num_patches = world->terrain.num_patches_per_side * world->terrain.num_patches_per_side; i < num_patches; i++)
		{
			CHWTerrainPatch *patch = &world->terrain.patch[i];

			if(!RectIntersectsCircle(patch->min * (float)world->terrain.heights_space, patch->max * (float)world->terrain.heights_space, pi, shift_radius))
				continue;

			for(int y = patch->min.y; y < patch->max.y; y++)
			{
				for(int x = patch->min.x; x < patch->max.x; x++)
				{
					if(x == 0 || x == world->terrain.num_heights_per_side - 1 ||
						y == 0 || y == world->terrain.num_heights_per_side - 1)
						continue;

					ph.x = x * world->terrain.heights_space;
					ph.y = y * world->terrain.heights_space;

					float d = (ph - pi).Length();
					if(d <= shift_radius)
					{
						short v = short((shift_radius - d) * shift_step * (up ? sf : -sf));
						int idx = (y * world->terrain.num_heights_per_side) + x;
						world->terrain.heightmap[idx] += v;
						if(world->terrain.heightmap[idx] < 0)
							world->terrain.heightmap[idx] = 0;
					}
				}
			}
			patch->CreateVarianceTree();
		}
	}
}

void CHWViewTerrain::Render3D(CRenderDevice *rd)
{
	CHWView::Render3D(rd);

	UpdateCamera(&rd->cam);
	rd->SetTransform3D();

	if(wireframe)
		rd->WIREFRAME_Enable();
	else
		rd->WIREFRAME_Disable();

	world->terrain.ResetPatches();
	world->terrain.Tesselate(rd->cam.pos);
	world->terrain.CalcNormals();
	//world->terrain.CalcFlatShades();
	world->terrain.CalcGouraudShades();

	rd->SetVertexShader(rd->vs_diffuse_tex0);
	//rd->Diffuse_RGB(255, 0, 0);
	rd->SetTexture(0, world->terrain.tex);
	for(int i = 0, num_patches = world->terrain.num_patches_per_side * world->terrain.num_patches_per_side; i < num_patches; i++)
	{
		rd->BeginTriangles(RND_TEX0 | RND_DIFFUSE);

		RenderTriNodes(world->terrain.patch[i].n1,
			CVector2d(world->terrain.patch[i].min.x, world->terrain.patch[i].max.y),
			CVector2d(world->terrain.patch[i].max.x, world->terrain.patch[i].min.y),
			CVector2d(world->terrain.patch[i].min.x, world->terrain.patch[i].min.y),
			world->terrain.patch[i].hm, world->terrain.patch[i].hm_per_side, rd);
		RenderTriNodes(world->terrain.patch[i].n2,
			CVector2d(world->terrain.patch[i].max.x, world->terrain.patch[i].min.y),
			CVector2d(world->terrain.patch[i].min.x, world->terrain.patch[i].max.y),
			CVector2d(world->terrain.patch[i].max.x, world->terrain.patch[i].max.y),
			world->terrain.patch[i].hm, world->terrain.patch[i].hm_per_side, rd);

		rd->EndTriangles();
	}
}

void CHWViewTerrain::Render2D(CRenderDevice *rd)
{
}

void CHWViewTerrain::RenderTriNodes(CTriNode *n,
									const CVector2d &left,
									const CVector2d &right,
									const CVector2d &apex,
									const short *hm,
									int hm_per_side,
									CRenderDevice *rd)
{
	if(n->left)
	{
		CVector2d center;
		center.x = (left.x + right.x) >> 1;
		center.y = (left.y + right.y) >> 1;

		RenderTriNodes(n->left, apex, left, center, hm, hm_per_side, rd);
		RenderTriNodes(n->right, right, apex, center, hm, hm_per_side, rd);
	}
	else
	{
		float lefth = hm[(left.y * world->terrain.num_heights_per_side) + left.x];
		float righth = hm[(right.y * world->terrain.num_heights_per_side) + right.x];
		float apexh = hm[(apex.y * world->terrain.num_heights_per_side) + apex.x];

		unsigned char l = world->terrain.hm_normal[(left.y * world->terrain.num_heights_per_side) + left.x].light;
		rd->Diffuse_RGB(l, l, l);
		rd->Texcoord(((float)left.x / world->terrain.num_heights_per_side) * TEX_SCALE,
			((float)left.y / world->terrain.num_heights_per_side) * TEX_SCALE);
		rd->Vertex((float)left.x * world->terrain.heights_space, lefth, (float)-left.y * world->terrain.heights_space);

		l = world->terrain.hm_normal[(apex.y * world->terrain.num_heights_per_side) + apex.x].light;
		rd->Diffuse_RGB(l, l, l);
		rd->Texcoord(((float)apex.x / world->terrain.num_heights_per_side) * TEX_SCALE,
			((float)apex.y / world->terrain.num_heights_per_side) * TEX_SCALE);
		rd->Vertex((float)apex.x * world->terrain.heights_space, apexh, (float)-apex.y * world->terrain.heights_space);

		l = world->terrain.hm_normal[(right.y * world->terrain.num_heights_per_side) + right.x].light;
		rd->Diffuse_RGB(l, l, l);
		rd->Texcoord(((float)right.x / world->terrain.num_heights_per_side) * TEX_SCALE,
			((float)right.y / world->terrain.num_heights_per_side) * TEX_SCALE);
		rd->Vertex((float)right.x * world->terrain.heights_space, righth, (float)-right.y * world->terrain.heights_space);
	}
}