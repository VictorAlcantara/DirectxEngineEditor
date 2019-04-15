#include "DebugRender.h"

uint DebugRender::s_idControl = 1;

DebugRender::DebugRender() : color4(1.0f)
{
	id = s_idControl++;
	visible = true;
}
