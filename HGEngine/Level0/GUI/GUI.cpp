#include "GUI.h"

CGUI::CGUI()
{
	win = NULL;
	focus_window = NULL;
	num = 0;
}

CGUI::~CGUI()
{
	Destruct();
}

bool CGUI::LoadTextures(const char *textures)
{
	char tex[64];

	for(int i = 0, j = 0; ; i++, j++)
	{
		tex[j] = textures[i];

		if(tex[j] == '|')
		{
			tex[j] = 0;
			if(!tex_mgr.LoadFromFile(tex))
				return false;

			j = -1;
		}
		else if(tex[j] == 0)
			return tex_mgr.LoadFromFile(tex);
	}

	return true;
}

void CGUI::AddWindows(CGUIWindow *wins, int n)
{
	int i, j;

	if(win)
	{
		CGUIWindow **wt = new CGUIWindow*[num + n];
		for(i = 0; i < num; i++)
			wt[i] = win[i];
		delete [] win;
		for(j = 0; j < n; j++, i++)
			wt[i] = &wins[j];
		win = wt;
		num += n;
	}
	else
	{
		num = n;
		win = new CGUIWindow*[num];
		for(i = 0; i < num; i++)
			win[i] = &wins[i];
	}
}

void CGUI::ProcessInput(CControl *con, const CVector2d &mpos, int bbx, int bby)
{
	if(con->hit[KEY_MLEFT])
	{
		focus_window = NULL;
		for(int i = 0; i < num; i++)
		{
			if(win[i]->visible) // check for click
			{
				CVector2d min, max;
				min.x = int(win[i]->min.x * bbx);
				min.y = int(win[i]->min.y * bby);
				max.x = int(win[i]->max.x * bbx);
				max.y = int(win[i]->max.y * bby);

				if(mpos.x > min.x && mpos.y > min.y &&
					mpos.x < max.x && mpos.y < max.y)
				{
					focus_window = win[i];
					break;
				}
			}
		}
	}

	if(focus_window)
		focus_window->ProcessInput(con, mpos, bbx, bby);
}

void CGUI::Update()
{
	for(int i = 0; i < num; i++)
	{
		if(win[i]->visible)
			win[i]->Update();
	}
}

CGUIWindow *CGUI::GetWindowByID(int id)
{
	for(int i = 0; i < num; i++)
	{
		if(win[i]->id == id)
			return win[i];
	}

	return NULL;
}

void CGUI::Destruct()
{
	int i;

	if(win)
	{
		for(i = 0; i < num; i++)
			delete win[i];
		delete [] win;
		win = NULL;
	}

	num = 0;
}

void CGUI::Render(CRenderDevice *rd, CText *txt)
{
	for(int i = 0; i < num; i++)
	{
		if(win[i]->visible)
			RenderWindow(win[i], tex_mgr, rd, txt);
	}
}

void CGUI::RenderWindow(CGUIWindow *win, const CTextureManager &tex_mgr, CRenderDevice *rd, CText *txt)
{
	int i, j, num;
	CVector2d wmin, wmax, bmin, bmax;

	rd->SetVertexShader(rd->vs_tex0);

	// render window quad
	rd->SetTexture(0, tex_mgr.tex[win->tex]);
	rd->BeginPoly(RND_TEX0);
	rd->Texcoord(0, 0);
	rd->Vertex_perc(win->min.x, win->min.y);
	rd->Texcoord(1, 0);
	rd->Vertex_perc(win->max.x, win->min.y);
	rd->Texcoord(1, 1);
	rd->Vertex_perc(win->max.x, win->max.y);
	rd->Texcoord(0, 1);
	rd->Vertex_perc(win->min.x, win->max.y);
	rd->EndPoly();

	wmin.x = int(rd->bb_x * win->min.x);
	wmin.y = int(rd->bb_y * win->min.y);
	wmax.x = int(rd->bb_x * win->max.x);
	wmax.y = int(rd->bb_y * win->max.y);

	// render buttons
	CButton *b = win->GetButtons(&num);
	txt->EnableTextBlack();
	for(i = 0; i < num; i++)
	{
		bmin.x = int(((wmax.x - wmin.x) * b[i].min.x) + wmin.x);
		bmin.y = int(((wmax.y - wmin.y) * b[i].min.y) + wmin.y);
		bmax.x = int(((wmax.x - wmin.x) * b[i].max.x) + wmin.x);
		bmax.y = int(((wmax.y - wmin.y) * b[i].max.y) + wmin.y);

		rd->SetTexture(0, tex_mgr.tex[b[i].tex]);
		rd->BeginPoly(RND_TEX0);
		rd->Texcoord(0, 0);
		rd->Vertex(bmin.x, bmin.y);
		rd->Texcoord(1, 0);
		rd->Vertex(bmax.x, bmin.y);
		rd->Texcoord(1, 1);
		rd->Vertex(bmax.x, bmax.y);
		rd->Texcoord(0, 1);
		rd->Vertex(bmin.x, bmax.y);
		rd->EndPoly();

		rd->SetTexture(0, txt->texfont_black);
		txt->Print_rect(bmin.x, bmin.y, bmax.x, bmax.y, b[i].text);
	}

	// render editboxes
	CEditBox *e = win->GetEditBoxes(&num);
	for(i = 0; i < num; i++)
	{
		bmin.x = int(((wmax.x - wmin.x) * e[i].pos.x) + wmin.x);
		bmin.y = int(((wmax.y - wmin.y) * e[i].pos.y) + wmin.y);
		bmax.x = int(((wmax.x - wmin.x) * (e[i].pos.x + e[i].length)) + wmin.x);
		//bmax.y = int(((wmax.y - wmin.y) * (e[i].pos.y + e[i].height)) + wmin.y);
		bmax.y = int(bmin.y + (e[i].height * rd->bb_y));

		txt->DisableTextBlack();
		rd->SetTexture(0, NULL);
		rd->SetVertexShader(rd->vs_diffuse);

		rd->Diffuse_RGB(255, 255, win->cur_editbox == &e[i] ? 0 : 255);

		rd->BeginPoly(RND_DIFFUSE);
		rd->Vertex(bmin.x, bmin.y);
		rd->Vertex(bmax.x, bmin.y);
		rd->Vertex(bmax.x, bmax.y);
		rd->Vertex(bmin.x, bmax.y);
		rd->EndPoly();

		txt->EnableTextBlack();
		rd->SetVertexShader(rd->vs_tex0);
		rd->SetTexture(0, txt->texfont_black);

		j = int(e[i].length / e[i].char_width); // max chars
		if((int)strlen(e[i].text) < j)
		{
			txt->cs_x = (float(bmax.x - bmin.x) / j) / rd->bb_x;
			txt->cs_y = float(bmax.y - bmin.y) / rd->bb_y;
			txt->Print(bmin.x, bmin.y, e[i].text);
		}
		else
		{
			if(win->cur_editbox == &e[i]) // editbox is selected
			{
				char *c = &e[i].text[strlen(e[i].text) - j];
				txt->Print_rect(bmin.x, bmin.y, bmax.x, bmax.y, c);
			}
			else
			{
				char c = e[i].text[j];
				e[i].text[j] = 0;
				txt->cs_x = (float(bmax.x - bmin.x) / j) / rd->bb_x;
				txt->cs_y = float(bmax.y - bmin.y) / rd->bb_y;
				txt->Print(bmin.x, bmin.y, e[i].text);
				e[i].text[j] = c;
			}
		}
	}
	txt->DisableTextBlack();
}