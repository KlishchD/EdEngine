#pragma once

#if (DEBUG_BUILD == 1) || (DEVELOPMENT_BUILD == 1)
#   include "RendererStats.h"
#endif

class Window;
class PipelineStateObject;
class CommandList;
struct RenderScene;
class RenderGraph;
class RootSignature;

enum class AAMethod : u8
{
    None,
    TAA,
    FXAA
};

enum class MipMappingMode
{
    Albedo,
    Normal,
    Material
};

struct MipMappingRequest
{
    ResourceView View;
    MipMappingMode Mode;
    u32 Mips;
};

class Renderer
{
public:
    static inline Renderer& Get()
    {
        ED_ASSERT(s_Renderer, "Can querry renderer as it was not yet created.");
        return *s_Renderer;
    }

    void Initialize(Window* window);
    void Deinitialize();

    void PreFrameUpdate();
    void RenderFrame(const RenderScene& scene, f32 deltaSeconds);
    void PostFrameUpdate();

    const RenderScene& GetScene() const { return *m_Scene; }

#if DEBUG_BUILD == 1 || DEVELOPMENT_BUILD == 1
    RenderStats& GetRenderStats() { return m_Stats; }
#endif

    bool IsViewportSizeDirty() const;

    void ResizeViewport(glm::vec2 size);
    glm::u32vec2 GetViewportSize() const;

    void SetSSAOEnabled(bool enabled);
    bool IsSSAOEnabled() const;

    void SetBloomEnabled(bool enabled);
    bool IsBloomEnabled() const;

    void SetUpsampleScale(f32 scale);
    f32 GetUpsampleScale() const;

    void SetAAMethod(AAMethod method);
    AAMethod GetAAMethod() const;

    f32 GetFarPlane() const;
 	
//  void SetCamera(const Camera& camera);
// 	void SetCamera(const glm::mat4& view, const glm::mat4& projection, glm::vec3 viewPosition);

    // TODO: Move these to command queue in the future, it is strange to have them here to be honest
    void DrawFullscreenQuad(CommandList* list);

    ResourceView GetDisplayTarget() const;

    Camera GetCamera() const { return m_Camera; }

    RenderGraph* GetRenderGraph() const { return m_Graph; }

    RootSignature* GetRootSignature() const { return m_RootSignature; }

    void RequestMipMapping(const ResourceView& view, MipMappingMode mode, u32 mips);
    Array<MipMappingRequest>& GetMipMappingRequests() { return m_Requests; }
protected:
    friend class Engine;

    Renderer();
    ~Renderer();

protected:
    static inline Renderer* s_Renderer;

    RenderingContext* m_Context;

    RootSignature* m_RootSignature;

    ResourceView m_QuadVertexView;
    ResourceView m_QuadIndexView;

    const RenderScene* m_Scene;

    RenderGraph* m_Graph;

    Array<MipMappingRequest> m_Requests;

#if DEBUG_BUILD == 1 || DEVELOPMENT_BUILD == 1
    RenderStats m_Stats;
#endif

    bool m_bSSAOEnabled = true;
    bool m_bIsBloomEnabled = false;

    f32 m_FarPlane = 500.0f;

    AAMethod m_AAMethod = AAMethod::TAA;

    f32 m_UpsampleScale = 1.0f;

    bool m_bIsViewportSizeDirty = true;
    glm::ivec2 m_ViewportSize = glm::ivec2(1);
    
    Resource* m_QuadVBO;

    Camera m_Camera;
};