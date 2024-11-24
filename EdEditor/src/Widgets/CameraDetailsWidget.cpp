#include "CameraDetailsWidget.h"
#include "Editor.h"
#include "Core/Scene.h"
#include "Core/Engine.h"
#include "Core/Rendering/Renderer.h"
#include "Core/Rendering/Passes/ResolutionPass.h"
#include "Core/Rendering/Passes/Bloom/BloomMultiPass.h"
#include "Core/Rendering/Passes/FXAAPass.h"
#include "Core/Rendering/Passes/TAAPass.h"
#include "Core/Rendering/Passes/Lighting/SpotLight/SpotLightMultiPass.h"
#include "Core/Rendering/Passes/Lighting/SpotLight/SpotLightShadingPass.h"
#include "Core/Rendering/Passes/SSAO/SSAOMultiPass.h"
#include "Core/Rendering/Passes/SSAO/SSAOPass.h"
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

    Camera& camera = m_Engine->GetLoadedScene()->GetPlayerActor()->GetCameraComponent()->GetCamera();
	{
		if (ImGui::Begin("Camera"))
		{
			if (ImGui::BeginTable("Transform", 2, ImGuiTableFlags_SizingStretchProp))
			{
				glm::vec3 translation = camera.GetPosition();
				glm::vec3 rotation = camera.GetRotation();

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
		}

        ImGui::End();
	}

	{
		if (ImGui::Begin("Rendering"))
		{

			static RenderTarget targets[] = { RenderTarget::GAlbedo, RenderTarget::GPosition, RenderTarget::GNormal, RenderTarget::GRougnessMetalicEmission, RenderTarget::GVelocity, RenderTarget::GDepth, RenderTarget::SSAO, RenderTarget::Diffuse, RenderTarget::Specular, RenderTarget::Light, RenderTarget::Bloom, RenderTarget::AAOutput, RenderTarget::Resolution };

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

			std::shared_ptr<RenderGraph> graph = m_Renderer->GetGraph();

			if (bool enabled = m_Renderer->IsBloomEnabled(); ImGui::Checkbox("Bloom Active", &enabled))
			{
				m_Renderer->SetBloomEnabled(enabled);
			}

			if (m_Renderer->IsBloomEnabled())
			{
				std::shared_ptr<ResolutionPass> resoultion = graph->GetPass<ResolutionPass>();
				if (float strength = resoultion->GetBloomStrength(); ImGui::SliderFloat("Bloom strength", &strength, 0.0f, 1.0f))
				{
					resoultion->SetBloomStrength(strength);
				}

				std::shared_ptr<BloomMultiPass> bloom = graph->GetPass<BloomMultiPass>();
				if (float strength = bloom->GetBloomMixStrength(); ImGui::SliderFloat("Bloom mix strength", &strength, 0.0f, 1.0f))
				{
					bloom->SetBloomMixStrength(strength);
				}

				if (int32_t count = bloom->GetBloomDownscaleCount(); ImGui::SliderInt("Downscale count", &count, 1, 8))
				{
					bloom->SetBloomDownscaleCount(count);
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
				std::shared_ptr<FXAAPass> fxaa = graph->GetPass<FXAAPass>();
				if (float threshold = fxaa->GetContrastThreshold(); ImGui::SliderFloat("Contrast threshold", &threshold, 0.01f, 0.1f))
				{
					fxaa->SetContrastThreshold(threshold);
				}

				if (float threshold = fxaa->GetRelativeThreshold(); ImGui::SliderFloat("Relative threshold", &threshold, 0.01f, 0.4f))
				{
					fxaa->SetRelativeThreshold(threshold);
				}

				if (float scale = fxaa->GetSubpixelBlending(); ImGui::SliderFloat("Subpixel blending", &scale, 0.0f, 1.0f))
				{
					fxaa->SetSubpixelBlending(scale);
				}
			}
			else if (m_Renderer->GetAAMethod() == AAMethod::TAA)
			{
				std::shared_ptr<TAAPass> taa = graph->GetPass<TAAPass>();
				if (float gamma = taa->GetGamma(); ImGui::SliderFloat("TAA Gamma", &gamma, 0.5f, 10.0f))
				{
					taa->SetGamma(gamma);
				}
			}

			if (bool enabled = m_Renderer->IsSSAOEnabled(); ImGui::Checkbox("SSAO enabled", &enabled))
			{
				m_Renderer->SetSSAOEnabled(enabled);
			}

			if (m_Renderer->IsSSAOEnabled())
			{
				std::shared_ptr<SSAOBasePass> ssao = m_Renderer->GetGraph()->GetPass<SSAOMultiPass>()->GetPass<SSAOBasePass>();

				if (int32_t samples = ssao->GetSamplesCount(); ImGui::SliderInt("SSAO samples count", &samples, 1, 32))
				{
					ssao->SetSamplesCount(samples);
				}

				if (int32_t size = ssao->GetNosiseSize(); ImGui::SliderInt("SSAO noise size", &size, 10, 64))
				{
					ssao->SetNoiseSize(size);
				}

				if (float radius = ssao->GetRadius(); ImGui::SliderFloat("SSAO radius", &radius, 0.5f, 10.0f))
				{
					ssao->SetRadius(radius);
				}

				if (float bias = ssao->GetBias(); ImGui::SliderFloat("SSAO bias", &bias, 0.001f, 1.0f))
				{
					ssao->SetBias(bias);
				}
			}

			if (std::shared_ptr<SpotLightMultiPass> multiPass = graph->GetPass<SpotLightMultiPass>())
			{
				std::shared_ptr<SpotLightShadingPass> shading = multiPass->GetPass<SpotLightShadingPass>();

				if (int32_t count = shading->GetShadowSamplesBlocksCount(); ImGui::SliderInt("Spot light samples blocks count", &count, 1, 10))
				{
					shading->SetShadowSamplesBlockCount(count);
				}

				if (int32_t size = shading->GetShadowSamplesBlockSize(); ImGui::SliderInt("Spot light samples block size", &size, 1, 32))
				{
					shading->SetShadowSamplesBlockSize(size);
				}
			}

			std::shared_ptr<ResolutionPass> resoultion = graph->GetPass<ResolutionPass>();
			if (float gamma = resoultion->GetGamma(); ImGui::SliderFloat("Gamma", &gamma, 0.1f, 10.0f))
			{
				resoultion->SetGamma(gamma);
			}
		}

		ImGui::End();
	}
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
    case RenderTarget::GDepth:                   return "GDepth";
    case RenderTarget::SSAO:                     return "SSAO";
    case RenderTarget::Diffuse:                  return "Diffuse";
    case RenderTarget::Specular:                 return "Specular";
    case RenderTarget::Light:                    return "Light";
    case RenderTarget::Bloom:                    return "Bloom";
    case RenderTarget::AAOutput:                 return "AAOutput";
    case RenderTarget::Resolution:               return "Resolution";
    default:
        ED_ASSERT(0, "Unsupported render target")
    }
}
