#include "EdRendererPrivate.h"
#include "Renderer.h"

#include "Helpers/RenderingHelper.h"

#include "Passes/DepthPrepass.h"
#include "Passes/GBufferPass.h"
#include "Passes/SSAOPass.h"
#include "Passes/MipMappingPass.h"
#include "Passes/ShadowsPass.h"
#include "Passes/BloomPass.h"
#include "Passes/LightingPass.h"
#include "Passes/ResolutionPass.h"
#include "Passes/AAPass.h"

#include "Passes/EmissionPass.h"
#include "Passes/AmbientPass.h"
#include "Passes/Lighting/DirectionalLight/DirectionalLightMultiPass.h"
#include "Passes/Lighting/SpotLight/SpotLightMultiPass.h"
#include "Passes/Lighting/PointLight/PointLightMultiPass.h"
#include "Passes/GrayscalePass.h"
#include "Passes/Editor/IconsPass.h"

#include "visualization.h"
#include "Passes/visualization_pass.h"

#include "RenderScene.h"


static auto& r_visualization_mode = console::create_u32("r_visualization_mode", visualization::none, visualization::none, visualization::max);

void Renderer::Initialize(Window* window)
{
	ED_LOG(Renderer, info, "Initalizing Renderer");

	m_Context = new RenderingContext();
	m_Context->Initialize(window);

    {
        RootSignatureBuilder builder;
        builder.AddParameter(DescriptorHeapType::CBV, 0, 0, SVF_All);
        builder.AddParameter(DescriptorHeapType::CBV, 1, 0, SVF_All);
        builder.AddParameter(DescriptorHeapType::CBV, 2, 0, SVF_All);
        builder.AddParameter(DescriptorHeapType::CBV, 3, 0, SVF_All);
        
		builder.AddParameter(0, 0, DescriptorRangeType::SRV, 1024, 0, SVF_All);
		builder.AddParameter(0, 1, DescriptorRangeType::SRV, 1024, 0, SVF_All);
        builder.AddParameter(0, 2, DescriptorRangeType::SRV, 1024, 0, SVF_All);
        builder.AddParameter(0, 3, DescriptorRangeType::SRV, 1024, 0, SVF_All);

        builder.AddParameter(0, 0, DescriptorRangeType::UAV, 1, 0, SVF_All);
        builder.AddParameter(1, 0, DescriptorRangeType::UAV, 1, 0, SVF_All);
        builder.AddParameter(2, 0, DescriptorRangeType::UAV, 1, 0, SVF_All);
        builder.AddParameter(3, 0, DescriptorRangeType::UAV, 1024, 0, SVF_All);
		
		builder.AddParameter(0, 1, 20, SVF_All);

		for (u32 wrap = 0; wrap < static_cast<u32>(WrapMode::Count); ++wrap)
		{
			WrapMode casted = static_cast<WrapMode>(wrap);
			builder.AddStaticSampler(0, wrap, FilteringMode::Trilinear, casted, casted, casted, 1,  ComparisonFunction::LessEqual, SVF_All);
			builder.AddStaticSampler(1, wrap, FilteringMode::Bilinear,  casted, casted, casted, 1,  ComparisonFunction::LessEqual, SVF_All);
			builder.AddStaticSampler(2, wrap, FilteringMode::Point,     casted, casted, casted, 1,  ComparisonFunction::LessEqual, SVF_All);
			builder.AddStaticSampler(3, wrap, FilteringMode::Aniso,     casted, casted, casted, 16, ComparisonFunction::LessEqual, SVF_All);
		}

        builder.AddFlags(RSF_AllowInputAssembler);
        builder.AddFlags(RSF_CBV_SRV_UAV_HEAP_DirectlyIndexed);
        m_RootSignature = m_Context->CreateRootSignature(builder);
    }

	// Querying it early, so that there would not be a frame delay first time it is used.
	RenderingHelper::GetWhiteTexture();

	{
        static f32 vertices[] = {
            -1.0f, -1.0f,
            -1.0f,  3.0f,
             3.0f, -1.0f,
        };

		static u32 indices[] = { 0, 1, 2 };

		RenderingContext::Get().UploadModelDeferred(vertices, 2 * sizeof(f32), 3, m_QuadVertexView, indices, 3, m_QuadIndexView);
	}

	/*

        PipelineStateObject::GraphicsPipelineStateObjectBuilder builder;

        ShaderProgram* program = new ShaderProgram();
        Shader* vertex = new Shader(ST_Vertex, );
        Shader* pixel = new Shader(ST_Pixel, "shaders\\Icon.hlsl");
        program->AttachShader(vertex);
        program->AttachShader(pixel);

        builder.SetShaderProgram("shaders\\Icon.hlsl", static_cast<ShaderType>(ST_Vertex | ST_Pixel));
        builder.AddInputElement("Position", 0, PixelFormat::RG32F, 0);
        builder.AddInputElement("TextureCoord", 1, PixelFormat::RG32F, 0);

        builder.AddRenderTarget()
            .SetColorBlend(BlendFactor::SourceAlpha, BlendFactor::InvertedSourceAlpha, BlendOperation::Add);


        m_QuadPSO = new PipelineStateObject();
	*/

	//{
	//	m_Graph = new RenderGraph();
	//	m_Graph->Initilaize(this);
	//
	//	//m_Graph->DeclareObjectPtrParameter("Camera", m_Camera); // Level transitions ? Will it crash here ? ;) yes it did :)
	//	//
	//	//m_Graph->DeclareParameter("Scene.Component", m_Components);
	//	//m_Graph->DeclareParameter("Scene.StaticMesh", m_StaticMeshes);
	//	//m_Graph->DeclareParameter("Scene.PointLight", m_PointLights);
	//	//m_Graph->DeclareParameter("Scene.DirectionalLight", m_DirectionalLights);
	//	//m_Graph->DeclareParameter("Scene.SpotLight", m_SpotLights);
	//	//
	//	//m_Graph->AddPass<GBufferPass>();
	//	//
	//	//m_Graph->AddPass<SSAOMultiPass>();
	//	//
	//	//m_Graph->AddPass<AmbientPass>();
	//	//m_Graph->AddPass<EmissionPass>();
	//	//
	//	//m_Graph->AddPass<DirectionalLightMultiPass>();
	//	//m_Graph->AddPass<SpotLightMultiPass>();
	//	//m_Graph->AddPass<PointLightMultiPass>();
	//	//
	//	//m_Graph->AddPass<FXAAPass>();
	//	//m_Graph->AddPass<TAAPass>();
	//	//
	//	//m_Graph->AddPass<BloomMultiPass>();
	//	//
	//	//m_Graph->AddPass<ResolutionPass>();
	//	//
	//	//m_Graph->AddPass<GrayscalePass>();
	//	//
	//	//m_Graph->AddPass<IconsPass>();
	//
	//	m_Graph->Build();
	//}

	m_Graph = new RenderGraph();
	m_Graph->Initialize();

	m_Graph->AddRenderPass<DepthPrepass>();
	m_Graph->AddRenderPass<GBufferPass>();
    m_Graph->AddRenderPass<SSAOPass>();
    m_Graph->AddRenderPass<ShadowsPass>();
    m_Graph->AddRenderPass<MipMappingPass>();
    m_Graph->AddRenderPass<LightingPass>();
    m_Graph->AddRenderPass<BloomPass>();
    m_Graph->AddRenderPass<ResolutionPass>();

	m_Graph->InitializePasses();

	SetSSAOEnabled(m_bSSAOEnabled);
	SetBloomEnabled(m_bIsBloomEnabled);

	SetAAMethod(m_AAMethod);
}

void Renderer::Deinitialize()
{
//	delete m_Graph;
//	m_Graph = nullptr;

	m_Context->Wait();

	delete m_Graph;
	m_Graph = nullptr;

	m_Context->Deinitialize();
	delete m_Context;
	m_Context = nullptr;
}

void Renderer::PreFrameUpdate()
{
	RenderingContext::Get().PreFrameUpdate();
}

void Renderer::RenderFrame(const RenderScene& scene, f32 deltaSeconds)
{
	m_Scene = &scene;

	{
		auto& parameters = visualization::parameters;

		parameters.mode = static_cast<visualization::modes>(r_visualization_mode());
		parameters.custom_target = ResourceView();
	}

#if DEBUG_BUILD == 1 || DEVELOPMENT_BUILD == 1
	m_Stats.Reset();
#endif

	m_Camera.SetProjection(scene.Camera.Fov, m_Camera.GetAspect(), scene.Camera.Near, scene.Camera.Far);
	m_Camera.SetPosition(scene.Camera.Position);
	m_Camera.SetRotation(scene.Camera.Rotation);

	if (m_bIsViewportSizeDirty)
	{
		m_Camera.SetProjection(90.0f, 1.0f * m_ViewportSize.x / m_ViewportSize.y, 1.0f, m_FarPlane);
	}

	// Shader parameters gather stage.
	m_Graph->Gather();

	// Execute stage.
	m_Graph->Execute();

    m_Context->Wait();

    RenderingContext::Get().Present();

	m_bIsViewportSizeDirty = false;

	m_Scene = nullptr;
}

void Renderer::PostFrameUpdate()
{
	RenderingContext::Get().PostFrameUpdate();
}

bool Renderer::IsViewportSizeDirty() const
{
	return m_bIsViewportSizeDirty;
}

void Renderer::ResizeViewport(glm::vec2 size)
{
	if (m_ViewportSize != glm::ivec2(size))
	{
		m_bIsViewportSizeDirty = true;
		m_ViewportSize = size;
	}
}

glm::u32vec2 Renderer::GetViewportSize() const
{
	return m_ViewportSize;
}

void Renderer::SetSSAOEnabled(bool enabled)
{
	m_bSSAOEnabled = enabled;
}

bool Renderer::IsSSAOEnabled() const
{
    return m_bSSAOEnabled;
}

void Renderer::SetBloomEnabled(bool enabled)
{
	m_bIsBloomEnabled = enabled;
}

bool Renderer::IsBloomEnabled() const
{
	return m_bIsBloomEnabled;
}

void Renderer::SetUpsampleScale(f32 scale)
{
	m_UpsampleScale = scale;
}

f32 Renderer::GetUpsampleScale() const
{
	return m_UpsampleScale;
}

AAMethod Renderer::GetAAMethod() const
{
	return m_AAMethod;
}

void Renderer::SetAAMethod(AAMethod method)
{
	m_AAMethod = method;
}

f32 Renderer::GetFarPlane() const
{
	return m_FarPlane;
}

Renderer::Renderer()
{
	ED_ASSERT(!s_Renderer, "Can not create two renderers at the same time.");
	s_Renderer = this;
}

Renderer::~Renderer()
{
	s_Renderer = nullptr;
}

// void Renderer::SetCamera(const Camera& camera)
// {
// 	glm::mat4 view = camera.GetView();
// 	glm::mat4 projection = camera.GetProjection();
// 	
// 	RenderingContext::Get().SetShaderDataMat4("u_ViewMatrix", view);
// 	RenderingContext::Get().SetShaderDataMat4("u_ProjectionMatrix", projection);
// 	RenderingContext::Get().SetShaderDataMat4("u_ProjectionViewMatrix", projection * view);
// 	RenderingContext::Get().SetShaderDataMat4("u_InvProjectionViewMatrix", glm::inverse(projection * view));
// 
// 	RenderingContext::Get().SetShaderDataFloat3("u_ViewPosition", camera.GetPosition());
// 
// 	RenderingContext::Get().SetShaderDataFloat("u_FarPlane", camera.GetFar());
// }
// 
// void Renderer::SetCamera(const glm::mat4& view, const glm::mat4& projection, glm::vec3 viewPosition)
// {
// 	RenderingContext::Get().SetShaderDataMat4("u_ViewMatrix", view);
// 	RenderingContext::Get().SetShaderDataMat4("u_ProjectionMatrix", projection);
// 	RenderingContext::Get().SetShaderDataMat4("u_ProjectionViewMatrix", projection * view);
// 	RenderingContext::Get().SetShaderDataFloat3("u_ViewPosition", viewPosition);
// }

void Renderer::DrawFullscreenQuad(CommandList* list)
{
	list->SetVertexBuffer(m_QuadVertexView, 0, 2 * sizeof(f32));
	list->SetIndexBuffer(m_QuadIndexView);
	list->DrawIndexed(0, 3, 0);
}

ResourceView Renderer::GetDisplayTarget() const
{
	return RenderingContext::Get().GetDisplayBufferSRV();
}

void Renderer::RequestMipMapping(ResourceView& view, MipMappingMode mode, u32 mips)
{
	ED_ASSERT(view.Viewed, "Can not create mips for invalid resource.");
	ED_ASSERT(mips <= 5, "Can not create more than 5 mips.");

	MipMappingRequest& request = m_Requests.Add();
	request.View = view;
	request.Mode = mode;
	request.Mips = mips;

	m_Context->RequsetDefferedTransition(view, ResourceState::Common);
}