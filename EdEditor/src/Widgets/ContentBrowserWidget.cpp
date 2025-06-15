#include "EdEditor.h"
#include "ContentBrowserWidget.h"
#include "Helpers/FilesHelper.h"
#include "Helpers/RenderingHelper.h"
#include "Helpers/AssetHelper.h"

void ContentBrowserWidget::Initialize()
{
    Widget::Initialize();

    m_DirectoryIcon = AssetHelper::ImportOrLoadTexture(ContentPath("Editor\\icons\\directory.png"), "Editor\\icons\\", TextureAsset::Albedo, "DirectoryIcon", PixelFormat::RGBA8F, 5);
    m_TextureIcon = AssetHelper::ImportOrLoadTexture(ContentPath("Editor\\icons\\texture.png"), "Editor\\icons\\", TextureAsset::Albedo, "TextureIcon", PixelFormat::RGBA8F, 5);
    m_MaterialIcon = AssetHelper::ImportOrLoadTexture(ContentPath("Editor\\icons\\material.png"), "Editor\\icons\\", TextureAsset::Albedo, "MaterialIcon", PixelFormat::RGBA8F, 5);
    m_MeshIcon = AssetHelper::ImportOrLoadTexture(ContentPath("Editor\\icons\\mesh.png"), "Editor\\icons\\", TextureAsset::Albedo, "MeshIcon", PixelFormat::RGBA8F, 5);
    m_SceneIcon = AssetHelper::ImportOrLoadTexture(ContentPath("Editor\\icons\\scene.png"), "Editor\\icons\\", TextureAsset::Albedo, "SceneIcon", PixelFormat::RGBA8F, 5);
    m_PrefabIcon = AssetHelper::ImportOrLoadTexture(ContentPath("Editor\\icons\\prefab.png"), "Editor\\icons\\", TextureAsset::Albedo, "PrefabIcon", PixelFormat::RGBA8F, 5);
}

void ContentBrowserWidget::Tick(f32 DeltaTime)
{
    Widget::Tick(DeltaTime);

    ContentTree();
    ContentItems();
}

void ContentBrowserWidget::ContentTree()
{
    if (ImGui::Begin("Content"))
    {
        if (ImGui::BeginChild("Folders"))
        {
            DirectoryStructure(Files::GetContentPath());

            ImGui::EndChild();
        }
    }

    ImGui::End();
}

void ContentBrowserWidget::ContentItems()
{
    if (ImGui::Begin("Folder"))
    {
        ImVec2 buttonSize = { 155.0f, 150.0f };
        i32 columns_count = std::max(1, (i32)(ImGui::GetWindowWidth() / buttonSize.x));

        PathButtons();

        if (m_CurrentFolder.IsValid() && ImGui::BeginTable("files", columns_count))
        {
            TemporaryArray<Path> paths = m_CurrentFolder.GetAllPathsTemporary(false);
            for (const Path& path : paths)
            {
                if (path.IsDirectory())
                {
                    ImGui::TableNextColumn();

                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 5.0f, 5.0f });
                    if (ImGui::ImageButton(path.Get(), m_DirectoryIcon->TextureView.GPUHandlePtr(), buttonSize))
                    {
                        m_CurrentFolder = path.Content();
                    }
                    ImGui::Text(path.GetFileName());
                    ImGui::PopStyleVar();
                }
                else
                {
                    if (TextureAsset* icon = GetTextureByExtension(path.GetExtension())) {
                        ImGui::TableNextColumn();

                        Asset* asset = AssetManager::Get().GetAsset(path.Content());
                        if (asset && asset->AssetType == TextureAsset::Type && asset->HasData)
                        {
                            icon = static_cast<TextureAsset*>(asset->Data);
                        }

                        if (ImGui::ImageButton(path.Get(), icon->TextureView.GPUHandlePtr(), buttonSize))
                        {
                            Asset* asset = AssetManager::Get().GetAsset(path.Content());
                            Editor::Get().SetSelectedAsset(asset);
                        }
                        ImGui::Text(path.GetFileName());
                    }
                }
            }
            ImGui::EndTable();
        }
    }

    ImGui::End();
}

void ContentBrowserWidget::DirectoryStructure(const Path& path)
{
    if (ImGui::TreeNode("resources"))
    {
        if (ImGui::IsItemClicked() || ImGui::IsItemToggledOpen())
        {
            m_CurrentFolder = path.Content();
        }

        DirectoryStructureInternal(path.GetAllPathsTemporary(false));
    
        ImGui::TreePop();
    }
}

void ContentBrowserWidget::DirectoryStructureInternal(const TemporaryArray<Path>& paths)
{
    for (const Path& path: paths)
    {
        if (path.IsDirectory())
        {
            bool bHasSubdirectories = path.HasSubdirectories();
            if (ImGui::TreeNodeEx(path.GetFileName(), bHasSubdirectories ? ImGuiTreeNodeFlags_None : ImGuiTreeNodeFlags_Leaf))
            {
                if (ImGui::IsItemClicked() || ImGui::IsItemToggledOpen())
                {
                    m_CurrentFolder = path.Content();
                }
                
                DirectoryStructureInternal(path.GetAllPathsTemporary(false));
                
                ImGui::TreePop();
            }
        }
    }
}

void ContentBrowserWidget::PathButtons()
{
    if (ImGui::Button("<##back", {30, 30}) && m_CurrentFolder != Files::GetContentPath())
    {
        m_CurrentFolder.Pop();
    }
    ImGui::SameLine();

    TemporaryArray<Pair<ccstr8, u32>> folders = m_CurrentFolder.GetAllFoldersTemporary();
    
    u32 popCount = 0;

    bool bSeenContentFolder = false;
    for (u32 index = 0; index < folders.GetSize(); ++index)
    {
        const Pair<ccstr8, u32>& folder = folders[index];

        if (!bSeenContentFolder && strncmp(Files::ContentFolderName, folder.First, folder.Second) == 0)
        {
            bSeenContentFolder = true;
        }

        if (bSeenContentFolder)
        {
            ImGui::PushID(static_cast<i32>(reinterpret_cast<iptr>(folder.First)));

            std::string_view view(folder.First, folder.Second);
            if (ImGui::Button(std::string(view).c_str(), {0, 30}))
            {
                popCount = folders.GetSize() - index - 1;
            }

            ImGui::PopID();

            ImGui::SameLine();
        }
    }

    while (popCount > 0)
    {
        m_CurrentFolder.Pop();
        --popCount;
    }

    ImGui::NewLine();
}

TextureAsset* ContentBrowserWidget::GetTextureByExtension(const std::string& extension) const
{
    if (extension == Files::FullMeshAssetExtension) return m_MeshIcon;
    if (extension == Files::FullTextureAssetExtension) return m_TextureIcon;
    if (extension == Files::FullMaterialAssetExtension) return m_MaterialIcon;
    if (extension == Files::FullSceneAssetExtension) return m_SceneIcon;
    if (extension == Files::FullPrefabAssetExtension) return m_PrefabIcon;
    return nullptr;
}
