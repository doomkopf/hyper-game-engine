#ifndef RENDERABLE_H_
#define RENDERABLE_H_

#include "RenderDevice.h"
#include "Text.h"

class IRenderable
{
public:
	virtual void Render(CRenderDevice *) = 0;
};

class IRenderableText
{
public:
	virtual void Render(CRenderDevice *, CText *) = 0;
};

class IRenderable2D3D
{
public:
	virtual void Render2D(CRenderDevice *) = 0;
	virtual void Render3D(CRenderDevice *) = 0;
};

#endif