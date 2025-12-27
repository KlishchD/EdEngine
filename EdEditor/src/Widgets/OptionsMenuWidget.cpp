#include "EdEditor.h"
#include "OptionsMenuWidget.h"
#include "Helpers/FilesHelper.h"
#include "Helpers/RenderingHelper.h"
#include "Helpers/AssetHelper.h"
#include "Helpers/PlatformHelper.h"

void OptionsMenuWidget::Initialize()
{
    Widget::Initialize();
    
    m_Icon = AssetHelper::ImportOrLoadTexture(ContentPath("Editor\\icons\\light.png"), "Editor\\icons\\", TextureAsset::Albedo, "EngineIcon", PixelFormat::RGBA8F, 5);
}

void OptionsMenuWidget::Tick(f32 DeltaTime)
{
    Widget::Tick(DeltaTime);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 10 });

    if (ImGui::BeginMainMenuBar())
    {
        Engine::Get().GetWindow()->SetDragging(ImGui::IsWindowHovered());

        ImGui::Image(m_Icon->TextureView.GPUHandlePtr(), ImVec2(ImGui::GetWindowHeight(), ImGui::GetWindowHeight()), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0.14f, 0.14f, 0.14f, 1.00f), ImVec4(1, 1, 1, 1));

        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Import Scene"))
            {
                Path scene = PlatformHelper::OpenFileWindow(".fbx|.obj", *Engine::Get().GetWindow(), "Select scene to import.");
                Path save = PlatformHelper::SaveFileWindow(".edprefab", *Engine::Get().GetWindow(), "Select save destination.", Files::PrefabAssetExtension);

                if (scene.IsValid())
                {
                    AssetHelper::ImportScene(scene, save.Content(), save.GetFileName(), true, true);
                }
                else
                {
                    ED_LOG(OptionsMenuWidget, err, "Invalid path {}", scene.Get());
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Layout"))
        {
            if (ImGui::MenuItem("Save"))
            {
                ImGui::SaveIniSettingsToDisk(Files::GetEditorLayoutPath());
            }

            if (ImGui::MenuItem("Load"))
            {
                ImGui::LoadIniSettingsFromDisk(Files::GetEditorLayoutPath());
            }

            if (ImGui::MenuItem("Save as"))
            {
                Path path = PlatformHelper::SaveFileWindow(".ini", *Engine::Get().GetWindow(), "Save layout.", ".ini");
                
                if (path.IsFile()) // Needs better validation than this
                {
                    ImGui::SaveIniSettingsToDisk(path.Get());
                }
                else
                {
                    ED_LOG(OptionsMenuWidget, err, "Invalid path {}", path.Get());
                }
            }

            if (ImGui::MenuItem("Load as"))
            {
                Path path = PlatformHelper::OpenFileWindow(".ini", *Engine::Get().GetWindow(), "Load layout.");
                
                if (path.IsValid() && path.IsFile())
                {
                    ImGui::LoadIniSettingsFromDisk(path.Get());
                }
                else
                {
                    ED_LOG(OptionsMenuWidget, err, "Invalid path {}", path.Get());
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Scene"))
        {
            if (ImGui::MenuItem("Save", nullptr, nullptr, EntityManager::Get().GetLoadedScenePath() != Files::GetDefaultScenePath()))
            {
                EntityManager::Get().SaveScene();
            }

            if (ImGui::MenuItem("Load"))
            {
                EntityManager::Get().LoadScene();
            }

            if (ImGui::MenuItem("Save as"))
            {
                ContentPath path = PlatformHelper::SaveFileWindow(Files::SceneAssetExtension, *Engine::Get().GetWindow(), "Save scene.", Files::SceneAssetExtension).Content();

                if (path.IsFile())
                {
                    EntityManager::Get().SaveScene(path);
                }
                else
                {
                    ED_LOG(Options, err, "Scene save path is incorrect '{}'", path.Get());
                }
            }

            if (ImGui::MenuItem("Load as"))
            {
                ContentPath path = PlatformHelper::OpenFileWindow(Files::FullSceneAssetExtension, *Engine::Get().GetWindow(), "Save scene.").Content();

                if (path.IsValid() && path.IsFile())
                {
                    EntityManager::Get().LoadScene(path);
                }
                else
                {
                    ED_LOG(Options, err, "Scene save path is incorrect '{}'", path.Get());
                }
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
      
    ImGui::PopStyleVar();

//    if (m_StaticMeshImportPopupIsOpened) StaticMeshImportPopup();
//    if (m_TextureImportPopupIsOpened) TextureImportPopup();
//
//    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 10 });
//
//    if (ImGui::BeginMainMenuBar())
//    {
//        if (ImGui::IsMouseDoubleClicked(0))
//        {
//		    	ED_LOG(Widget, info, "Double cliked")
//		    }
//
//        if (ImGui::ImageButton("Options", m_Icon->GetID(), ImVec2(ImGui::GetWindowHeight(), ImGui::GetWindowHeight()), ImVec2(0, 1), ImVec2(1, 0), ImVec4(0.14f, 0.14f, 0.14f, 1.00f), ImVec4(1, 1, 1, 1)))
//        {
//
//        }
//
//        if (ImGui::BeginMenu("File"))
//        {
//            if (ImGui::MenuItem("Import mesh"))
//            {
//                m_StaticMeshImportParameters = std::make_shared<StaticMeshImportParameters>();
//                m_StaticMeshImportParameters->Path = PlatformHelper::OpenFileWindow("Model\0", *m_Window, "Model");
//                m_StaticMeshImportPopupIsOpened = true;
//            }
//    
//            if (ImGui::MenuItem("Import texture"))
//            {
//                m_TextureImportParameters = std::make_shared<Texture2DImportParameters>();
//                m_TextureImportParameters->Format = PixelFormat::SRGBA8F;
//                m_TextureImportParameters->Path = PlatformHelper::OpenFileWindow("Texture\0", *m_Window, "Texture");
//                m_TextureImportPopupIsOpened = true;
//            }
//
//            if (ImGui::MenuItem("Create Material"))
//            {
//                std::string materialPath = PlatformHelper::OpenFileWindow("Material\0", *m_Window, "Material");
//
//                m_AssetManager->CreateAsset<Material>(AssetType::Material, materialPath);
//            }
//            
//            ImGui::EndMenu();
//        }
//
//        if (ImGui::BeginMenu("Scene"))
//        {
//            if (ImGui::MenuItem("Add actor"))
//            {
//                m_Engine->GetLoadedScene()->CreateActor<Actor>("New Actor");
//            }
//            
//            ImGui::EndMenu();
//        }
//        
//        ImGui::EndMainMenuBar();
//    }
//
//	ImGui::PopStyleVar();
}
// 
// void OptionsMenuWidget::StaticMeshImportPopup()
// {
//     ImGui::OpenPopup("Static mesh import parameters");
//     
//     if (ImGui::BeginPopup("Static mesh import parameters"))
//     {
//         ImGui::Checkbox("Join Identical Vertices", &m_StaticMeshImportParameters->JoinIdenticalVertices);
//         ImGui::Checkbox("Gen UV Coords", &m_StaticMeshImportParameters->GenUVCoords);
//         ImGui::Checkbox("Calculate Tangent Space", &m_StaticMeshImportParameters->CalculateTangentSpace);
//         ImGui::Checkbox("Fix Infacing Normals", &m_StaticMeshImportParameters->FixInfacingNormals);
//                                                  
//         if (ImGui::Button("Import"))
//         {
//             m_AssetManager->ImportAsset<StaticMesh>(AssetType::StaticMesh, m_StaticMeshImportParameters);
//             m_StaticMeshImportPopupIsOpened = false;
// 
//             ImGui::CloseCurrentPopup();
//         }
//                     
//         ImGui::EndPopup();
//     }
// }
// 
// void OptionsMenuWidget::TextureImportPopup()
// {
//     ImGui::OpenPopup("Texture import parameters");
// 
//     if (ImGui::BeginPopup("Texture import parameters"))
// 	{
//         if (ImGui::BeginCombo("WrapS", m_TextureImportParameters->WrapS == WrapMode::ClampToEdge ? "Clamp to border" : "Repeat"))
//         {
//             if (ImGui::Selectable("Clamp to border", m_TextureImportParameters->WrapS == WrapMode::ClampToEdge))
//             {
//                 m_TextureImportParameters->WrapS = WrapMode::ClampToEdge;
//             }
//             if (ImGui::Selectable("Repeat", m_TextureImportParameters->WrapS == WrapMode::Repeat))
//             {
//                 m_TextureImportParameters->WrapS = WrapMode::Repeat;
//             }
//             ImGui::EndCombo();
//         }
// 
//         if (ImGui::BeginCombo("WrapT", m_TextureImportParameters->WrapT == WrapMode::ClampToEdge ? "Clamp to border" : "Repeat"))
//         {
//             if (ImGui::Selectable("Clamp to border", m_TextureImportParameters->WrapT == WrapMode::ClampToEdge))
//             {
//                 m_TextureImportParameters->WrapT = WrapMode::ClampToEdge;
//             }
//             if (ImGui::Selectable("Repeat", m_TextureImportParameters->WrapT == WrapMode::Repeat))
//             {
//                 m_TextureImportParameters->WrapT = WrapMode::Repeat;
//             }
//             ImGui::EndCombo();
//         }
//         
//         if (ImGui::BeginCombo("Format", m_TextureImportParameters->Format == PixelFormat::RGBA8F ? "RGBA8" : "SRGBA8"))
//         {
//             if (ImGui::Selectable("RGBA8", m_TextureImportParameters->Format == PixelFormat::RGBA8F))
//             {
//                 m_TextureImportParameters->Format = PixelFormat::RGBA8F;
//             }
//             if (ImGui::Selectable("SRGBA8", m_TextureImportParameters->Format == PixelFormat::SRGBA8F))
//             {
//                 m_TextureImportParameters->Format = PixelFormat::SRGBA8F;
//             }
//             ImGui::EndCombo();
//         }
//         if (ImGui::BeginCombo("Filtering", m_TextureImportParameters->Filtering == FilteringMode::Linear ? "Linear" : "Nearest"))
//         {
//             if (ImGui::Selectable("Linear", m_TextureImportParameters->Filtering == FilteringMode::Linear))
//             {
//                 m_TextureImportParameters->Filtering = FilteringMode::Linear;
//             }
//             
//             if (ImGui::Selectable("Nearest", m_TextureImportParameters->Filtering == FilteringMode::Nearest))
//             {
//                 m_TextureImportParameters->Filtering = FilteringMode::Nearest;
//             }
//             ImGui::EndCombo();
//         }
//         
//         ImGui::Checkbox("Generate MipMaps", &m_TextureImportParameters->GenerateMipMaps);
// 
//         if (ImGui::Button("Import")) {
// 			m_AssetManager->ImportAsset<Texture2D>(AssetType::Texture2D, m_TextureImportParameters);
// 
//             m_TextureImportPopupIsOpened = false;
// 
//             ImGui::CloseCurrentPopup();
//         }
// 
// 
//         ImGui::EndPopup();
//     }
// }
