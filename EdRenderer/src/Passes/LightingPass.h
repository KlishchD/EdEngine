#pragma once

class RenderGraph;
class RenderPass;

class LightingPass : public RenderPass
{
    struct PointLightParameters
    {
        glm::vec3 Position;
        float Radius;

        glm::vec3 Color;
        float Intensity;

        glm::i32vec2 ShadowMapStart;
        glm::u32vec2 ShadowMapSize;

        glm::vec3 ViewPosition;

        float ShadowViewDepthConversion0;
        float ShadowViewDepthConversion1;
        float ShadowFilterRadius;
        i32 ShadowFilterSize;
    };

    struct SpotLightParameters
    {
        glm::mat4 WorldTransform;
        glm::mat4 ShadowProjectionViewTransform;

        glm::vec3 Position;
        float InnerAngle;

        glm::vec3 Forward;
        float OuterAngle;

        glm::vec3 Color;
        float MaxDistance;

        glm::i32vec2 ShadowMapStart;
        glm::u32vec2 ShadowMapSize;

        glm::vec3 ViewPosition;

        float Intensity;

        float ShadowFilterRadius;
        i32 ShadowFilterSize;
    };

public:
    LightingPass();

    virtual void Initialize(RenderGraph* graph) override;

    virtual void GetWaitSignals(ccstr8** names, u32& count) override;

    virtual u32 GetCustomShaderParametersStructSize() override;
    virtual void GatherCustomShaderParameters(void* memory) override;

    virtual void Execute(CommandList* list, Resource* buffer, u64 offset) override;
protected:
    RenderTarget* m_Lighting;
    RenderTarget* m_Albedo;
    RenderTarget* m_Material;
    RenderTarget* m_Depth;
    RenderTarget* m_ShadowMaps;
    UAVTarget* m_SSAO;

    ResourceView m_ShadowNoise;

    ResourceView m_PointLightMeshVertexBufferView;
    ResourceView m_PointLightMeshIndexBufferView;

    ResourceView m_SpotLightMeshVertexBufferView;
    ResourceView m_SpotLightMeshIndexBufferView;

    PipelineStateObject* m_BaseLightingPSO;
    PipelineStateObject* m_LightsPSO[2];
};