#pragma once

#include <memory>
#include <string>
#include "Core/Widget.h"

struct AssetDescriptor;
struct StaticMeshDescriptor;
struct Texture2DDescriptor;
struct CubeTextureDescriptor;
struct MaterialDescriptor;

class AssetDescriptorDetails: public Widget
{
public:
    virtual void Initialize() override;
    virtual void Tick(float DeltaTime) override;
private:
    std::shared_ptr<class Editor> m_Editor;
    std::shared_ptr<class AssetManager> m_AssetManager;

    void BaseDescriptorDetails(std::shared_ptr<AssetDescriptor> descriptor);
    void StaticMeshDescriptorDetails(std::shared_ptr<StaticMeshDescriptor> descriptor);
    void Texture2DDescriptorDetails(std::shared_ptr<Texture2DDescriptor> descriptor);
    void CubeTextureDescriptorDetails(std::shared_ptr<CubeTextureDescriptor> descriptor);
    void MaterialDescriptorDetails(std::shared_ptr<MaterialDescriptor> descriptor);
};
