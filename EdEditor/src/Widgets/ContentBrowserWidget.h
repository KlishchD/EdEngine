#pragma once

#include "EdEditor.h"

class Editor;
class AssetManager;
struct TextureAsset;

class ContentBrowserWidget: public Widget
{
public:
    virtual void Initialize() override;
    virtual void Tick(f32 DeltaTime) override;
private:
    ContentPath m_CurrentFolder;

    TextureAsset* m_DirectoryIcon;
    TextureAsset* m_TextureIcon;
    TextureAsset* m_MaterialIcon;
    TextureAsset* m_MeshIcon;
    TextureAsset* m_SceneIcon;
    TextureAsset* m_PrefabIcon;

    void ContentTree();
    void ContentItems();
    
    void DirectoryStructure(const Path& path);
    void DirectoryStructureInternal(const TemporaryArray<Path>& paths);

    void PathButtons();

    TextureAsset* GetTextureByExtension(const std::string& extension) const;
};
