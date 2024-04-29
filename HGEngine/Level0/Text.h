#ifndef TEXT_H_
#define TEXT_H_

#include "RenderDevice.h"

class CText
{
public:
	CText();
	~CText();
	bool Init(CRenderDevice *);
	void EnableTextBlack();
	void DisableTextBlack();
	void EnableText();
	void DisableText();
	void Printp(float, float, const char *, ...);
	void Printp_rect(float, float, float, float, const char *, ...);
	void Print(int, int, const char *, ...);
	void Print_rect(int, int, int, int, const char *, ...);

	CTexture *texfont_black, *texfont_white;
	float cs_x, cs_y;

protected:
	CRenderDevice *rd;
	static char tb[256];
};

#endif