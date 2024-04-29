#include "HWView2D.h"

CHWView2D::CHWView2D(CHWWorld *w, CHWTools *t, int x, int y) : CHWView(w, t)
{
	cx = x;
	cy = y;

	if(cx == 0 && cy == 1)
		cz = 2;
	else if(cx == 0 && cy == 2)
		cz = 1;
	else if(cx == 2 && cy == 1)
		cz = 0;

	zoom_speed = 500;
	move_speed = 500;
	pos.Set(0, 0, -1000);
}

void CHWView2D::ProcessInput(CHWWindow *win)
{
	CHWView::ProcessInput(win);

	if(win->key[KEY_LEFT] || win->key[KEY_A])
		pos.x -= move_speed * sf;
	if(win->key[KEY_RIGHT] || win->key[KEY_D])
		pos.x += move_speed * sf;
	if(win->key[KEY_UP] || win->key[KEY_W])
		pos.y += move_speed * sf;
	if(win->key[KEY_DOWN] || win->key[KEY_S])
		pos.y -= move_speed * sf;
	if(win->key[KEY_BUP])
		Zoom(int(zoom_speed * sf));
	if(win->key[KEY_BDOWN])
		Zoom(int(zoom_speed * -sf));

	if(win->hit[KEY_MLEFT])
	{
		Project2DOnGridPlane(mpos, &tmp_mov);
		SetToGrid(&tmp_mov);

		if(win->key[KEY_SPACE])
			SetBrushPoints();
	}

	if(win->release[KEY_MLEFT])
	{
		if(win->key[KEY_SHIFT])
			SelectBrushes();
		else if(win->key[KEY_CONTROL])
			SelectBrushPoints();
	}

	if(mdown)
	{
		if(win->key[KEY_CONTROL])
			MoveSelectedBrushPoints();
		else
			MoveSelectedBrushes();
	}

	if(win->hit[KEY_DELETE])
		world->building->DeleteSelectedBrushes();
	if(win->hit[KEY_C])
		world->building->DuplicateSelectedBrushes(gridsize);
}

void CHWView2D::SetBrushPoints()
{
	CVector3f p;
	CHWBrush brush;

	Project2DOnGridPlane(mpos, &p);
	SetToGrid(&p);
	draw_points.Add(p);

	switch(tools->bt)
	{
	case BT_BOX:
		switch(brush.CreateBlock(draw_points, (float)gridsize, cz))
		{
		case CRRES_NOTYET:
			break;

		case CRRES_IMPOSSIBLE:
			draw_points.GLDN()->Delete();
			break;

		case CRRES_OK:
			world->building->brushes.Add(brush);
			draw_points.Clear();
			break;
		}
		break;

	case BT_POLYHEDRON:
		switch(brush.CreatePolyhedron(draw_points, (float)gridsize, cz))
		{
		case CRRES_NOTYET:
			break;

		case CRRES_IMPOSSIBLE:
			draw_points.GLDN()->Delete();
			break;

		case CRRES_OK:
			world->building->brushes.Add(brush);
			draw_points.Clear();
			break;
		}
		break;
	}
}

void CHWView2D::Zoom(int zoom)
{
	pos.z += zoom;
	if(pos.z > 0)
		pos.z = 0;
}

void CHWView2D::Project2DOnGridPlane(const CVector2d &point, CVector3f *p3d)
{
	CVector3f p_near, p_far;
	CPlane plane(0, 0, -1, 0);

	Project2DTo3D(point, &p_near, &p_far);

	GetIntersectionPoint_lineseg_plane(p_near, p_far, plane, p3d);

	((float*)p3d)[cx] = p3d->x;
	((float*)p3d)[cy] = p3d->y;
	((float*)p3d)[cz] = 0;
}

#define POINT_SIZE 5

bool CHWView2D::PointInSelectionRect(const CVector3f &p_p)
{
	CVector2d v2d;
	CVector3f p;

	p.x = ((float*)&p_p)[cx];
	p.y = ((float*)&p_p)[cy];
	p.z = 0;

	if(!Project3DTo2D(p, &v2d))
		return false;

	if(((v2d.x >= mpos_down.x - POINT_SIZE) &&
		(v2d.y >= mpos_down.y - POINT_SIZE) &&
		(v2d.x <= mpos.x + POINT_SIZE) &&
		(v2d.y <= mpos.y + POINT_SIZE)) ||
		((v2d.x <= mpos_down.x + POINT_SIZE) &&
		(v2d.y >= mpos_down.y - POINT_SIZE) &&
		(v2d.x >= mpos.x - POINT_SIZE) &&
		(v2d.y <= mpos.y + POINT_SIZE)) ||
		((v2d.x <= mpos_down.x + POINT_SIZE) &&
		(v2d.y <= mpos_down.y + POINT_SIZE) &&
		(v2d.x >= mpos.x - POINT_SIZE) &&
		(v2d.y >= mpos.y - POINT_SIZE)) ||
		((v2d.x >= mpos_down.x - POINT_SIZE) &&
		(v2d.y <= mpos_down.y + POINT_SIZE) &&
		(v2d.x <= mpos.x + POINT_SIZE) &&
		(v2d.y >= mpos.y - POINT_SIZE)))
		return true;

	return false;
}

void CHWView2D::SelectBrushes()
{
	for(CNode<CHWBrush> *b_node = world->building->brushes.GFDN(); b_node; b_node = b_node->next)
	{
		CHWBrush *brush = b_node->GetData();

		for(int i = 0; i < brush->num_points; i++)
		{
			if(PointInSelectionRect(brush->point[i]))
			{
				brush->selected = true;
				break;
			}

			brush->selected = false;
		}
	}
}

void CHWView2D::SelectBrushPoints()
{
	for(CNode<CHWBrush> *b_node=world->building->brushes.GFDN(); b_node; b_node = b_node->next)
	{
		CHWBrush *brush = b_node->GetData();

		for(int i = 0; i < brush->num_points; i++)
		{
			if(PointInSelectionRect(brush->point[i]))
				brush->point_selected[i] = true;
			else
				brush->point_selected[i] = false;
		}
	}
}

void CHWView2D::MoveSelectedBrushes()
{
	CVector3f p, v;

	Project2DOnGridPlane(mpos, &p);
	SetToGrid(&p);

	if(p.Equal(tmp_mov))
		return;

	v = p - tmp_mov;
	tmp_mov = p;

	for(CNode<CHWBrush> *b_node = world->building->brushes.GFDN(); b_node; b_node = b_node->next)
	{
		CHWBrush *brush = b_node->GetData();

		if(!brush->selected)
			continue;

		for(int i = 0; i < brush->num_points; i++)
		{
			brush->point[i] += v;
			SetToGrid(&brush->point[i]);
		}

		brush->CalcPlanes();
	}
}

void CHWView2D::MoveSelectedBrushPoints()
{
	CVector3f p, v;

	Project2DOnGridPlane(mpos, &p);
	SetToGrid(&p);

	if(p.Equal(tmp_mov))
		return;

	v = p - tmp_mov;
	tmp_mov = p;

	for(CNode<CHWBrush> *b_node=world->building->brushes.GFDN(); b_node; b_node = b_node->next)
	{
		CHWBrush *brush = b_node->GetData();

		for(int i = 0; i < brush->num_points; i++)
		{
			if(brush->point_selected[i])
				brush->point[i] += v;
		}

		brush->CalcPlanes();

		if(brush->CheckIfConvex())
		{
			for(i = 0; i < brush->num_points; i++)
			{
				if(brush->point_selected[i])
					SetToGrid(&brush->point[i]);
			}
		}
		else
		{
			for(i = 0; i < brush->num_points; i++)
			{
				if(brush->point_selected[i])
					brush->point[i] -= v;
			}
		}
	}
}

void CHWView2D::RenderGrid(CRenderDevice *rd)
{
	rd->SetTexture(0, NULL);
	rd->SetVertexShader(rd->vs_diffuse);

	float grid_wh = float(gridsize * 100);
	rd->Diffuse_RGB(100, 100, 100);
	rd->BeginLines(RND_DIFFUSE);
	for(float f = -grid_wh; f < grid_wh; f += gridsize)
	{
		rd->Vertex(-grid_wh, f);
		rd->Vertex( grid_wh, f);

		rd->Vertex(f, -grid_wh);
		rd->Vertex(f,  grid_wh);
	}
	rd->EndLines();
}

void CHWView2D::RenderSelectionRect(CRenderDevice *rd)
{
	if(mdown)
	{
		rd->SetVertexShader(rd->vs_diffuse);
		rd->Diffuse_RGB(0, 0, 255);
		rd->BeginLines(RND_DIFFUSE);

		rd->Vertex(mpos_down.x, mpos_down.y);
		rd->Vertex(mpos.x, mpos_down.y);

		rd->Vertex(mpos.x, mpos_down.y);
		rd->Vertex(mpos.x, mpos.y);

		rd->Vertex(mpos.x, mpos.y);
		rd->Vertex(mpos_down.x, mpos.y);

		rd->Vertex(mpos_down.x, mpos.y);
		rd->Vertex(mpos_down.x, mpos_down.y);

		rd->EndLines();
	}
}

void CHWView2D::RenderPoint(const CVector2f &p, CRenderDevice *rd)
{
	float size = 0.005f * -pos.z;

	rd->BeginPoly(RND_DIFFUSE);
	rd->Vertex(p.x - size, p.y - size);
	rd->Vertex(p.x - size, p.y + size);
	rd->Vertex(p.x + size, p.y + size);
	rd->Vertex(p.x + size, p.y - size);
	rd->EndPoly();
}

void CHWView2D::RenderDrawPoints(CRenderDevice *rd)
{
	rd->SetVertexShader(rd->vs_diffuse);
	rd->Diffuse_RGB(255, 255, 0);

	rd->BeginLines(RND_DIFFUSE);
	for(CNode<CVector3f> *v_node = draw_points.GFDN(); v_node; v_node = v_node->next)
	{
		if(!v_node->next)
			break;

		CVector3f *v1 = v_node->GetData();
		CVector3f *v2 = v_node->next->GetData();

		rd->Vertex(((float*)v1)[cx], ((float*)v1)[cy]);
		rd->Vertex(((float*)v2)[cx], ((float*)v2)[cy]);
	}
	rd->EndLines();

	for(v_node = draw_points.GFDN(); v_node; v_node = v_node->next)
	{
		CVector3f *v = v_node->GetData();
		RenderPoint(CVector2f(((float*)v)[cx], ((float*)v)[cy]), rd);
	}
}

void CHWView2D::RenderBrush(const CHWBrush &brush, CRenderDevice *rd)
{
	rd->SetVertexShader(rd->vs_diffuse);
	if(brush.selected)
		rd->Diffuse_RGB(255, 0, 0);
	else
		rd->Diffuse_RGB(255, 255, 255);

	rd->BeginLines(RND_DIFFUSE);
	for(int i = 0; i < brush.num_sides; i++)
	{
		rd->Vertex(((float*)&brush.point[brush.side[i].point[0]])[cx], ((float*)&brush.point[brush.side[i].point[0]])[cy]);
		rd->Vertex(((float*)&brush.point[brush.side[i].point[1]])[cx], ((float*)&brush.point[brush.side[i].point[1]])[cy]);

		rd->Vertex(((float*)&brush.point[brush.side[i].point[1]])[cx], ((float*)&brush.point[brush.side[i].point[1]])[cy]);
		rd->Vertex(((float*)&brush.point[brush.side[i].point[2]])[cx], ((float*)&brush.point[brush.side[i].point[2]])[cy]);

		rd->Vertex(((float*)&brush.point[brush.side[i].point[2]])[cx], ((float*)&brush.point[brush.side[i].point[2]])[cy]);
		rd->Vertex(((float*)&brush.point[brush.side[i].point[0]])[cx], ((float*)&brush.point[brush.side[i].point[0]])[cy]);
	}
	rd->EndLines();

	for(i = 0; i < brush.num_points; i++)
	{
		if(brush.point_selected[i])
			rd->Diffuse_RGB(255, 255, 0);
		else
		{
			if(brush.selected)
				rd->Diffuse_RGB(255, 0, 0);
			else
				rd->Diffuse_RGB(255, 255, 255);
		}

		RenderPoint(CVector2f(((float*)&brush.point[i])[cx], ((float*)&brush.point[i])[cy]), rd);
	}
}

void CHWView2D::Render3D(CRenderDevice *rd)
{
	rd->cam.pos = pos;
	rd->cam.omt.Identity();
	rd->cam.UpdateViewMatrix();
	rd->SetTransform3D();

	RenderGrid(rd);

	CHWView::Render3D(rd);

	RenderDrawPoints(rd);

	for(CNode<CHWBrush> *b_node = world->building->brushes.GFDN(); b_node; b_node = b_node->next)
		RenderBrush(*b_node->GetData(), rd);
}

void CHWView2D::Render2D(CRenderDevice *rd)
{
	RenderSelectionRect(rd);
}