#include "Console.h"
#include "../../stuff/scan2ascii.h"

CConsole::CConsole() : text(32)
{
	tex = new CTexture;

	state = CS_UP;
	active = false;
	has_cmd = false;
	dropstep = 1.0f;
	pos = 0;
	perc = 0.75f;
	cur = 0;
	input.str[cur] = 0;
}

CConsole::~CConsole()
{
	delete tex;
}

bool CConsole::Init(CRenderDevice *rd)
{
	tex = rd->CreateTextureFromFile("../textures/conbg.jpg");
	return tex ? true : false;
}

void CConsole::DropDown()
{
	state = CS_MOVDOWN;
	active = false;
}

void CConsole::DropUp()
{
	state = CS_MOVUP;
	active = false;
}

void CConsole::DropToggle()
{
	if(state == CS_UP || state == CS_MOVUP)
		state = CS_MOVDOWN;
	else
		state = CS_MOVUP;

	active = false;
}

void CConsole::Print(const char *str, unsigned char r, unsigned char g, unsigned char b)
{
	if(strlen(str) >= MAX_CONSTRING)
		return;

	constring_t cs;

	strcpy(cs.str, str);
	cs.r = r;
	cs.g = g;
	cs.b = b;

	text.Push(cs);
}

void CConsole::AddInput(const CControl &control)
{
	if(has_cmd)
		return;

	bool shift = (control.key[KEY_RSHIFT] || control.key[KEY_LSHIFT]) ? true : false;

	for(int i = 0; i < NUM_KEYS; i++)
	{
		if(control.hit[i])
		{
			switch(i)
			{
			case KEY_RETURN:
				if(cur > 0)
					has_cmd = true;
				break;

			case KEY_BACKSPACE:
				cur--;
				if(cur < 0)
					cur = 0;
				input.str[cur] = 0;
				break;

			case KEY_DELETE:
				cur = 0;
				input.str[cur] = 0;
				break;

			default:
				if(shift)
					input.str[cur] = scan2ascii_shift(control.converts[i]);
				else
					input.str[cur] = scan2ascii(control.converts[i]);

				if((cur < MAX_CONSTRING - 1) && input.str[cur])
					cur++;

				input.str[cur] = 0;
				break;
			}
		}
	}
}

bool CConsole::HasCommand()
{
	if(has_cmd)
	{
		has_cmd = false;
		return true;
	}

	return has_cmd;
}

void CConsole::Update()
{
	if(state == CS_MOVDOWN)
	{
		pos += dropstep * sf;
		if(pos >= perc)
		{
			pos = perc;
			state = CS_DOWN;
			active = true;
		}
	}
	else if(state == CS_MOVUP)
	{
		pos -= dropstep * sf;
		if(pos <= 0)
		{
			pos = 0;
			state = CS_UP;
		}
	}
}

void CConsole::Render(CRenderDevice *rd, CText *txt)
{
	if(state == CS_UP)
		return;

	static CMatrix4x4 m4x4;
	constring_t **buf;
	int p;
	float pos;

	m4x4.View2D(rd->bb_x, rd->bb_y);
	m4x4.Translate(0, -rd->bb_y * (1 - this->pos), 0);
	rd->cam.mt_view2d = m4x4;
	rd->SetTransform2D();

	// background

	rd->ALBL_Enable();
	rd->ALBL_SetMode(ALBL_TRANSPARENCE);

	rd->SetTexture(0, tex);
	rd->SetVertexShader(rd->vs_diffuse_tex0);

	rd->Diffuse_RGBA(255, 255, 255, 170);

	rd->BeginPoly(RND_TEX0 | RND_DIFFUSE);

	rd->Texcoord(0, 0);
	rd->Vertex(0, 0);
	rd->Texcoord(1, 0);
	rd->Vertex(rd->bb_x, 0);
	rd->Texcoord(1, 1);
	rd->Vertex(rd->bb_x, rd->bb_y);
	rd->Texcoord(0, 1);
	rd->Vertex(0, rd->bb_y);

	rd->EndPoly();

	// text

	txt->cs_x = 0.016125f;
	txt->cs_y = 0.03125f;

	rd->dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	rd->SetTexture(0, txt->texfont_white);

	pos = 1.0f - (txt->cs_y * 4);
	buf = text.GetBuffer();
	p = text.GFDI();

	while(p > -1)
	{
		rd->Diffuse_RGB(buf[p]->r, buf[p]->g, buf[p]->b);
		txt->Printp(0.01625f, pos, buf[p]->str);
		pos -= txt->cs_y * 2;
		p = text.GNDI(p);
	}

	// input line

	rd->Diffuse_RGB(255, 255, 255);
	txt->Printp(0.01625f, 1.0f - txt->cs_y, ":>%s_", input.str);


	rd->cam.mt_view2d.View2D(rd->bb_x, rd->bb_y);
	rd->SetTransform2D();

	rd->ALBL_Disable();

	rd->SetTexture(0, NULL);
}