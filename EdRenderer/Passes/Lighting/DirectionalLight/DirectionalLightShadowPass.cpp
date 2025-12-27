#include "EdRendererPrivate.h"

// #include "DirectionalLightShadowPass.h"
// #include "Helpers/MathHelper.h"
// 
// void DirectionalLightShadowPass::Initialize(std::shared_ptr<RenderGraph> graph)
// {
// 	RenderPass<DirectionalLightShadowPassParameters, DirectionalLightShadowPassShaderParameters>::Initialize(graph);
// 
// 	m_Parameters.Name = "Directional light shadow pass";
// 	m_Parameters.Shader = RenderingHelper::CreateShader("shaders\\light\\directional-light-shadow-pass.glsl");
// 
// 	m_Parameters.bUseBlending = false;
// 	m_Parameters.bClearDepth = true;
// }
// 
// void DirectionalLightShadowPass::Execute()
// {
// 	RenderPass<DirectionalLightShadowPassParameters, DirectionalLightShadowPassShaderParameters>::Execute();
// 
// 	glm::u32vec2 size = m_Renderer->GetViewportSize();
// 	u32 sideSize = glm::max((u32)size.x, ShadowCascadeMinSize);
// 	m_Parameters.DrawFramebuffer->Resize(sideSize, sideSize, MaxShadowCascadesCount);
// 
// 	if (m_Parameters.Light->IsShadowCasting())
// 	{
// 		m_Parameters.ShadowViewProjectionMatrices = CalculateShadowViewProjectionMatrices(m_Parameters.Light);
// 
// 		m_ShaderParameters.Cascades = m_Parameters.ShadowViewProjectionMatrices.size();
// 		for (u32 i = 0; i < m_ShaderParameters.Cascades; ++i)
// 		{
// 			m_ShaderParameters.ProjectionViewMatries[i] = m_Parameters.ShadowViewProjectionMatrices[i];
// 		}
// 
// 		for (const std::shared_ptr<StaticMeshComponent>& component : m_Parameters.Meshes.Get())
// 		{
// 			if (std::shared_ptr<StaticMesh> mesh = component->GetStaticMesh())
// 			{
// 				Transform worldTransform = component->GetWorldTransform();
// 				Transform previousWorldTransform = component->GetPreviousWorldTransform();
// 
// 				for (const std::shared_ptr<StaticSubmesh>& submesh : mesh->GetSubmeshes())
// 				{
// 					if (std::shared_ptr<Material> material = submesh->GetMaterial())
// 					{
// 						m_ShaderParameters.ModelMatrix = worldTransform.GetMatrix();
// 
// 						SubmitShaderParameters();
// 
// 						m_Context->SetVertexBuffer(submesh->GetVertexBuffer());
// 						m_Context->SetIndexBuffer(submesh->GetIndexBuffer());
// 						m_Context->Draw();
// 					}
// 				}
// 			}
// 		}
// 	}
// 
//     m_Renderer->DirectionalLightsShadowsDrawn++;
// }
// 
// std::vector<glm::mat4> DirectionalLightShadowPass::CalculateShadowViewProjectionMatrices(std::shared_ptr<DirectionalLightComponent> light) const
// {
// 	std::vector<glm::mat4> matrices;
// 
// 	if (light->IsShadowCasting())
// 	{
// 		static std::vector<glm::vec4> cornersNDC = {
// 			{ -1.0f, -1.0f, -1.0f, 1.0f },
// 			{ -1.0f,  1.0f, -1.0f, 1.0f },
// 			{  1.0f,  1.0f, -1.0f, 1.0f },
// 			{  1.0f, -1.0f, -1.0f, 1.0f },
// 			{ -1.0f, -1.0f,  1.0f, 1.0f },
// 			{ -1.0f,  1.0f,  1.0f, 1.0f },
// 			{  1.0f,  1.0f,  1.0f, 1.0f },
// 			{  1.0f, -1.0f,  1.0f, 1.0f }
// 		};
// 
// 		i32 cascades = glm::clamp<i32>(light->GetShadowCascadesCount(), MinShadowCascadesCount, MaxShadowCascadesCount);
// 		f32 farPlane = m_Renderer->GetFarPlane();
// 
// 		Camera& camera = m_Parameters.Camera->GetCamera();
// 
// 		for (i32 i = 0; i < cascades; ++i)
// 		{
// 			f32 currentNearPlane = glm::max(1.0f, (1.0f * i / cascades) * farPlane);
// 			f32 currentFarPlane = ((i + 1.0f) / cascades) * farPlane;
// 			glm::mat4 inverseProjectionView = glm::inverse(camera.GetProjectionView(currentNearPlane, currentFarPlane));
// 
// 			glm::vec3 centerWorld(0.0f);
// 
// 			std::vector<glm::vec3> cornersWorld;
// 
// 			for (const glm::vec4& cornerNDC : cornersNDC)
// 			{
// 				glm::vec4 cornerWorld = inverseProjectionView * cornerNDC;
// 				cornerWorld /= cornerWorld.w;
// 				cornersWorld.emplace_back(cornerWorld);
// 
// 				centerWorld += glm::vec3(cornerWorld);
// 			}
// 
// 			centerWorld /= cornersNDC.size();
// 
// 			static glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
// 			glm::vec3 direction = light->GetDirection();
// 			if (direction == up || direction == -up)
// 			{
// 				direction += glm::vec3(0.001f);
// 			}
// 
// 			glm::mat4 lightView = glm::lookAt(centerWorld - direction, centerWorld, up);
// 
// 			glm::vec3 leftCornerView(std::numeric_limits<f32>::max());
// 			glm::vec3 rightCornerView(std::numeric_limits<f32>::min());
// 
// 			for (const glm::vec3& cornerWorld : cornersWorld)
// 			{
// 				glm::vec3 cornerView = lightView * glm::vec4(cornerWorld, 1.0f);
// 
// 				leftCornerView = MathHelper::MinPerComponent(leftCornerView, cornerView);
// 				rightCornerView = MathHelper::MaxPerComponent(rightCornerView, cornerView);
// 			}
// 
// 			f32 size = glm::max(glm::distance(cornersWorld[0], cornersWorld[6]), glm::distance(cornersWorld[0], cornersWorld[2]));
// 
// 			f32 width = rightCornerView.x - leftCornerView.x;
// 			f32 height = rightCornerView.y - leftCornerView.y;
// 
// 			size = glm::max(size, glm::min(width, height));
// 
// 			{
// 				f32 diff = size - width;
// 				leftCornerView.x -= diff / 2.0f;
// 				rightCornerView.x += diff / 2.0f;
// 			}
// 
// 			{
// 				f32 diff = size - height;
// 				leftCornerView.y -= diff / 2.0f;
// 				rightCornerView.y += diff / 2.0f;
// 			}
// 
// 			leftCornerView.z *= light->GetShadowMapZMultiplier();
// 			rightCornerView.z *= light->GetShadowMapZMultiplier();
// 
// 			f32 pixelSize = size / m_Parameters.DrawFramebuffer->GetWidth();
// 
// 			leftCornerView.x = std::round(leftCornerView.x / pixelSize) * pixelSize;
// 			leftCornerView.y = std::round(leftCornerView.y / pixelSize) * pixelSize;
// 			rightCornerView.x = std::round(rightCornerView.x / pixelSize) * pixelSize;
// 			rightCornerView.y = std::round(rightCornerView.y / pixelSize) * pixelSize;
// 
// 			glm::mat4 lightProjection = glm::ortho(leftCornerView.x, rightCornerView.x, leftCornerView.y, rightCornerView.y, leftCornerView.z, rightCornerView.z);
// 
// 			matrices.push_back(lightProjection * lightView);
// 		}
// 	}
// 
// 	return matrices;
// }
