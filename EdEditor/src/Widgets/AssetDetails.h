#pragma once

#include "EdEditor.h"

class AssetDetailsWidget : public Widget
{
public:
    virtual void Tick(f32 DeltaTime) override;
private:
    Editor* m_Editor;
    AssetManager* m_AssetManager;

    void BaseDetails(Asset* asset);
    void StaticMeshDetails(StaticMeshAsset* asset);
    void TextureDetails(TextureAsset* asset);
    void MaterialDetails(MaterialAsset* asset);
    void PrefabDetails(PrefabAsset* asset);

    void SelectTexture(ccstr8 lable, TextureAsset*& asset);
};
