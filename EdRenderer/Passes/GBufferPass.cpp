#include "EdRendererPrivate.h"
#include "GBufferPass.h"

// 
// void GBufferPass::Initialize(std::shared_ptr<RenderGraph> graph)
// {
// 	RenderPass<GBufferPassParameters, GBufferPassShaderParameters>::Initialize(graph);
// 
// 	m_Parameters.Name = "GBuffer pass";
// 	m_Parameters.Shader = RenderingHelper::CreateShader("shaders\\deferred\\geometry-pass.glsl");
// 
// 	m_Parameters.bUseBlending = false;
// 
// 	m_Parameters.bClearColors = true;
// 	m_Parameters.bClearDepth = true;
// 
// 	for (i32 i = 0; i < m_JitterSequenceSize; ++i)
// 	{
// 		m_JitterSequence.push_back(glm::vec2(2.0f * MathHelper::Halton(i + 1, 2) - 1.0f, 2.0f * MathHelper::Halton(i + 1, 3) - 1.0f));
// 	}
// }
// 
// void GBufferPass::Execute()
// {
// 	RenderPass<GBufferPassParameters, GBufferPassShaderParameters>::Execute();
// 
// 	glm::vec2 size = m_Renderer->GetViewportSize();
// 	m_Parameters.DrawFramebuffer->Resize(size.x, size.y, 1);
// 
// 	SetCameraInformation();
// 
// 	for (const std::shared_ptr<StaticMeshComponent>& component : m_Parameters.Meshes.Get())
// 	{
// 		if (std::shared_ptr<StaticMesh> mesh = component->GetStaticMesh())
// 		{
// 			Transform worldTransform = component->GetWorldTransform();
// 			Transform previousWorldTransform = component->GetPreviousWorldTransform();
// 
// 			for (const std::shared_ptr<StaticSubmesh>& submesh : mesh->GetSubmeshes())
// 			{
// 				if (std::shared_ptr<Material> material = submesh->GetMaterial())
// 				{
// 					m_ShaderParameters.Material_BaseColor = material->GetBaseColor();
// 
// 					SetTextureOrWhite(m_ShaderParameters.Material_BaseColorTexture, material->GetBaseColorTexture());
// 					SetTextureOrWhite(m_ShaderParameters.Material_NormalTexture, material->GetNormalTexture());
// 					SetTextureOrWhite(m_ShaderParameters.Material_RoughnessTexture, material->GetRoughnessTexture());
// 					SetTextureOrWhite(m_ShaderParameters.Material_MetalicTexture, material->GetMetalicTexture());
// 					
// 					m_ShaderParameters.Material_PerformNormalMapping = material->ShouldPerformNormalMapping();
// 
// 					m_ShaderParameters.PreviousModelMatrix = previousWorldTransform.GetMatrix();
// 					m_ShaderParameters.ModelMatrix = worldTransform.GetMatrix();
// 					m_ShaderParameters.NormalMatrix = worldTransform.GetInversedTransposedMatrix();
// 
// 					m_ShaderParameters.Material_Roughness = material->GetRoughness();
// 					m_ShaderParameters.Material_Metalic = material->GetMetalic();
// 					m_ShaderParameters.Material_Emission = material->GetEmission();
// 
// 					SubmitShaderParameters();
// 
// 					m_Context->SetVertexBuffer(submesh->GetVertexBuffer());
// 					m_Context->SetIndexBuffer(submesh->GetIndexBuffer());
// 					m_Context->Draw();
// 
// 					m_Renderer->StaticMeshesDrawn++;
// 				}
// 			}
// 		}
// 	}
// }
// 
// void GBufferPass::SetCameraInformation()
// {
// 	Camera& camera = m_Parameters.Camera->GetCamera();
// 
// 	glm::mat4 view = camera.GetView();
// 	glm::mat4 projection = camera.GetProjection();
// 
// 	glm::vec2 size = glm::vec2(m_Parameters.DrawFramebuffer->GetWidth(), m_Parameters.DrawFramebuffer->GetHeight());
// 
// 	glm::vec2 jitter = m_JitterSequence[m_CurrentJitterIndex] / size;
// 	glm::vec2 previousJitter = m_JitterSequence[(m_CurrentJitterIndex - 1 + m_JitterSequenceSize) % m_JitterSequenceSize] / size;
// 
// 	bool bIsTAAEnabled = m_Renderer->GetAAMethod() == AAMethod::TAA;
// 
// 	if (bIsTAAEnabled)
// 	{
// 		projection = glm::translate(glm::mat4(1.0f), glm::vec3(jitter - previousJitter, 0.0f)) * projection;
// 		camera.SetProjection(projection);
// 	}
// 
// 	m_Renderer->SetCamera(view, projection, camera.GetPosition());
// 
// 	if (bIsTAAEnabled)
// 	{
// 		m_Context->SetShaderDataFloat2("u_PreviousJitter", previousJitter);
// 		m_Context->SetShaderDataFloat2("u_Jitter", jitter);
// 	}
// 	else
// 	{
// 		m_Context->SetShaderDataMat4("u_PreviousProjectionMatrix", projection);
// 	}
// 
// 	m_Context->SetShaderDataMat4("u_PreviousViewMatrix", m_PreviousView);
// 
// 	m_CurrentJitterIndex = (m_CurrentJitterIndex + 1) % m_JitterSequenceSize;
// 	m_PreviousView = view;
// }

GBufferPass::GBufferPass() : RenderPass("GBufferPass")
{
}

void GBufferPass::Initialize(RenderGraph* graph)
{
  RenderPass::Initialize(graph);

  m_Albedo = graph->CreateRenderTarget("GBuffer.Albedo", PixelFormat::RGBA16F, RenderTargetSizePolicy::Full, false);
  m_Normal = graph->CreateRenderTarget("GBuffer.Normal", PixelFormat::RGBA16F, RenderTargetSizePolicy::Full, false);
  m_Material = graph->CreateRenderTarget("GBuffer.Material", PixelFormat::RGBA16F, RenderTargetSizePolicy::Full, false);
  m_Velocity = graph->CreateRenderTarget("GBuffer.Velocity", PixelFormat::RG16F, RenderTargetSizePolicy::Full, false);
  m_Depth = graph->GetRenderTarget("GBuffer.Depth");

  {
    GraphicsPipelineStateObjectBuilder builder;
    builder.SetRootSignature(m_Renderer->GetRootSignature())
      .SetShader("GBuffer.h")
      .SetDepthStencil(true, false)
      .SetDepthFormat(PixelFormat::Depth)
      .SetDepthFunction(ComparisonFunction::GreaterEqual)
      .AddInputElement("Position", 0, PixelFormat::RGB32F)
      .AddInputElement("Color", 0, PixelFormat::RGBA32F)
      .AddInputElement("UVs", 0, PixelFormat::RGB32F)
      .AddInputElement("Normals", 0, PixelFormat::RGB32F)
      .AddInputElement("Tangents", 0, PixelFormat::RGB32F)
      .AddInputElement("Bitangent", 0, PixelFormat::RGB32F);

    builder.AddRenderTarget().SetFormat(PixelFormat::RGBA16F).SetColorBlend(BlendFactor::One, BlendFactor::Zero, BlendOperation::Add);
    builder.AddRenderTarget().SetFormat(PixelFormat::RGBA16F);
    builder.AddRenderTarget().SetFormat(PixelFormat::RGBA16F);
    builder.AddRenderTarget().SetFormat(PixelFormat::RG16F);


    m_PSO = m_Context->CreatePipelineStateObject("GBufferBase", builder);
  }
}

u32 GBufferPass::GetCustomShaderParametersStructSize()
{
  const RenderScene& scene = m_Renderer->GetScene();
  const u32 meshCount = scene.StaticMeshes.GetSize();
  return meshCount * sizeof(ObjectParameters);
}

void GBufferPass::GatherCustomShaderParameters(void* memory)
{
  const RenderScene& scene = m_Renderer->GetScene();
  const u32 meshCount = scene.StaticMeshes.GetSize();

  if (meshCount == 0)
  {
    return;
  }

  ObjectParameters* data = reinterpret_cast<ObjectParameters*>(memory);

  DescriptorHeap* srvHeap = m_Context->GetSRVHeap();
  for (u32 index = 0; index < meshCount; ++index)
  {
    const StaticMeshElement& element = scene.StaticMeshes[index];

    ObjectParameters& parameters = data[index];

    glm::mat4 worldMatrix = element.WorldTransform->GetMatrix();
    memcpy(parameters.WorldTransform, glm::value_ptr(worldMatrix), sizeof(f32) * 16);

    glm::mat4 previousWorldMatrix = element.PreviousWorldTransform->GetMatrix();
    memcpy(parameters.PreviousWorldTransfom, glm::value_ptr(previousWorldMatrix), sizeof(f32) * 16);
  }
}

void GBufferPass::Execute(CommandList* list, Resource* buffer, u64 offset)
{
  const RenderScene& scene = m_Renderer->GetScene();
  const u32 meshCount = scene.StaticMeshes.GetSize();

  if (meshCount == 0)
  {
    return;
  }

  ResourceView targets[] =
  {
    m_Albedo->GetView(),
    m_Normal->GetView(),
    m_Material->GetView(),
    m_Velocity->GetView()
  };

  list->Transition(targets, ResourceState::RenderTarget);

  // https://www.gamedev.net/forums/topic/684708-why-call-discardresource/
  for (const auto& target_view : targets)
  {
    list->DiscardResource(target_view);
  }

  list->Transition(m_Context->GetGeometryVertexPool(), ResourceState::VertexAndConstantBuffer);
  list->Transition(m_Context->GetGeometryIndexPool(), ResourceState::IndexBuffer);

  list->SetRenderTargets(targets, std::size(targets), m_Depth->GetView());
  list->SetPrimitiveTopology(PrimitiveTopology::TriangleList);

  list->SetPipelineState(m_PSO);

  Window* window = m_Context->GetWindow();
  list->SetViewport({ 0.0f, 0.0f }, { window->GetWidth(), window->GetHeight() }, { 0.0f, 1.0f });
  list->SetScissor({ 0.0f, 0.0f }, { window->GetWidth(), window->GetHeight() });

  list->SetGraphicsConstantBufferView(1, buffer, offset);

  for (u32 index = 0; index < meshCount; ++index)
  {
    const StaticMeshElement& element = scene.StaticMeshes[index];

    u32 constants[20];
    element.PackRootParameters(constants);
    constants[16] = index;

    list->SetGraphicsRootConstants(12, 20, 0, constants);

    list->SetVertexBuffer(element.VertexBufferView, 0, m_PSO->GetVertexStride());
    list->SetIndexBuffer(element.IndexBufferView);

    list->DrawIndexed(0, element.IndexBufferView.Size / sizeof(u32), 0);
  }

  list->Transition(targets, ResourceState::ShaderRead);
  list->Transition(m_Depth->GetView(), ResourceState::DepthRead);
}