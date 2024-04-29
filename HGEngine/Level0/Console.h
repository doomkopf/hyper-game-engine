#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "Texture.h"
#include "../../stuff/Stack.h"
#include "Control.h"
#include "PObject.h"
#include "Renderable.h"

#define MAX_CONSTRING 64

enum constate_t
{
	CS_UP = 0,
	CS_DOWN = 1,
	CS_MOVUP = 2,
	CS_MOVDOWN = 3
};

struct constring_t
{
	char str[MAX_CONSTRING];
	unsigned char r, g, b;
};

class CConsole : public CPObject, IRenderableText
{
	friend class CRender;
public:
	CConsole();
	~CConsole();
	bool Init(CRenderDevice *);
	void DropDown();
	void DropUp();
	void DropToggle();
	void AddInput(const CControl &);
	bool HasCommand();
	void Print(const char *, unsigned char r = 255, unsigned char g = 255, unsigned char b = 255);
	void Update();
	virtual void Render(CRenderDevice *, CText *);

	bool active;

	constring_t input;
	int cur;

protected:
	constate_t state;
	float dropstep, pos, perc;
	bool has_cmd;

	CStack<constring_t> text;

	CTexture *tex;
};

#endif