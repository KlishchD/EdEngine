#include "RenderTask.h"

void RenderTask::Setup(Renderer* renderer)
{
	m_Renderer = renderer;
	m_Context = renderer->GetRenderContext();
}
