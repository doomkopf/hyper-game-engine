#include "GUIWindow.h"
#include "../../../stuff/scan2ascii.h"

CGUIWindow::CGUIWindow()
{
	id = -1;
	tex = -1;
	visible = false;
	cur_editbox = NULL;

	min.x = 0.25f;
	min.y = 0.25f;
	max.x = 0.75f;
	max.y = 0.75f;
}

CGUIWindow::~CGUIWindow()
{
	Destruct();
}

void CGUIWindow::ProcessInput(CControl *con, const CVector2d &mpos, int bbx, int bby)
{
	int i, num;
	CVector2d wmin, wmax;

	wmin.x = int(bbx * min.x);
	wmin.y = int(bby * min.y);
	wmax.x = int(bbx * max.x);
	wmax.y = int(bby * max.y);

	CButton *btn = GetButtons(&num);
	for(i = 0; i < num; i++)
	{
		if(mpos.x > ((wmax.x - wmin.x) * btn[i].min.x) + wmin.x &&
			mpos.y > ((wmax.y - wmin.y) * btn[i].min.y) + wmin.y &&
			mpos.x < ((wmax.x - wmin.x) * btn[i].max.x) + wmin.x &&
			mpos.y < ((wmax.y - wmin.y) * btn[i].max.y) + wmin.y)
		{
			btn[i].flags |= GUI_MOUSEOVER;
			if(con->hit[KEY_MLEFT])
				btn[i].flags |= GUI_CLICK;
		}
		else
		{
			btn[i].flags &= ~GUI_MOUSEOVER;
		}
	}

	if(con->hit[KEY_MLEFT])
		cur_editbox = NULL;
	CEditBox *eb = GetEditBoxes(&num);
	for(i = 0; i < num; i++)
	{
		if(mpos.x > ((wmax.x - wmin.x) * eb[i].pos.x) + wmin.x &&
			mpos.y > ((wmax.y - wmin.y) * eb[i].pos.y) + wmin.y &&
			mpos.x < ((wmax.x - wmin.x) * (eb[i].pos.x + eb[i].length)) + wmin.x &&
			//mpos.y < ((wmax.y - wmin.y) * (eb[i].pos.y + eb[i].height)) + wmin.y)
			mpos.y < (((wmax.y - wmin.y) * eb[i].pos.y) + wmin.y) + (eb[i].height * bby))
		{
			eb[i].flags |= GUI_MOUSEOVER;
			if(con->hit[KEY_MLEFT])
			{
				eb[i].flags |= GUI_CLICK;
				cur_editbox = &eb[i];
			}
		}
		else
		{
			eb[i].flags &= ~GUI_MOUSEOVER;
		}
	}

	// processing input for currently selected editbox
	if(cur_editbox)
	{
		bool shift = (con->key[KEY_RSHIFT] || con->key[KEY_LSHIFT]) ? true : false;
		char *c = &cur_editbox->text[strlen(cur_editbox->text)];

		for(int i = 0; i < NUM_KEYS; i++)
		{
			if(con->hit[i])
			{
				switch(i)
				{
				case KEY_RETURN:
					cur_editbox = NULL;
					return;

				case KEY_BACKSPACE:
					if(c > cur_editbox->text)
						*(--c) = 0;
					break;

				case KEY_DELETE:
					cur_editbox->text[0] = 0;
					c = cur_editbox->text;
					break;

				default:
					if(strlen(cur_editbox->text) < sizeof(cur_editbox->text) - 1)
					{
						*c = shift ? scan2ascii_shift(con->converts[i]) : scan2ascii(con->converts[i]);
						*(++c) = 0;
					}
					break;
				}
			}
		}
	}
}

void CGUIWindow::Update()
{
}

void CGUIWindow::Destruct()
{
}