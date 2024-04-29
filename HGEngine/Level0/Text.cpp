#include "Text.h"
#include <stdio.h>

char CText::tb[256];

CText::CText()
{
	rd = NULL;
	texfont_black = NULL;
	texfont_white = NULL;
}

CText::~CText()
{
	if(texfont_white)
	{
		delete texfont_white;
		texfont_white = NULL;
	}

	if(texfont_black)
	{
		delete texfont_black;
		texfont_black = NULL;
	}
}

bool CText::Init(CRenderDevice *r)
{
	rd = r;

	texfont_white = rd->CreateTextureFromFile("../textures/font.bmp");
	if(!texfont_white)
		return false;

	texfont_black = rd->CreateTextureFromFile("../textures/font2.bmp");
	if(!texfont_black)
		return false;

	return true;
}

void CText::Printp_rect(float min_x, float min_y, float max_x, float max_y, const char *text, ...)
{
	va_list list;
	va_start(list, text);
	vsprintf(tb, text, list);
	va_end(list);

	// finding the longest line and line count
	int mc = 1, lc = 1;
	for(int i = 0, c = 0; ; i++)
	{
		if(text[i] == '\n')
		{
			if(c > mc)
				mc = c;
			c = 0;
			lc++;
		}
		else if(!text[i])
		{
			mc = c;
			break;
		}
		else
			c++;
	}
	//

	cs_x = (max_x - min_x) / mc;
	cs_y = (max_y - min_y) / lc;

	Printp(min_x, min_y, tb);
}

void CText::Print_rect(int min_x, int min_y, int max_x, int max_y, const char *text, ...)
{
	va_list list;
	va_start(list, text);
	vsprintf(tb, text, list);
	va_end(list);

	// finding the longest line and line count
	int mc = 1, lc = 1;
	for(int i = 0, c = 0; ; i++)
	{
		if(text[i] == '\n')
		{
			if(c > mc)
				mc = c;
			c = 0;
			lc++;
		}
		else if(!text[i])
		{
			mc = c;
			break;
		}
		else
			c++;
	}
	//

	cs_x = (float(max_x - min_x) / mc) / rd->bb_x;
	cs_y = (float(max_y - min_y) / lc) / rd->bb_y;

	Print(min_x, min_y, tb);
}

void CText::Printp(float x, float yp, const char *text, ...)
{
	if(!text)
		return;

	va_list list;
	float right, down, y = yp;

	va_start(list, text);
	vsprintf(tb, text, list);
	va_end(list);

	for(int i = 0, j = 0; tb[i] != 0; i++, j++)
	{
		if(tb[i] == '\n')
		{
			y += cs_y;
			j = -1;
			continue;
		}

		right = float((tb[i] - 32) % 16);
		down = float((tb[i] - 32) / 16);

		rd->BeginPoly(RND_TEX0 | RND_DIFFUSE);

		rd->Texcoord(0.0625f * (right + 1), 0.0625f * down);
		rd->Vertex_perc(x + (cs_x * (j + 1)), y);

		rd->Texcoord(0.0625f * (right + 1), 0.0625f * (down + 1));
		rd->Vertex_perc(x + (cs_x * (j + 1)), y + cs_y);

		rd->Texcoord(0.0625f * right, 0.0625f * (down + 1));
		rd->Vertex_perc(x + (cs_x * j), y + cs_y);

		rd->Texcoord(0.0625f * right, 0.0625f * down);
		rd->Vertex_perc(x + (cs_x * j), y);

		rd->EndPoly();
	}
}

void CText::Print(int x, int yp, const char *text, ...)
{
	if(!text)
		return;

	va_list list;
	float right, down;
	int lcs_x, lcs_y, y = yp;

	va_start(list, text);
	vsprintf(tb, text, list);
	va_end(list);

	lcs_x = int(cs_x * rd->bb_x);
	lcs_y = int(cs_y * rd->bb_y);

	for(int i = 0, j = 0; tb[i] != 0; i++, j++)
	{
		if(tb[i] == '\n')
		{
			y += lcs_y;
			j = -1;
			continue;
		}

		right = float((tb[i] - 32) % 16);
		down = float((tb[i] - 32) / 16);

		rd->BeginPoly(RND_TEX0 | RND_DIFFUSE);

		rd->Texcoord(0.0625f * (right + 1), 0.0625f * down);
		rd->Vertex(float(x + (lcs_x * (j + 1))), (float)y);

		rd->Texcoord(0.0625f * (right + 1), 0.0625f * (down + 1));
		rd->Vertex(x + (lcs_x * (j + 1)), y + lcs_y);

		rd->Texcoord(0.0625f * right, 0.0625f * (down + 1));
		rd->Vertex(x + (lcs_x * j), y + lcs_y);

		rd->Texcoord(0.0625f * right, 0.0625f * down);
		rd->Vertex(float(x + (lcs_x * j)), (float)y);

		rd->EndPoly();
	}
}

void CText::EnableTextBlack()
{
	rd->ALBL_Enable();
	rd->dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_MIN);
}

void CText::DisableTextBlack()
{
	rd->dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	rd->ALBL_Disable();
}

void CText::EnableText()
{
	rd->ALBL_Enable();
	rd->ALBL_SetMode(ALBL_TRANSPARENCE);
}

void CText::DisableText()
{
	rd->ALBL_Disable();
}