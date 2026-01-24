#include "EdRendererPrivate.h"
#include "LightingPass.h"
#include "Helpers/GeometryBuilder.h"
#include "Helpers/MathHelper.h"

#define POINT_LIGHT_MESH_SECTOR_COUNT 30
#define POINT_LIGHT_MESH_STACK_COUNT 30
#define SPOT_LIGHT_MESH_SECTOR_COUNT 50
#define SHADOW_SAMPLES_BLOCK_DIMENSTIONS 10
#define SHADOW_SAMPLES_BLOCK_COUNT 10

LightingPass::LightingPass() : RenderPass("LigthingPass")
{

}

void LightingPass::Initialize(RenderGraph* graph)
{
    RenderPass::Initialize(graph);

    m_Lighting = graph->CreateRenderTarget("Lighting", PixelFormat::RGBA16F, RenderTargetSizePolicy::Full);
    m_Albedo = graph->GetRenderTarget("GBuffer.Albedo");
    m_Material = graph->GetRenderTarget("GBuffer.Material");
    m_Depth = graph->GetRenderTarget("GBuffer.Depth");
    m_ShadowMaps = graph->GetRenderTarget("LocalShadowMaps");
    m_SSAO = graph->GetUAVTarget("SSAO2");

    {
        GraphicsPipelineStateObjectBuilder builder;
        builder.SetRootSignature(m_Renderer->GetRootSignature())
            .SetShader("BaseLighting.h")
            .AddInputElement("Position", 0, PixelFormat::RG32F)
            .AddRenderTarget().SetFormat(PixelFormat::RGBA16F);

        m_BaseLightingPSO = RenderingContext::Get().CreatePipelineStateObject("BaseLighting", builder);
    }

    ccstr8 psoNames[2] = {
      "POINT_LIGHT_TYPE",
      "SPOT_LIGHT_TYPE"
    };

    for (u32 i = 0; i < 2; ++i)
    {
        GraphicsPipelineStateObjectBuilder builder;
        builder.SetRootSignature(m_Renderer->GetRootSignature())
            .SetShader("LightsLighting.h")
            .AddShaderDefine("LIGHT_TYPE", i + 1)
            .AddInputElement("Position", 0, PixelFormat::RGB32F)
            .SetCullFace(PrimitiveCullingMode::Front)
            .SetDepthClip(false)
            .AddRenderTarget().SetColorBlend(BlendFactor::One, BlendFactor::One, BlendOperation::Add).SetFormat(PixelFormat::RGBA16F);

        ccstr8 name = Strings::Concat(32, "Lights_", psoNames[i]);
        m_LightsPSO[i] = m_Context->CreatePipelineStateObject(name, builder);
    }

    {
        TemporaryArray<glm::vec3> vertices;
        TemporaryArray<u32> indices;
        GeometryBuilder::MakeSphere(1.0f, POINT_LIGHT_MESH_SECTOR_COUNT, POINT_LIGHT_MESH_STACK_COUNT, vertices, indices);
        m_Context->UploadModelDeferred(&vertices[0], sizeof(glm::vec3), vertices.GetSize(), m_PointLightMeshVertexBufferView, &indices[0], indices.GetSize(), m_PointLightMeshIndexBufferView);
    }

    {
        TemporaryArray<glm::vec3> vertices;
        TemporaryArray<u32> indices;
        GeometryBuilder::MakeCone(SPOT_LIGHT_MESH_SECTOR_COUNT, vertices, indices);
        m_Context->UploadModelDeferred(&vertices[0], sizeof(glm::vec3), vertices.GetSize(), m_SpotLightMeshVertexBufferView, &indices[0], indices.GetSize(), m_SpotLightMeshIndexBufferView);
    }

    {
        Array<glm::vec2> samples = MathHelper::GenerateCircleSamples(SHADOW_SAMPLES_BLOCK_DIMENSTIONS * SHADOW_SAMPLES_BLOCK_DIMENSTIONS * SHADOW_SAMPLES_BLOCK_COUNT);
        m_Context->UploadTextureDeferred(reinterpret_cast<u8*>(samples.Get()), SHADOW_SAMPLES_BLOCK_DIMENSTIONS, SHADOW_SAMPLES_BLOCK_DIMENSTIONS * SHADOW_SAMPLES_BLOCK_COUNT, 1, PixelFormat::RG32F, m_ShadowNoise, ResourceState::ShaderRead, "ShadowNoise");
    }
}

void LightingPass::GetWaitSignals(ccstr8** names, u32& count)
{
    static ccstr8 depencnies[] = { "SSAOPass" };
    (*names) = depencnies;
    count = std::size(depencnies);
}

u32 LightingPass::GetCustomShaderParametersStructSize()
{
    const RenderScene& scene = m_Renderer->GetScene();
    return 256 * (1 + scene.PointLights.GetSize() + scene.SpotLights.GetSize());
}

void LightingPass::GatherCustomShaderParameters(void* memory)
{
    void* buffer = memory;

    DescriptorHeap* heap = m_Context->GetSRVHeap();
    buffer = PutValue(buffer, heap->GetDescriptorIndex(m_Albedo->GetSRV()));
    buffer = PutValue(buffer, heap->GetDescriptorIndex(m_Material->GetSRV()));

    const RenderScene& scene = m_Renderer->GetScene();

    memory = OffsetBuffer(memory, 256);

    u32 casters = 0;
    for (const PointLightElement& element : scene.PointLights)
    {
        PointLightParameters& paramters = *reinterpret_cast<PointLightParameters*>(memory);

        paramters.Position = element.Position;
        paramters.Color = element.Color;

        if (element.IsShadowCasting)
        {
            paramters.ShadowMapStart.x = (casters % MAX_POINT_LIGHTS_IN_COLUMN) * LOCAL_SHADOW_MAP_DIMENTIONS * 6;
            paramters.ShadowMapStart.y = (casters / MAX_POINT_LIGHTS_IN_COLUMN) * LOCAL_SHADOW_MAP_DIMENTIONS;
        }
        else
        {
            paramters.ShadowMapStart.x = -1;
            paramters.ShadowMapStart.y = -1;
        }

        paramters.ShadowMapSize = glm::u32vec2(LOCAL_SHADOW_MAP_DIMENTIONS, LOCAL_SHADOW_MAP_DIMENTIONS);

        paramters.ViewPosition = scene.Camera.Position;

        paramters.Radius = element.Radius;
        paramters.Intensity = element.Intensity;

        paramters.ShadowViewDepthConversion0 = element.NearPlane / (element.NearPlane - element.FarPlane);
        paramters.ShadowViewDepthConversion1 = (element.NearPlane * element.FarPlane) / (element.FarPlane - element.NearPlane);
        paramters.ShadowFilterRadius = element.ShadowFilterRadius;
        paramters.ShadowFilterSize = element.ShadowFilterSize;

        memory = OffsetBuffer(memory, 256);

        casters += element.IsShadowCasting;
    }

    casters = 0;
    for (const SpotLightElement& element : scene.SpotLights)
    {
        SpotLightParameters& paramters = *reinterpret_cast<SpotLightParameters*>(memory);

        paramters.WorldTransform = element.CalculateModelTransform();
        paramters.ShadowProjectionViewTransform = element.CalculateShadowTransform();

        paramters.Position = element.WorldTransform->GetTranslation();
        paramters.InnerAngle = glm::cos(element.InnerAngle);

        paramters.Forward = element.WorldTransform->GetForward();
        paramters.OuterAngle = glm::cos(element.OuterAngle);

        paramters.Color = element.Color;
        paramters.MaxDistance = element.MaxDistance;

        if (element.IsShadowCasting)
        {
            paramters.ShadowMapStart.x = (MAX_POINT_LIGHTS_IN_COLUMN * 6 + (casters % MAX_POINT_LIGHTS_IN_COLUMN)) * LOCAL_SHADOW_MAP_DIMENTIONS;
            paramters.ShadowMapStart.y = (casters / MAX_POINT_LIGHTS_IN_COLUMN) * LOCAL_SHADOW_MAP_DIMENTIONS;
        }
        else
        {
            paramters.ShadowMapStart.x = -1;
            paramters.ShadowMapStart.y = -1;
        }

        paramters.ShadowMapSize = glm::u32vec2(LOCAL_SHADOW_MAP_DIMENTIONS, LOCAL_SHADOW_MAP_DIMENTIONS);

        paramters.ViewPosition = scene.Camera.Position;
        paramters.Intensity = element.Intensity;
        
        paramters.ShadowFilterRadius = element.ShadowFilterRadius;
        paramters.ShadowFilterSize = element.ShadowFilterSize;

        memory = OffsetBuffer(memory, 256);
    }
}

void LightingPass::Execute(CommandList* list, Resource* buffer, u64 offset)
{
    TemporaryArray<Resource*> transitions;
    transitions.Add(m_Albedo->GetSRV().Viewed);
    transitions.Add(m_Material->GetSRV().Viewed);
    transitions.Add(m_ShadowMaps->GetSRV().Viewed);
    transitions.Add(m_SSAO->GetSRV().Viewed);
    list->Transition(transitions, ResourceState::ShaderRead);
    list->Transition(m_Depth->GetView(), ResourceState::DepthRead);

    list->Transition(m_Lighting->GetView(), ResourceState::RenderTarget);
    list->SetRenderTargets(m_Lighting->GetView(), {});

    list->SetPipelineState(m_BaseLightingPSO);
    list->SetPrimitiveTopology(PrimitiveTopology::TriangleList);

    Window* window = m_Context->GetWindow();
    list->SetViewport({ 0.0f, 0.0f }, { window->GetWidth(), window->GetHeight() }, { 0.0f, 1.0f });
    list->SetScissor({ 0.0f, 0.0f }, { window->GetWidth(), window->GetHeight() });

    list->SetGraphicsConstantBufferView(1, buffer, offset);
    list->SetGraphicsRootDescriptorTable(4, m_Albedo->GetSRV().GPUHandle);
    list->SetGraphicsRootDescriptorTable(5, m_ShadowNoise.GPUHandle);
    list->SetGraphicsRootDescriptorTable(6, m_ShadowMaps->GetSRV().GPUHandle);
    list->SetGraphicsRootDescriptorTable(7, m_Depth->GetSRV().GPUHandle);

    m_Renderer->DrawFullscreenQuad(list);

    const RenderScene& scene = m_Renderer->GetScene();

    if (scene.PointLights.GetSize())
    {
        list->SetPipelineState(m_LightsPSO[0]);
    
        list->SetVertexBuffer(m_PointLightMeshVertexBufferView, 0, m_LightsPSO[0]->GetVertexStride());
        list->SetIndexBuffer(m_PointLightMeshIndexBufferView);
    
        for (u32 i = 0; i < scene.PointLights.GetSize(); ++i)
        {
            list->SetGraphicsConstantBufferView(1, buffer, offset + 256 * (i + 1));
            list->DrawIndexed(0, m_PointLightMeshIndexBufferView.Size / sizeof(u32), 0);
        }
    }

    if (scene.SpotLights.GetSize())
    {
        list->SetPipelineState(m_LightsPSO[1]);

        list->SetVertexBuffer(m_SpotLightMeshVertexBufferView, 0, m_LightsPSO[1]->GetVertexStride());
        list->SetIndexBuffer(m_SpotLightMeshIndexBufferView);

        for (u32 i = 0; i < scene.SpotLights.GetSize(); ++i)
        {
            list->SetGraphicsConstantBufferView(1, buffer, offset + 256 * (scene.PointLights.GetSize() + i + 1));
            list->DrawIndexed(0, m_SpotLightMeshIndexBufferView.Size / sizeof(u32), 0);
        }
    }

    list->Transition(m_Lighting->GetView(), ResourceState::ShaderRead);
    list->Transition(m_SSAO->GetSRV().Viewed, ResourceState::Common);
}
