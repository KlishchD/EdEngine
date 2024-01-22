#include "CameraDetailsWidget.h"
#include "Editor.h"
#include "Core/Engine.h"
#include "Core/Rendering/Renderer.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

CameraDetailsWidget::CameraDetailsWidget()
{
	InstancesCount++;
}

void CameraDetailsWidget::Initialize()
{
	Widget::Initialize();

	m_Engine = &Engine::Get();
	m_Renderer = m_Engine->GetRenderer();
	m_Editor = m_Engine->GetManager<Editor>();
}

void CameraDetailsWidget::Tick(float DeltaTime)
{
	Widget::Tick(DeltaTime);

	if (Camera* camera = m_Engine->GetCamera())
	{
		ImGui::Begin("Camera");

		if (ImGui::BeginTable("Transform", 2, ImGuiTableFlags_SizingStretchProp))
		{
			glm::vec3 translation = camera->GetPosition();
			glm::vec3 rotation = camera->GetRotation();

			PositionSliders(translation, { -10000.0f, 10000.0f });
			RotationSliders(rotation);

			ImGui::EndTable();
		}

		float mSpeed = m_Editor->GetCameraSpeed();
		ImGui::SliderFloat("Camera speed", &mSpeed, 1, 10000);
		m_Editor->SetCameraSpeed(mSpeed);

		glm::vec2 rSpeed = m_Editor->GetCameraRotationSpeed();
		ImGui::SliderFloat2("Camera rotation speed", (float*)&rSpeed, 1, 10000);
		m_Editor->SetCameraRotationSpeed(rSpeed);

		ImGui::End();
	}

	ImGui::Begin("Rendering");

	static RenderTarget targets[] = { RenderTarget::GAlbedo, RenderTarget::GPosition, RenderTarget::GNormal, RenderTarget::GRougnessMetalicEmission, RenderTarget::GVelocity, RenderTarget::LightPass, RenderTarget::SSAO, RenderTarget::SSDO, RenderTarget::CombinationPass, RenderTarget::AAOutput, RenderTarget::PostProcessing };

	if (RenderTarget activeTarget = m_Renderer->GetActiveRenderTarget(); ImGui::BeginCombo("Render Target", GetRenderTargetName(activeTarget).c_str()))
	{
		for (RenderTarget target : targets)
		{
			if (ImGui::Selectable(GetRenderTargetName(target).c_str(), activeTarget == target))
			{
				m_Renderer->SetActiveRenderTarget(target);
			}
		}

		ImGui::EndCombo();
	}

	if (bool enabled = m_Renderer->IsBloomEnabled(); ImGui::Checkbox("Bloom Active", &enabled))
	{
		m_Renderer->SetBloomEnabled(enabled);
	}

	if (m_Renderer->IsBloomEnabled())
	{
		if (bool use = m_Renderer->IsUsingNewBloom(); ImGui::Checkbox("Use new bloom", &use))
		{
			m_Renderer->SetUseNewBloom(use);
		}

		if (m_Renderer->IsUsingNewBloom())
		{
			if (float strength = m_Renderer->GetBloomStrength(); ImGui::SliderFloat("Bloom strength", &strength, 0.0f, 1.0f))
			{
				m_Renderer->SetBloomStrength(strength);
			}

			if (float strength = m_Renderer->GetBloomMixStrength(); ImGui::SliderFloat("Bloom mix strength", &strength, 0.0f, 1.0f))
			{
				m_Renderer->SetBloomMixStrength(strength);
			}

			if (int32_t count = m_Renderer->GetBloomDownscaleTextureCount(); ImGui::SliderInt("Downscale count", &count, 1, 8))
			{
				m_Renderer->SetBloomDownscaleTexturesCount(count);
			}
		}
		else
		{
			if (float intensity = m_Renderer->GetBloomIntensity(); ImGui::SliderFloat("Bloom intensity", &intensity, 0.0f, 1.0f))
			{
				m_Renderer->SetBloomIntensity(intensity);
			}
		}
	}

	if (float scale = m_Renderer->GetUpsampleScale(); ImGui::SliderFloat("Upsample scale", &scale, 0.25f, 4.0f))
	{
		m_Renderer->SetUpsampleScale(scale);
	}

	if (AAMethod current = m_Renderer->GetAAMethod(); ImGui::BeginCombo("AA Method", ConvertAAMethodToString(current).c_str()))
	{
		static AAMethod methods[] = { AAMethod::None, AAMethod::TAA, AAMethod::FXAA };
		for (AAMethod method : methods)
		{
			if (ImGui::Selectable(ConvertAAMethodToString(method).c_str(), current == method))
			{
				m_Renderer->SetAAMethod(method);
			}
		}
		ImGui::EndCombo();
	}

	if (m_Renderer->GetAAMethod() == AAMethod::FXAA)
	{
		if (float threshold = m_Renderer->GetContrastThreshold(); ImGui::SliderFloat("Contrast threshold", &threshold, 0.01f, 0.1f))
		{
			m_Renderer->SetContrastThreshold(threshold);
		}

		if (float threshold = m_Renderer->GetRelativeThreshold(); ImGui::SliderFloat("Relative threshold", &threshold, 0.01f, 0.4f))
		{
			m_Renderer->SetRelativeThreshold(threshold);
		}

		if (float scale = m_Renderer->GetSubpixelBlending(); ImGui::SliderFloat("Subpixel blending", &scale, 0.0f, 1.0f))
		{
			m_Renderer->SetSubpixelBlending(scale);
		}
	}
	else if (m_Renderer->GetAAMethod() == AAMethod::TAA)
	{
		if (float gamma = m_Renderer->GetTAAGamma(); ImGui::SliderFloat("TAA Gamma", &gamma, 0.5f, 10.0f))
		{
			m_Renderer->SetTAAGamma(gamma);
		}
	}

	if (bool active = m_Renderer->IsUsingComputeShadersForPostProcessing(); ImGui::Checkbox("Use compute shaders for post processing", &active))
	{
		m_Renderer->SetIsUsingComputeShadersForPostProcessing(active);
	}

	if (float gamma = m_Renderer->GetGamma(); ImGui::SliderFloat("Gamma", &gamma, 0.1f, 10.0f))
	{
		m_Renderer->SetGamma(gamma);
	}

	if (bool enabled = m_Renderer->IsSSDOEnabled(); ImGui::Checkbox("SSDO enabled", &enabled))
	{
		m_Renderer->SetSSDOEnabled(enabled);
	}

	if (m_Renderer->IsSSDOEnabled())
	{
		if (float radius = m_Renderer->GetSSDORadius(); ImGui::SliderFloat("SSDO radius", &radius, 0.01f, 2.0f))
		{
			m_Renderer->SetSSDORadius(radius);
		}

		if (int32_t count = m_Renderer->GetSSDOSampleCount(); ImGui::SliderInt("SSDO samples", &count, 0, 100))
		{
			m_Renderer->SetSSDOSampleCount(count);
		}

		if (float strength = m_Renderer->GetSSDOBounceStrength(); ImGui::SliderFloat("SSDO bounce strength", &strength, 0.0f, 10.0f))
		{
			m_Renderer->SetSSDOBounceStrength(strength);
		}
	}

	ImGui::End();
}

bool CameraDetailsWidget::PositionSliders(glm::vec3& position, glm::vec2 range)
{
	ImGui::TableNextColumn(); ImGui::Text("Position");
	ImGui::TableNextColumn();
	return ImGui::SliderFloat3(("##CameraDetailsWidgetPosition" + std::to_string(InstancesCount)).data(), glm::value_ptr(position), range.x, range.y, "%.3f", ImGuiSliderFlags_Logarithmic);
}

bool CameraDetailsWidget::RotationSliders(glm::vec3& angles)
{
	ImGui::TableNextColumn(); ImGui::Text("Rotation");
	ImGui::TableNextColumn();
	return ImGui::SliderFloat3(("##CameraDetailsWidgetRotation" + std::to_string(InstancesCount)).data(), glm::value_ptr(angles), -180, 180);
}

std::string CameraDetailsWidget::ConvertAAMethodToString(AAMethod method)
{
	switch (method)
	{
	case AAMethod::None: return "None";
	case AAMethod::FXAA: return "FXAA";
	case AAMethod::TAA:  return "TAA";
	default:
		ED_ASSERT(0, "Unsupported AA method")
	}
}

std::string CameraDetailsWidget::GetRenderTargetName(RenderTarget target)
{
	switch (target)
	{
	case RenderTarget::GAlbedo:                  return "GAlbedo";
	case RenderTarget::GPosition:                return "GPosition";
	case RenderTarget::GNormal:                  return "GNormal";
	case RenderTarget::GRougnessMetalicEmission: return "GRougnessMetalicEmission";
	case RenderTarget::GVelocity:                return "GVelocity";
	case RenderTarget::LightPass:                return "LightPass";
	case RenderTarget::SSAO:                     return "SSAO";
	case RenderTarget::SSDO:                     return "SSDO";
	case RenderTarget::CombinationPass:          return "CombinationPass";
	case RenderTarget::AAOutput:                 return "AAOutput";
	case RenderTarget::PostProcessing:           return "PostProvessing";
	default:
		ED_ASSERT(0, "Unsupported render target")
	}
}
