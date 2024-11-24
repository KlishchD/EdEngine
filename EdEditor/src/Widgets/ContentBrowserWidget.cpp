#include "ContentBrowserWidget.h"
#include "Core/Engine.h"
#include "Helpers/FilesHelper.h"
#include "Helpers/RenderingHelper.h"
#include "Editor.h"
#include "Core/Assets/AssetManager.h"
#include "Core/Rendering/Textures/Texture2D.h"
#include <filesystem>
#include <imgui.h>

void ContentBrowserWidget::Initialize()
{
    Widget::Initialize();

    Engine& engine = Engine::Get();
    m_Editor = engine.GetManager<Editor>();
    m_AssetManager = engine.GetManager<AssetManager>();

    m_DirectoryIcon = RenderingHelper::ImportBaseColorTexture("Editor\\icons\\directory.png");
    m_TextureIcon = RenderingHelper::ImportBaseColorTexture("Editor\\icons\\texture.png");
    m_MaterialIcon = RenderingHelper::ImportBaseColorTexture("Editor\\icons\\material.png");
    m_MeshIcon = RenderingHelper::ImportBaseColorTexture("Editor\\icons\\mesh.png");

    m_CurrentFolder = FilesHelper::ContentFolderPath;
}

void ContentBrowserWidget::Tick(float DeltaTime)
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
            DirectoryStructure(FilesHelper::ContentFolderPath);

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
        int32_t columns_count = std::max(1, (int32_t)(ImGui::GetWindowWidth() / buttonSize.x));

        PathButtons();

        if (std::filesystem::exists(m_CurrentFolder) && ImGui::BeginTable("files", columns_count))
        {
            for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(m_CurrentFolder))
            {
                if (entry.is_directory())
                {
                    ImGui::TableNextColumn();

                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 5.0f, 5.0f });
                    if (ImGui::ImageButton(entry.path().string().data(), m_DirectoryIcon->GetID(), buttonSize, { 0.0f, 1.0f }, { 1.0f, 0.0f }))
                    {
                        m_CurrentFolder = entry.path().string();
                    }
                    ImGui::Text(entry.path().filename().string().data());
                    ImGui::PopStyleVar();
                }
                else
                {
                    std::string extension = entry.path().extension().string();

                    if (std::shared_ptr<Texture2D> icon = GetTextureByExtension(extension)) {
                        ImGui::TableNextColumn();

                        if (ImGui::ImageButton(entry.path().string().data(), icon->GetID(), buttonSize, { 0.0f, 1.0f }, { 1.0f, 0.0f }))
                        {
                            std::shared_ptr<Asset> asset = m_AssetManager->GetAsset(entry.path().string());
                            m_Editor->SetSelectedAsset(asset);
                        }
                        ImGui::Text(entry.path().filename().replace_extension().string().data());
                    }
                }
            }
            ImGui::EndTable();
        }
    }

    ImGui::End();
}

void ContentBrowserWidget::DirectoryStructure(const std::string& path)
{
    if (ImGui::TreeNode("resources"))
    {
        if (ImGui::IsItemClicked() || ImGui::IsItemToggledOpen())
        {
            m_CurrentFolder = path;
        }

        DirectoryStructureInternal(std::filesystem::directory_iterator(path));
    
        ImGui::TreePop();
    }
}

void ContentBrowserWidget::DirectoryStructureInternal(const std::filesystem::directory_iterator& directory_iterator)
{
    for (const std::filesystem::directory_entry& entry: directory_iterator)
    {
        if (entry.is_directory())
        {
            bool bHasSubdirectories = false;
            for (const std::filesystem::directory_entry& subentry: std::filesystem::directory_iterator(entry))
            {
                if (subentry.is_directory())
                {
                    bHasSubdirectories = true;
                    break;
                }
            }

            if (ImGui::TreeNodeEx(entry.path().filename().string().data(), bHasSubdirectories ? ImGuiTreeNodeFlags_None : ImGuiTreeNodeFlags_Leaf))
            {
                if (ImGui::IsItemClicked() || ImGui::IsItemToggledOpen())
                {
                    m_CurrentFolder = entry.path().string();
                }
                
                DirectoryStructureInternal(std::filesystem::directory_iterator(entry));
                
                ImGui::TreePop();
            }
        }
    }
}

void ContentBrowserWidget::PathButtons()
{
    if (ImGui::Button("<##back", {30, 30}))
    {
        int32_t lastPosition = m_CurrentFolder.find_last_of(R"(\)");
        if (lastPosition != std::string::npos)
        {
            m_CurrentFolder = m_CurrentFolder.substr(0, lastPosition);
        }
    }
    ImGui::SameLine();
    
    int32_t currentPosition = 2;
    bool bSeenContentFolder = false;
    while (currentPosition != std::string::npos)
    {
        int32_t nextPosition = m_CurrentFolder.find_first_of(R"(\)", currentPosition + 1);
        if (nextPosition == std::string::npos) break;

        std::string substr = m_CurrentFolder.substr(currentPosition + 1, nextPosition - currentPosition - 1);

        if (substr == FilesHelper::ContentFolderName)
        {
            bSeenContentFolder = true;
        }

        if (bSeenContentFolder)
        {
            if (ImGui::Button(substr.data(), { 0, 30 }))
            {
                m_CurrentFolder = m_CurrentFolder.substr(0, nextPosition);
            }
            ImGui::SameLine();
        }
        
        currentPosition = nextPosition;
    }

    ImGui::NewLine();
}

std::shared_ptr<Texture2D> ContentBrowserWidget::GetTextureByExtension(const std::string& extension) const
{
    if (extension == ".edmesh") return m_MeshIcon;
    if (extension == ".edtexture") return m_TextureIcon;
    if (extension == ".edmaterial") return m_MaterialIcon;
    return nullptr;
}
