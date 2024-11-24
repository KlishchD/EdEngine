#include "AssetDetails.h"
#include "Editor.h"
#include "Core/Engine.h"
#include "Core/Assets/Asset.h"
#include "Core/Assets/AssetManager.h"
#include "Core/Rendering/Textures/Texture2D.h"
#include "Core/Rendering/Textures/CubeTexture.h"
#include "Core/Assets/Material.h"
#include "Core/Assets/StaticMesh.h"
#include "Helpers/AssetHelper.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

void AssetDetails::Initialize()
{
    Widget::Initialize();

    Engine& engine = Engine::Get();
    m_Editor = engine.GetManager<Editor>();
    m_AssetManager = engine.GetManager<AssetManager>();
}

void AssetDetails::Tick(float DeltaTime)
{
    Widget::Tick(DeltaTime);

    if (std::shared_ptr<Asset> asset = m_Editor->GetSelectedAsset())
    {
        m_AssetManager->LoadAsset(asset->GetId());

        if (ImGui::Begin("Asset Details"))
        {
            switch (asset->GetType())
            {
            case AssetType::StaticMesh: StaticMeshDetails(std::static_pointer_cast<StaticMesh>(asset)); break;
            case AssetType::Texture2D: Texture2DDetails(std::static_pointer_cast<Texture2D>(asset)); break;
            case AssetType::CubeTexture: CubeTextureDetails(std::static_pointer_cast<CubeTexture>(asset)); break;
            case AssetType::Material: MaterialDetails(std::static_pointer_cast<Material>(asset)); break;
            }
        }

        ImGui::End();
    }
}

void AssetDetails::BaseDetails(std::shared_ptr<Asset> asset)
{
    ImGui::Text("Asset %s", asset->GetName().data());
}

void AssetDetails::StaticMeshDetails(std::shared_ptr<StaticMesh> asset)
{
    BaseDetails(asset);
}

void AssetDetails::Texture2DDetails(std::shared_ptr<Texture2D> asset)
{
    BaseDetails(asset);
}

void AssetDetails::CubeTextureDetails(std::shared_ptr<CubeTexture> asset)
{
    BaseDetails(asset);
}

void AssetDetails::MaterialDetails(std::shared_ptr<Material> material)
{
    BaseDetails(material);

    if (glm::vec3 color = material->GetBaseColor(); ImGui::ColorPicker3("Base color", glm::value_ptr(color)))
    {
        material->SetBaseColor(color);
    }

    if (float emission = material->GetEmission(); ImGui::SliderFloat("Emission", &emission, 0, 100))
    {
        material->SetEmission(emission);
    }

    std::vector<std::shared_ptr<Texture2D>> textures = m_AssetManager->GetAssets<Texture2D>(AssetType::Texture2D);

    ImGui::Text("Base Color Texture"); ImGui::SameLine();
    if (ImGui::BeginCombo("##BaseColorTexture", AssetHelper::GetAssetNameLable(material->GetBaseColorTexture()).c_str()))
    {
        if (ImGui::Selectable("None", material->GetBaseColorTexture() == nullptr))
        {
            material->SetBaseColorTexture(nullptr);
        }
        for (const auto& texture : textures)
        {
            if (ImGui::Selectable(AssetHelper::GetAssetNameLable(texture).c_str(), material->GetBaseColorTexture() == texture))
            {
                m_AssetManager->LoadAsset(texture->GetId());
                material->SetBaseColorTexture(texture);
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Text("Normal Texture"); ImGui::SameLine();
    if (ImGui::BeginCombo("##NormalTexture", AssetHelper::GetAssetNameLable(material->GetNormalTexture()).c_str()))
    {
        if (ImGui::Selectable("None", material->GetNormalTexture() == nullptr))
        {
            material->SetNormalTexture(nullptr);
        }
        for (const auto& texture : textures)
        {
            if (ImGui::Selectable(AssetHelper::GetAssetNameLable(texture).c_str(), material->GetNormalTexture() == texture))
            {
                m_AssetManager->LoadAsset(texture->GetId());
                material->SetNormalTexture(texture);
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Text("Roughness Texture"); ImGui::SameLine();
    if (ImGui::BeginCombo("##RoughnessTexture", AssetHelper::GetAssetNameLable(material->GetRoughnessTexture()).c_str()))
    {
        if (ImGui::Selectable("None", material->GetRoughnessTexture() == nullptr))
        {
            material->SetRoughnessTexture(nullptr);
        }
        for (const auto& texture : textures)
        {
            if (ImGui::Selectable(AssetHelper::GetAssetNameLable(texture).c_str(), material->GetRoughnessTexture() == texture))
            {
                m_AssetManager->LoadAsset(texture->GetId());
                material->SetRoughnessTexture(texture);
            }
        }
        ImGui::EndCombo();
    }

    if (float roughness = material->GetRoughness(); ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f))
    {
        material->SetRoughness(roughness);
    }

    ImGui::Text("Metalic Texture"); ImGui::SameLine();
    if (ImGui::BeginCombo("##MetalicTexture", AssetHelper::GetAssetNameLable(material->GetMetalicTexture()).c_str()))
    {
        if (ImGui::Selectable("None", material->GetMetalicTexture() == nullptr))
        {
            material->SetMetalicTexture(nullptr);
        }
        for (const auto& texture : textures)
        {
            if (ImGui::Selectable(AssetHelper::GetAssetNameLable(texture).c_str(), material->GetMetalicTexture() == texture))
            {
                m_AssetManager->LoadAsset(texture->GetId());
                material->SetMetalicTexture(texture);
            }
        }
        ImGui::EndCombo();
    }

    if (float metalic = material->GetMetalic(); ImGui::SliderFloat("Metalic", &metalic, 0.0f, 1.0f))
    {
        material->SetMetalic(metalic);
    }
}