#pragma once

#include <memory>
#include "Core/Widget.h"

class AssetDescriptorDetails: public Widget
{
public:
    virtual void Initialize() override;
    virtual void Tick(float DeltaTime) override;
private:
    std::shared_ptr<class Editor> m_Editor;
    std::shared_ptr<class AssetManager> m_AssetManager;

    void BaseDescriptorDetails(const std::shared_ptr<struct AssetDescriptor>& descriptor);
    void StaticMeshDescriptorDetails(const std::shared_ptr<struct StaticMeshDescriptor>& descriptor);
    void Texture2DDescriptorDetails(const std::shared_ptr<struct Texture2DDescriptor>& descriptor);
    void Texture3DDescriptorDetails(const std::shared_ptr<struct Texture3DDescriptor>& descriptor);
    void MaterialDescriptorDetails(const std::shared_ptr<struct MaterialDescriptor>& descriptor);
};
