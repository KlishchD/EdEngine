#pragma once

#include <memory>
#include <string>
#include "Core/Widget.h"

class Asset;
class StaticMesh;
class Texture2D;
class CubeTexture;
class Material;

class AssetDetails: public Widget
{
public:
    virtual void Initialize() override;
    virtual void Tick(float DeltaTime) override;
private:
    std::shared_ptr<class Editor> m_Editor;
    std::shared_ptr<class AssetManager> m_AssetManager;

    void BaseDetails(std::shared_ptr<Asset> asset);
    void StaticMeshDetails(std::shared_ptr<StaticMesh> asset);
    void Texture2DDetails(std::shared_ptr<Texture2D> asset);
    void CubeTextureDetails(std::shared_ptr<CubeTexture> asset);
    void MaterialDetails(std::shared_ptr<Material> material);
};
