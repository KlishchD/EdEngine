#include "EdRendererPrivate.h"
#include "ShadowsPass.h"

ShadowsPass::ShadowsPass() : RenderPass("ShadowPass")
{
}

void ShadowsPass::Initialize(RenderGraph* graph)
{
    RenderPass::Initialize(graph);

    m_LocalShadowMaps = graph->CreateRenderTarget("LocalShadowMaps", PixelFormat::Depth, LOCAL_SHADOW_MAPS_DIMENTIONS, LOCAL_SHADOW_MAPS_DIMENTIONS);

    {
        GraphicsPipelineStateObjectBuilder builder;
        builder.SetRootSignature(m_Renderer->GetRootSignature())
            .SetShaderProgram("Shaders\\hlsl\\LocalShadows.h", static_cast<ShaderType>(ST_Vertex | ST_Geometry | ST_Pixel))
            .SetDepthStencil(true, false)
            .SetDepthFormat(PixelFormat::Depth)
            .SetDepthFunction(ComparisonFunction::Greater)
            .SetCullFace(PrimitiveCullingMode::Front)   
            .AddInputElement("Position", 0, PixelFormat::RGB32F);

        m_LoacalShadowsPSO = RenderingContext::Get().CreatePipelineStateObject("LocalShadowsPSO", builder);

        // I will probably regret making it a direct value
        // but I am more interested in getting it working now.
        // I promise one day it will break and I will fix it ;)
        m_LoacalShadowsPSO->SetVertexStride(76);
    }
}

u32 ShadowsPass::GetCustomShaderParametersStructSize()
{
    // TODO: We could reuse data from GBuffer pass ;)
    const RenderScene& scene = m_Renderer->GetScene();
    const u32 meshCount = scene.StaticMeshes.GetSize();
    const u32 meshCountAligned = Align(meshCount, 4);

    u32 points = 0;
    for (const PointLightElement& element : scene.PointLights)
    {
        points += element.IsShadowCasting;
    }

    u32 spots = 0;
    for (const SpotLightElement& element : scene.SpotLights)
    {
        spots += element.IsShadowCasting;
    }

#if DEBUG_BUILD || DEVELOPMENT_BUILD
    if (points > MAX_SHADOWED_POINT_LIGHT_COUNT)
    {
        ED_LOG(ShadowsPass, err, "Exceeded maximum shadowed point lights count.");
    }

    if (spots > MAX_SHADOWED_SPOT_LIGHTS_COUNT)
    {
        ED_LOG(ShadowsPass, err, "Exceeded maximum shadowed spot lights count.");
    }
#endif

    m_HasShadowCastingLights = points != 0 || spots != 0;

    return meshCountAligned * sizeof(glm::mat4) + sizeof(glm::mat4) * (points * 6 + spots);
}

void ShadowsPass::GatherCustomShaderParameters(void* memory)
{
    if (!m_HasShadowCastingLights)
    {
        return;
    }

    const RenderScene& scene = m_Renderer->GetScene();
    const u32 meshCount = scene.StaticMeshes.GetSize();

    for (u32 i = 0; i < meshCount; ++i)
    {
        const StaticMeshElement& element = scene.StaticMeshes[i];

        glm::mat4 matrix = element.WorldTransform->GetMatrix();
        memory = PutValue(memory, glm::value_ptr(matrix), sizeof(glm::mat4));
    }

    const u32 padding = meshCount % 4;
    if (padding != 0)
    {
        memory = OffsetBuffer<glm::mat4>(memory, 4 - padding);
    }

    u32 points = 0;
    for (const PointLightElement& element : scene.PointLights)
    {
        if (points >= MAX_SHADOWED_POINT_LIGHT_COUNT)
        {
            break;
        }

        if (!element.IsShadowCasting)
        {
            continue;
        }

        glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, element.FarPlane, element.NearPlane);

        for (u32 i = 0; i < 6; ++i)
        {
            f32 sign = i % 2 ? 1.0f : -1.0f;

            u32 direction = i / 2;
            glm::mat4 view = glm::lookAt(element.Position, element.Position + glm::globalDirections[direction] * sign, direction == 1 ? glm::right : glm::up);
            glm::mat4 projectionView = projection * view;
            memory = PutValue(memory, glm::value_ptr(projectionView), sizeof(glm::mat4));
        }

        ++points;
    }

    u32 spots = 0;
    for (const SpotLightElement& element : scene.SpotLights)
    {
        if (spots >= MAX_SHADOWED_SPOT_LIGHTS_COUNT)
        {
            break;
        }

        if (!element.IsShadowCasting)
        {
            continue;
        }

        glm::mat4 transform = element.CalculateShadowTransform();
        memory = PutValue(memory, glm::value_ptr(transform), sizeof(glm::mat4));

        ++spots;
    }
}

void ShadowsPass::Execute(CommandList* list, Resource* buffer, u64 offset)
{
    if (!m_HasShadowCastingLights)
    {
        return;
    }

    const RenderScene& scene = m_Renderer->GetScene();
    const u32 meshCount = scene.StaticMeshes.GetSize();
    const u32 meshCountAligned = Align(meshCount, 4);

    list->Transition(m_LocalShadowMaps->GetView(), ResourceState::DepthWrite);
    list->ClearDepthTarget(m_LocalShadowMaps->GetView(), 0.0f);

    list->SetRenderTargets(ResourceView(), m_LocalShadowMaps->GetView());

    list->SetPrimitiveTopology(PrimitiveTopology::TriangleList);

    list->SetPipelineState(m_LoacalShadowsPSO);

    list->SetGraphicsConstantBufferView(2, buffer, offset + meshCountAligned * sizeof(glm::mat4));

    constexpr u32 maxMarixCountThreshold = 1000;

    glm::vec4 viewports[MAX_VIEWPORTS_COUNT];
    u32 viewport = 0;

    for (u32 i = 0; i < meshCount; ++i)
    {
        const StaticMeshElement& mesh = scene.StaticMeshes[i];

        const u32 modelIndex = i % maxMarixCountThreshold;
        
        if (modelIndex == 0)
        {
            u32 modelsRangesCount = i / maxMarixCountThreshold;
            list->SetGraphicsConstantBufferView(1, buffer, offset + modelsRangesCount * maxMarixCountThreshold * sizeof(glm::mat4));
        }

        list->SetIndexBuffer(mesh.IndexBufferView);
        list->SetVertexBuffer(mesh.VertexBufferView, 0, m_LoacalShadowsPSO->GetVertexStride());

        u32 indices = mesh.IndexBufferView.Size / sizeof(u32);

        u32 shadowOffset = 0;
        u32 points = 0;
        for (const PointLightElement& element : scene.PointLights)
        {
            if (!element.IsShadowCasting)
            {
                continue;
            }

            u32 row = points / MAX_POINT_LIGHTS_IN_COLUMN;
            u32 column = 6 * (points % MAX_POINT_LIGHTS_IN_COLUMN);

            for (u32 i = 0; i < 6; ++i)
            {
                if (AppendViewport(row, column + i, viewports, viewport))
                {
                    Submit(list, modelIndex, shadowOffset, MAX_VIEWPORTS_COUNT, indices, viewports, viewport);
                    shadowOffset += MAX_VIEWPORTS_COUNT;
                }
            }

            ++points;
        }

        if (viewport)
        {
            Submit(list, modelIndex, shadowOffset, viewport, indices, viewports, viewport);
        }

        u32 spots = 0;
        for (const SpotLightElement& element : scene.SpotLights)
        {
            if (!element.IsShadowCasting)
            {
                continue;
            }

            u32 row = spots / MAX_SPOT_LIGHTS_IN_COLUMN;
            u32 column = MAX_POINT_LIGHTS_IN_COLUMN * 6 + spots % MAX_SPOT_LIGHTS_IN_COLUMN;

            if (AppendViewport(row, column, viewports, viewport))
            {
                Submit(list, modelIndex, shadowOffset, MAX_VIEWPORTS_COUNT, indices, viewports, viewport);
                shadowOffset += MAX_VIEWPORTS_COUNT;
            }

            ++spots;
        }

        if (viewport)
        {
            Submit(list, modelIndex, shadowOffset, viewport, indices, viewports, viewport);
        }
    }
}

bool ShadowsPass::AppendViewport(u32 row, u32 column, glm::vec4* viewports, u32& viewport)
{
    viewports[viewport].x = static_cast<f32>(column * LOCAL_SHADOW_MAP_DIMENTIONS);
    viewports[viewport].y = static_cast<f32>(row * LOCAL_SHADOW_MAP_DIMENTIONS);
    viewports[viewport].z = LOCAL_SHADOW_MAP_DIMENTIONS;
    viewports[viewport].w = LOCAL_SHADOW_MAP_DIMENTIONS;

    viewport = (viewport + 1) % MAX_VIEWPORTS_COUNT;

    return viewport == 0;
}

void ShadowsPass::Submit(CommandList* list, u32 modelIndex, u32 shadowOffset, u32 shadowCount, u32 indices, glm::vec4* viewports, u32& viewport)
{
    u32 parameters[3] = { modelIndex, shadowOffset, shadowCount };
    list->SetGraphicsRootConstants(12, 3, 0, parameters);

    list->SetViewports(viewport == 0 ? MAX_VIEWPORTS_COUNT : viewport, viewports, 0.0f, 1.0f);
    list->SetScissors(viewport == 0 ? MAX_VIEWPORTS_COUNT : viewport, viewports);

    list->DrawIndexed(0, indices, 0);

    viewport = 0;
}