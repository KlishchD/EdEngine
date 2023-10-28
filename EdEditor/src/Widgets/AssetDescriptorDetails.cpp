#include "AssetDescriptorDetails.h"
#include "Editor.h"
#include "Core/Engine.h"
#include "Core/Assets/Asset.h"
#include "Core/Assets/AssetManager.h"
#include "Core/Assets/Texture2D.h"
#include "Core/Assets/Texture3D.h"
#include "Core/Assets/Material.h"
#include "Core/Assets/StaticMesh.h"

#include <imgui.h>

void AssetDescriptorDetails::Initialize()
{
    Widget::Initialize();

    Engine& engine = Engine::Get();
    m_Editor = engine.GetManager<Editor>();
    m_AssetManager = engine.GetManager<AssetManager>();
}

void AssetDescriptorDetails::Tick(float DeltaTime)
{
    Widget::Tick(DeltaTime);

    if (std::shared_ptr<AssetDescriptor> selectedDescriptor = m_Editor->GetSelectedAssetDescriptor())
    {
        ImGui::Begin("Asset Details");
        
        switch (selectedDescriptor->AssetType)
        {
        case AssetType::StaticMesh: StaticMeshDescriptorDetails(std::static_pointer_cast<StaticMeshDescriptor>(selectedDescriptor)); break;
        case AssetType::Texture2D: Texture2DDescriptorDetails(std::static_pointer_cast<Texture2DDescriptor>(selectedDescriptor)); break;
        case AssetType::Texture3D: Texture3DDescriptorDetails(std::static_pointer_cast<Texture3DDescriptor>(selectedDescriptor)); break;
        case AssetType::Material: MaterialDescriptorDetails(std::static_pointer_cast<MaterialDescriptor>(selectedDescriptor)); break;
        }
    
        ImGui::End();
    }
}

void AssetDescriptorDetails::BaseDescriptorDetails(const std::shared_ptr<AssetDescriptor>& descriptor)
{
    ImGui::Text("Descriptor %s", descriptor->AssetName.data());

}

void AssetDescriptorDetails::StaticMeshDescriptorDetails(const std::shared_ptr<StaticMeshDescriptor>& descriptor)
{
    BaseDescriptorDetails(descriptor);
}

void AssetDescriptorDetails::Texture2DDescriptorDetails(const std::shared_ptr<Texture2DDescriptor>& descriptor)
{
    BaseDescriptorDetails(descriptor);
}

void AssetDescriptorDetails::Texture3DDescriptorDetails(const std::shared_ptr<Texture3DDescriptor>& descriptor)
{
    BaseDescriptorDetails(descriptor);
}

void AssetDescriptorDetails::MaterialDescriptorDetails(const std::shared_ptr<MaterialDescriptor>& descriptor)
{
    BaseDescriptorDetails(descriptor);

    {
        std::shared_ptr<AssetDescriptor> baseColorDescriptor = m_AssetManager->GetAssetDescriptor(descriptor->BaseColorTextureID); // TODO: make it better :)
        std::string label = baseColorDescriptor ? baseColorDescriptor->AssetName : "Select Base Color Texture";
        
        ImGui::Text("Base Color Texture"); ImGui::SameLine();
        if (ImGui::BeginCombo("##BaseColorTexture", label.data()))
        {
            if (ImGui::Selectable("None", descriptor->BaseColorTextureID == -1))
            {
                descriptor->BaseColorTextureID = -1;
                m_AssetManager->RefreshMaterial(descriptor);
            }
            for (const auto& textureDescriptor : m_AssetManager->GetTexture2DDescriptors())
            {
                if (ImGui::Selectable(textureDescriptor->AssetName.data(), descriptor->BaseColorTextureID == textureDescriptor->AssetId))
                {
                    descriptor->BaseColorTextureID = textureDescriptor->AssetId;
                    m_AssetManager->RefreshMaterial(descriptor);
                }
            }
            ImGui::EndCombo();
        }
    }

    {
        std::shared_ptr<AssetDescriptor> normalDescriptor = m_AssetManager->GetAssetDescriptor(descriptor->NormalTextureID); // TODO: make it better :)
        std::string label = normalDescriptor ? normalDescriptor->AssetName : "Select Normal Texture";
        
        ImGui::Text("Normal Texture"); ImGui::SameLine();
        if (ImGui::BeginCombo("##NormalTexture", label.data()))
        {
            if (ImGui::Selectable("None", descriptor->NormalTextureID == -1))
            {
                descriptor->NormalTextureID = -1;
                m_AssetManager->RefreshMaterial(descriptor);
            }
            for (const auto& textureDescriptor : m_AssetManager->GetTexture2DDescriptors())
            {
                if (ImGui::Selectable(textureDescriptor->AssetName.data(), descriptor->NormalTextureID == textureDescriptor->AssetId))
                {
                    descriptor->NormalTextureID = textureDescriptor->AssetId;
                    m_AssetManager->RefreshMaterial(descriptor);
                }
            }
            ImGui::EndCombo();
        }
    }

    {
        std::shared_ptr<AssetDescriptor> roughnessDescriptor = m_AssetManager->GetAssetDescriptor(descriptor->RoughnessTextureID); // TODO: make it better :)
        std::string label = roughnessDescriptor ? roughnessDescriptor->AssetName : "Select Roughness Texture";

        ImGui::Text("Roughness Texture"); ImGui::SameLine();
        if (ImGui::BeginCombo("##RoughnessTexture", label.data()))
        {
            if (ImGui::Selectable("None", descriptor->RoughnessTextureID == -1))
            {
                descriptor->RoughnessTextureID = -1;
                m_AssetManager->RefreshMaterial(descriptor);
            }
            for (const auto& textureDescriptor : m_AssetManager->GetTexture2DDescriptors())
            {
                if (ImGui::Selectable(textureDescriptor->AssetName.data(), descriptor->RoughnessTextureID == textureDescriptor->AssetId))
                {
                    descriptor->RoughnessTextureID = textureDescriptor->AssetId;
                    m_AssetManager->RefreshMaterial(descriptor);
                }
            }
            ImGui::EndCombo();
        }
    }

    {
        std::shared_ptr<AssetDescriptor> metalicDescriptor = m_AssetManager->GetAssetDescriptor(descriptor->MetalicTextureID); // TODO: make it better :)
        std::string label = metalicDescriptor ? metalicDescriptor->AssetName : "Select Metalic Texture";
    
        ImGui::Text("Metalic Texture"); ImGui::SameLine();
        if (ImGui::BeginCombo("##MetalicTexture", label.data()))
        {
            if (ImGui::Selectable("None", descriptor->MetalicTextureID == -1))
            {
                descriptor->MetalicTextureID = -1;
                m_AssetManager->RefreshMaterial(descriptor);
            }
            for (const auto& textureDescriptor : m_AssetManager->GetTexture2DDescriptors())
            {
                if (ImGui::Selectable(textureDescriptor->AssetName.data(), descriptor->MetalicTextureID == textureDescriptor->AssetId))
                {
                    descriptor->MetalicTextureID = textureDescriptor->AssetId;
                    m_AssetManager->RefreshMaterial(descriptor);
                }
            }
            ImGui::EndCombo();
        }
    }
}
