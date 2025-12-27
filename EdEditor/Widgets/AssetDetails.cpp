#include "EdEditor.h"
#include "AssetDetails.h"

void AssetDetailsWidget::Tick(f32 DeltaTime)
{
    Widget::Tick(DeltaTime);

    if (ImGui::Begin("Asset Details"))
    {
        if (Asset* asset = Editor::Get().GetSelectedAsset())
        {
            BaseDetails(asset);

            switch (asset->AssetType)
            {
            case StaticMeshAsset::Type: StaticMeshDetails(static_cast<StaticMeshAsset*>(asset->Data)); break;
            case TextureAsset::Type: TextureDetails(static_cast<TextureAsset*>(asset->Data)); break;
            case MaterialAsset::Type: MaterialDetails(static_cast<MaterialAsset*>(asset->Data)); break;
            case PrefabAsset::Type: PrefabDetails(static_cast<PrefabAsset*>(asset->Data)); break;
            }
        }
    }
    ImGui::End();
}

void AssetDetailsWidget::BaseDetails(Asset* asset)
{
    ImGui::Text("Name: %s", asset->Name.c_str());
    ImGui::Text("Path: %s", asset->FilePath.Get());
    ImGui::Text("Origin: %s", asset->OriginPath.Get());

    ImGui::Text("Id: %lld", asset->Id);

    ImGui::Text("Frame data was unclaimed: %d", asset->FrameDataWasUnclaimed);
    ImGui::Text("Data claims: %d", asset->DataClaims);
    
    ImGui::Text("Has data: %d", asset->HasData);
    ImGui::Text("Is dirty: %d", asset->IsDirty);
}

void AssetDetailsWidget::StaticMeshDetails(StaticMeshAsset* asset)
{
    ImGui::Text("Vertex count: %d", asset->VertexCount);
    ImGui::Text("Index count: %d", asset->IndexCount);
    
    ImGui::Text("Vertex buffer created: %d", asset->VertexBufferView.IsResourceValid());
    ImGui::Text("Index buffer created: %d", asset->IndexBufferView.IsResourceValid());

    if (ImGui::BeginCombo("Material", asset->Material ? asset->Material->BaseAsset->Name.c_str() : "None"))
    {
        for (MaterialAsset* material : AssetManager::Get().GetMaterials())
        {
            if (ImGui::Selectable(material->BaseAsset->Name.c_str(), asset->Material == material))
            {
                asset->SetMaterial(material);
            }
        }

        ImGui::EndCombo();
    }
}

void AssetDetailsWidget::TextureDetails(TextureAsset* asset)
{
    ImGui::Text("Width: %d", asset->Width);
    ImGui::Text("Height: %d", asset->Height);
    ImGui::Text("Mips: %d", asset->Mips);

    ImGui::Text("Type: %d", asset->RenderTextureType);
    ImGui::Text("Format: %d", asset->Format);

    if (asset->TextureView)
    {
        f32 size = std::min<f32>(ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
        ImGui::Image(asset->TextureView.GPUHandlePtr(), { size, size });
    }
}

void AssetDetailsWidget::MaterialDetails(MaterialAsset* asset)
{
    SelectTexture("Albedo texture", asset->AlbedoTexture);
    SelectTexture("Normal texture", asset->NormalTexture);
    SelectTexture("Metallic texture", asset->MetalicTexture);
    SelectTexture("Roughness texture", asset->RoughnessTexture);

    ImGui::ColorPicker3("Base color", glm::value_ptr(asset->BaseColor));

    ImGui::SliderFloat("Roughness", &asset->Roughness, 0.0f, 1.0f);
    ImGui::SliderFloat("Metallic", &asset->Metalic, 0.0f, 1.0f);
    ImGui::SliderFloat("Emission", &asset->Emission, 0.0f, 20.0f);
}

void AssetDetailsWidget::PrefabDetails(PrefabAsset* asset)
{
    ImGui::Text("Entities count: %d", asset->EntitiesCount);
    ImGui::Text("Features count: %d", asset->FeaturesCount);
}

void AssetDetailsWidget::SelectTexture(ccstr8 lable, TextureAsset*& asset)
{
    if (ImGui::BeginCombo(lable, asset ? asset->BaseAsset->Name.c_str() : "None"))
    {
        for (TextureAsset* texture : AssetManager::Get().GetTextures())
        {
            ImGui::PushID(static_cast<i32>(reinterpret_cast<iptr>(texture)));

            f32 size = std::min<f32>(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()) / 5.0f;

            if (texture->TextureView)
            {
                ImGui::Image(texture->TextureView.GPUHandlePtr(), { size, size });
                ImGui::SameLine();
            }

            if (ImGui::Selectable((texture->BaseAsset->Name + "##texture").c_str(), asset == texture))
            {
                // TODO: This will bite me in a future.
                asset->BaseAsset->UnclaimData();
                texture->BaseAsset->ClaimData();
                asset = texture;
            }

            ImGui::PopID();
        }

        ImGui::EndCombo();
    }

    if (asset)
    {
        f32 size = std::min<f32>(ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
        ImGui::Image(asset->TextureView.GPUHandlePtr(), { size, size });
    }
}