#include "RenderTask.h"

void RenderTask::Setup(Renderer* renderer)
{
	m_Renderer = renderer;
	m_Context = renderer->GetRenderContext();
}

void RenderTask::SetEnabled(bool enabled)
{
	m_bIsEnabled = enabled;
}

bool RenderTask::IsEnabled() const
{
	return m_bIsEnabled;
}
