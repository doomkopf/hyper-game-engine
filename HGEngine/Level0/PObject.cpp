#include "PObject.h"
#include "Engine.h"

float &GetSpeedFactor();

CPObject::CPObject() : sf(GetSpeedFactor())
{
}