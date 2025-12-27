#pragma once

class IconPass : public RenderPass
{
    struct IconParameters
    {
        glm::vec3 Tint;
        u32 IconIndex;

        glm::vec3 Location;
        f32 UniformScale;
    };
public:
    IconPass();

    virtual u32 GetCustomShaderParametersStructSize() override;
    virtual void GatherCustomShaderParameters(void* memory) override;

    virtual void Initialize(RenderGraph* graph) override;
    virtual void Execute(CommandList* list, Resource* buffer, u64 offset) override;

protected:
    PipelineStateObject* m_PSO;
};