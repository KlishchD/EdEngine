#include "RenderingContex.h"
#include "Types.h"
#include "Core/Macros.h"

RenderingContext* Context = nullptr;

void RenderingContext::SetContext(RenderingContext* context)
{
	Context = context;
}

RenderingContext& RenderingContext::Get()
{
	return *Context;
}
