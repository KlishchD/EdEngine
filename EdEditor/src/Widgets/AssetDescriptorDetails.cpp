#include "AssetDescriptorDetails.h"
#include "Editor.h"
#include "Core/Engine.h"
#include "Core/Assets/Asset.h"
#include "Core/Assets/AssetManager.h"
#include "Core/Rendering/Textures/Texture2D.h"
#include "Core/Rendering/Textures/CubeTexture.h"
#include "Core/Assets/Material.h"
#include "Core/Assets/Descriptors/MaterialDescriptor.h"
#include "Core/Assets/StaticMesh.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

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
        case AssetType::CubeTexture: CubeTextureDescriptorDetails(std::static_pointer_cast<CubeTextureDescriptor>(selectedDescriptor)); break;
        case AssetType::Material: MaterialDescriptorDetails(std::static_pointer_cast<MaterialDescriptor>(selectedDescriptor)); break;
        }
    
        ImGui::End();
    }
}

void AssetDescriptorDetails::BaseDescriptorDetails(std::shared_ptr<AssetDescriptor> descriptor)
{
    ImGui::Text("Descriptor %s", descriptor->AssetName.data());

}

void AssetDescriptorDetails::StaticMeshDescriptorDetails(std::shared_ptr<StaticMeshDescriptor> descriptor)
{
    BaseDescriptorDetails(descriptor);
}

void AssetDescriptorDetails::Texture2DDescriptorDetails(std::shared_ptr<Texture2DDescriptor> descriptor)
{
    BaseDescriptorDetails(descriptor);
}

void AssetDescriptorDetails::CubeTextureDescriptorDetails(std::shared_ptr<CubeTextureDescriptor> descriptor)
{
    BaseDescriptorDetails(descriptor);
}

void AssetDescriptorDetails::MaterialDescriptorDetails(std::shared_ptr<MaterialDescriptor> materialDescriptor)
{
    BaseDescriptorDetails(materialDescriptor);

    std::shared_ptr<Material> material = m_AssetManager->LoadMaterial(materialDescriptor);

    if (glm::vec3 color = material->GetBaseColor(); ImGui::ColorPicker3("Base color", glm::value_ptr(color)))
    {
        material->SetBaseColor(color);
    }

    if (float emission = material->GetEmission(); ImGui::SliderFloat("Emission", &emission, 0, 100))
    {
        material->SetEmission(emission);
    }

    const std::vector<std::shared_ptr<Texture2DDescriptor>>& descriptors = m_AssetManager->GetDescriptors<Texture2DDescriptor>(AssetType::Texture2D);

    ImGui::Text("Base Color Texture"); ImGui::SameLine();
    if (ImGui::BeginCombo("##BaseColorTexture", AssetUtils::GetSelectTextureLable(material->GetBaseColorTexture()).c_str()))
    {
        if (ImGui::Selectable("None", material->GetBaseColorTexture() == nullptr))
        {
            material->SetBaseColorTexture(nullptr);
        }
        for (const auto& descriptor : descriptors)
        {
            if (ImGui::Selectable(AssetUtils::GetAssetNameLable(descriptor).c_str(), materialDescriptor->BaseColorTextureID == descriptor->AssetId))
            {
                material->SetBaseColorTexture(m_AssetManager->LoadTexture(descriptor));
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Text("Normal Texture"); ImGui::SameLine();
    if (ImGui::BeginCombo("##NormalTexture", AssetUtils::GetSelectTextureLable(material->GetNormalTexture()).c_str()))
    {
        if (ImGui::Selectable("None", material->GetNormalTexture() == nullptr))
        {
            material->SetNormalTexture(nullptr);
        }
        for (const auto& descriptor : descriptors)
        {
            if (ImGui::Selectable(AssetUtils::GetAssetNameLable(descriptor).c_str(), materialDescriptor->NormalTextureID == descriptor->AssetId))
            {
				material->SetNormalTexture(m_AssetManager->LoadTexture(descriptor));
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Text("Roughness Texture"); ImGui::SameLine();
    if (ImGui::BeginCombo("##RoughnessTexture", AssetUtils::GetSelectTextureLable(material->GetRoughnessTexture()).c_str()))
    {
        if (ImGui::Selectable("None", material->GetRoughnessTexture() == nullptr))
        {
            material->SetRoughnessTexture(nullptr);
        }
        for (const auto& descriptor : descriptors)
        {
            if (ImGui::Selectable(AssetUtils::GetAssetNameLable(descriptor).c_str(), materialDescriptor->RoughnessTextureID == descriptor->AssetId))
            {
                material->SetRoughnessTexture(m_AssetManager->LoadTexture(descriptor));
            }
        }
        ImGui::EndCombo();
    }

    if (float roughness = material->GetRoughness(); ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f))
    {
        material->SetRoughness(roughness);
    }

    ImGui::Text("Metalic Texture"); ImGui::SameLine();
    if (ImGui::BeginCombo("##MetalicTexture", AssetUtils::GetSelectTextureLable(material->GetMetalicTexture()).c_str()))
    {
        if (ImGui::Selectable("None", material->GetMetalicTexture() == nullptr))
        {
            material->SetMetalicTexture(nullptr);
        }
        for (const auto& descriptor : descriptors)
        {
            if (ImGui::Selectable(AssetUtils::GetAssetNameLable(descriptor).c_str(), materialDescriptor->MetalicTextureID == descriptor->AssetId))
            {
                material->SetMetalicTexture(m_AssetManager->LoadTexture(descriptor));
            }
        }
        ImGui::EndCombo();
    }

    if (float metalic = material->GetMetalic(); ImGui::SliderFloat("Metalic", &metalic, 0.0f, 1.0f))
    {
        material->SetMetalic(metalic);
    }
}