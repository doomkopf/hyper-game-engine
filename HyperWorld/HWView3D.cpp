#include "HWView3D.h"
#include "../HGEngine/Level1/geom.h"

CQuaternion CHWView3D::quat;
CMatrix3x3 CHWView3D::m;

CHWView3D::CHWView3D(CHWWorld *w, CHWTools *to, CHWTextureManager *t) : CHWView(w, to)
{
	textures = t;

	pos.Set(0, 0, 0);
	right.Set(1, 0, 0);
	up.Set(0, 1, 0);
	dir.Set(0, 0, 1);
	speed = 150;
}

void CHWView3D::UpdateCamera(CCamera *cam)
{
	cam->pos = pos;
	cam->right = right;
	cam->up = up;
	cam->dir = dir;

	cam->UpdateViewMatrix();
}

void CHWView3D::MoveFw()
{
	pos += dir * speed * sf;
}

void CHWView3D::MoveBw()
{
	pos -= dir * speed * sf;
}

void CHWView3D::MoveL()
{
	pos -= right * speed * sf;
}

void CHWView3D::MoveR()
{
	pos += right * speed * sf;
}

void CHWView3D::RotateX(float r)
{
	quat.AxisAngle(right, r);
	quat.GetMatrix(&m);

	VectorRotate(&up, m);
	VectorRotate(&dir, m);
}

void CHWView3D::RotateY(float r)
{
	quat.AxisAngle(CVector3f(0, 1, 0), r);
	quat.GetMatrix(&m);

	VectorRotate(&right, m);
	VectorRotate(&up, m);
	VectorRotate(&dir, m);
}

void CHWView3D::ProcessInput(CHWWindow *win)
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

		if(win->hit[KEY_MLEFT])
		{
			UpdateTmpMovPoint();

			if(win->key[KEY_SHIFT])
			{
				SelectBrushes();
			}
		}

		if(mdown)
		{
			MoveSelectedBrushes();
		}
	}

	if(win->hit[KEY_DELETE])
		world->building->DeleteSelectedBrushes();
	if(win->hit[KEY_C])
		world->building->DuplicateSelectedBrushes(gridsize);
}

void CHWView3D::UpdateTmpMovPoint()
{
	projection_plane.normal = dir;
	if(abs_f(projection_plane.normal.x) > abs_f(projection_plane.normal.y) && abs_f(projection_plane.normal.x) > abs_f(projection_plane.normal.z))
	{
		projection_plane.normal.y = 0;
		projection_plane.normal.z = 0;
	}
	else if(abs_f(projection_plane.normal.y) > abs_f(projection_plane.normal.x) && abs_f(projection_plane.normal.y) > abs_f(projection_plane.normal.z))
	{
		projection_plane.normal.x = 0;
		projection_plane.normal.z = 0;
	}
	else
	{
		projection_plane.normal.x = 0;
		projection_plane.normal.y = 0;
	}
	projection_plane.normal.Normalize();
	CVector3f pnear, pfar, p = pos + (projection_plane.normal * 512);
	projection_plane.normal.Inverse();
	projection_plane.d = -DotProduct(p, projection_plane.normal);
	Project2DTo3D(mpos, &pnear, &pfar);
	GetIntersectionPoint_lineseg_plane(pnear, pfar, projection_plane, &tmp_mov);

	SetToGrid(&tmp_mov);
}

void CHWView3D::SelectBrushes()
{
	CVector3f pnear, pfar, p, p1, p2;
	Project2DTo3D(mpos, &pnear, &pfar);
	float min_dist;
	CHWBrush *min_dist_brush = NULL;

	for(CNode<CHWBrush> *b_node = world->building->brushes.GFDN(); b_node; b_node = b_node->next)
	{
		CHWBrush *brush = b_node->GetData();

		p1 = pnear;
		p2 = pfar;
		for(int i = 0; i < brush->num_sides; i++)
		{
			switch(ClassifyLineseg(p1, p2, brush->side[i].plane, EPSILON))
			{
			case FRONT:
			case COPLANAR:
				i = brush->num_sides;
				break;

			case BACK:
				if(i == brush->num_sides - 1)
				{
					//sel = true;
					if(!min_dist_brush) // first time found brush
					{
						min_dist = (p1 - pnear).Length();
						min_dist_brush = brush;
					}
					else
					{
						float d = (p1 - pnear).Length();
						if(d < min_dist)
						{
							min_dist = d;
							min_dist_brush = brush;
						}
					}
				}
				break;

			case SPANNING_FB:
				if(i == brush->num_sides - 1)
				{
					//sel = true;
					if(!min_dist_brush) // first time found brush
					{
						min_dist = (p1 - pnear).Length();
						min_dist_brush = brush;
					}
					else
					{
						float d = (p1 - pnear).Length();
						if(d < min_dist)
						{
							min_dist = d;
							min_dist_brush = brush;
						}
					}
					break;
				}
				GetIntersectionPoint_lineseg_plane(p1, p2, brush->side[i].plane, &p);
				p1 = p;
				break;

			case SPANNING_BF:
				if(i == brush->num_sides - 1)
				{
					//sel = true;
					if(!min_dist_brush) // first time found brush
					{
						min_dist = (p1 - pnear).Length();
						min_dist_brush = brush;
					}
					else
					{
						float d = (p1 - pnear).Length();
						if(d < min_dist)
						{
							min_dist = d;
							min_dist_brush = brush;
						}
					}
					break;
				}
				GetIntersectionPoint_lineseg_plane(p1, p2, brush->side[i].plane, &p);
				p2 = p;
				break;
			}
		}
	}
	if(min_dist_brush)
		min_dist_brush->selected = !min_dist_brush->selected;
}

void CHWView3D::MoveSelectedBrushes()
{
	CVector3f pnear, pfar, p, v;
	Project2DTo3D(mpos, &pnear, &pfar);
	GetIntersectionPoint_lineseg_plane(pnear, pfar, projection_plane, &p);
	SetToGrid(&p);

	if(p.Equal(tmp_mov))
		return;

	v = p - tmp_mov;
	tmp_mov = p;

	for(CNode<CHWBrush> *b_node = world->building->brushes.GFDN(); b_node; b_node = b_node->next)
	{
		CHWBrush *brush = b_node->GetData();

		if(brush->selected)
		{
			for(int j = 0; j < brush->num_points; j++)
			{
				brush->point[j] += v;
				SetToGrid(&brush->point[j]);
			}
			brush->CalcPlanes();
		}
	}
}

void CHWView3D::Render3D(CRenderDevice *rd)
{
	CHWView::Render3D(rd);

	UpdateCamera(&rd->cam);
	rd->SetTransform3D();

	for(CNode<CHWBrush> *b_node = world->building->brushes.GFDN(); b_node; b_node = b_node->next)
		RenderBrush(*b_node->GetData(), rd);
}

void CHWView3D::Render2D(CRenderDevice *rd)
{
}

void CHWView3D::RenderBrush(const CHWBrush &brush, CRenderDevice *rd)
{
	rd->SetVertexShader(rd->vs_diffuse_tex0);

	if(brush.selected)
		rd->Diffuse_RGB(255, 0, 0);
	else
		rd->Diffuse_RGB(255, 255, 255);

	rd->SetTexture(0, brush.tex == -1 ? textures->def_tex : textures->tex[brush.tex]);

	rd->BeginTriangles(RND_DIFFUSE | RND_TEX0);

	for(int i = 0; i < brush.num_sides; i++)
	{
		rd->Texcoord(brush.side[i].texcoord[0].u, brush.side[i].texcoord[0].v);
		rd->Vertex(brush.point[brush.side[i].point[0]]);
		rd->Texcoord(brush.side[i].texcoord[1].u, brush.side[i].texcoord[1].v);
		rd->Vertex(brush.point[brush.side[i].point[1]]);
		rd->Texcoord(brush.side[i].texcoord[2].u, brush.side[i].texcoord[2].v);
		rd->Vertex(brush.point[brush.side[i].point[2]]);
	}

	rd->EndTriangles();
}