#pragma once

#include <filesystem>
#include <string>
#include "Core/Widget.h"

class Texture2D;

class ContentBrowserWidget: public Widget
{
public:
    virtual void Initialize() override;
    virtual void Tick(float DeltaTime) override;
private:
    std::shared_ptr<class Editor> m_Editor;
    std::shared_ptr<class AssetManager> m_AssetManager;
    
    std::string m_CurrentFolder;

    std::shared_ptr<Texture2D> m_DirectoryIcon;
    std::shared_ptr<Texture2D> m_TextureIcon;
    std::shared_ptr<Texture2D> m_MaterialIcon;
    std::shared_ptr<Texture2D> m_MeshIcon;

    void ContentTree();
    void ContentItems();
    
    void DirectoryStructure(const std::string& path);
    void DirectoryStructureInternal(const std::filesystem::directory_iterator& directory_iterator);

    void PathButtons();

    std::shared_ptr<Texture2D> GetTextureByExtension(const std::string& extension) const;
};
