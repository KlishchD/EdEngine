#pragma once

struct StaticMeshElement
{
#if DEBUG_BUILD == 1 || DEVELOPMENT_BUILD == 1
    std::string Name;
#endif

    Transform* WorldTransform;
    Transform* PreviousWorldTransform;
    ResourceView VertexBufferView;
    ResourceView IndexBufferView;

    ResourceView AlbedoTextureView;
    ResourceView NormalTextureView;
    ResourceView RoughnessTextureView;
    ResourceView MetalicTextureView;

    glm::vec3 BaseColor;
    f32 Metalic;
    f32 Roughness;
    f32 Emission;

    void PackRootParameters(u32* parameters) const;
};

struct DirectionalLightElement
{
#if DEBUG_BUILD == 1 || DEVELOPMENT_BUILD == 1
    ccstr8 Name;
#endif

    glm::vec3 Direction;

    glm::vec3 Color;
    f32 Intensity;
    bool IsShadowCasting;

    u32 CascadesCount;
    f32 ShadowMapZMultiplier;

    u32 ShadowFilterSize;
    f32 ShadowFilterRadius;
};

struct SpotLightElement
{
#if DEBUG_BUILD == 1 || DEVELOPMENT_BUILD == 1
    ccstr8 Name;
#endif
    Transform* WorldTransform;

    glm::vec3 Color;
    f32 Intensity;

    f32 NearPlane;
    f32 FarPlane;

    f32 InnerAngle;
    f32 OuterAngle;
    f32 MaxDistance;
    u32 ShadowFilterSize;
    f32 ShadowFilterRadius;

    bool IsShadowCasting;
    bool DrawWireframe;

    glm::mat4 CalculateModelTransform() const
    {
        const f32 angle = OuterAngle;
        const f32 length = MaxDistance;
        const f32 radius = glm::tan(angle) * length;
        glm::vec3 scale(radius, radius, length);

        return glm::scale(WorldTransform->GetMatrixWithOutScale(), scale);
    }

    glm::mat4 CalculateShadowTransform() const 
    {
        glm::vec3 position = WorldTransform->GetTranslation();
        glm::mat4 view = glm::lookAt(position, position + WorldTransform->GetForward(), glm::up);
        glm::mat4 projection = glm::perspective(OuterAngle * 2.0f, 1.0f, FarPlane, NearPlane); //, MaxDistance
        return projection * view;
    }
};

struct PointLightElement
{
#if DEBUG_BUILD == 1 || DEVELOPMENT_BUILD == 1
    ccstr8 Name;
#endif

    glm::vec3 Position;

    glm::vec3 Color;
    f32 Intensity;

    f32 NearPlane;
    f32 FarPlane;

    f32 Radius;
    u32 ShadowFilterSize;
    f32 ShadowFilterRadius;

    bool IsShadowCasting;
    bool DrawWireframe;
};

struct IconElement
{
#if DEBUG_BUILD == 1 || DEVELOPMENT_BUILD == 1
    ccstr8 Name;
#endif

    ResourceView IconView;

    glm::vec3 Location;
    f32 UniformScale;

    glm::vec3 Tint;
};

struct CameraElement
{
#if DEBUG_BUILD == 1 || DEVELOPMENT_BUILD == 1
    ccstr8 Name;
#endif

    glm::vec3 Position;
    glm::vec3 Rotation;
    f32 Fov;
    f32 Near;
    f32 Far;
    f32 Aspect;
};

struct RenderScene
{
#if DEBUG_BUILD == 1 || DEVELOPMENT_BUILD == 1
    std::string Name;
#endif

    CameraElement Camera;
    
    Array<StaticMeshElement> StaticMeshes;
    Array<DirectionalLightElement> DirectionalLights;
    Array<SpotLightElement> SpotLights;
    Array<PointLightElement> PointLights;
    Array<IconElement> IconElements;
};