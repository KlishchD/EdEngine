#include "OptionsMenuWidget.h"
#include "Utils/PlatformUtils.h"
#include "Core/Engine.h"
#include "Core/Assets/AssetManager.h"
#include "Core/Assets/StaticMesh.h"
#include "Core/Rendering/Textures/Texture2D.h"
#include "Core/Scene.h"
#include "Utils/Files.h"
#include "Utils/RenderingHelper.h"
#include <imgui.h>
#include "Core/Macros.h"

void OptionsMenuWidget::Initialize()
{
    Widget::Initialize();
    
    m_Engine = &Engine::Get();
    m_Window = m_Engine->GetWindow();
    m_AssetManager = m_Engine->GetManager<AssetManager>();

    m_Icon = m_AssetManager->LoadTexture(RenderingHelper::GetDefaultBaseColorTexture2DImportParameters(Files::ContentFolderPath + R"(Editor\icons\icon.png)"));
}

void OptionsMenuWidget::Tick(float DeltaTime)
{
    Widget::Tick(DeltaTime);
    
    if (m_StaticMeshImportPopupIsOpened) StaticMeshImportPopup();
    if (m_TextureImportPopupIsOpened) TextureImportPopup();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 10 });

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::IsMouseDoubleClicked(0))
        {
			ED_LOG(Widget, info, "Double cliked")
		}

        if (ImGui::ImageButton((ImTextureID)m_Icon->GetID(), { ImGui::GetWindowHeight(), ImGui::GetWindowHeight() }, { 0, 1 }, { 1, 0 }, 0, ImVec4(0.14f, 0.14f, 0.14f, 1.00f), {1, 1, 1, 1}))
        {

        }

        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Import mesh"))
            {
                m_StaticMeshImportParameters = StaticMeshImportParameters();
                m_StaticMeshImportParameters.MeshPath = PlatformUtils::OpenFileWindow("Model\0", *m_Window, "Model");
                m_StaticMeshImportPopupIsOpened = true;
            }
    
            if (ImGui::MenuItem("Import texture"))
            {
                m_TextureImportParameters = Texture2DImportParameters();
                m_TextureImportParameters.Format = PixelFormat::SRGBA8F;
                m_TextureImportParameters.Path = PlatformUtils::OpenFileWindow("Texture\0", *m_Window, "Texture");
                m_TextureImportPopupIsOpened = true;
            }

            if (ImGui::MenuItem("Create Material"))
            {
                std::string materialPath = PlatformUtils::OpenFileWindow("Material\0", *m_Window, "Material");

                m_AssetManager->CreateMaterial(materialPath);
            }
            
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Scene"))
        {
            if (ImGui::MenuItem("Add actor"))
            {
                m_Engine->GetLoadedScene()->CreateActor<Actor>("New Actor");
            }
            
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }

	ImGui::PopStyleVar();
}

void OptionsMenuWidget::StaticMeshImportPopup()
{
    ImGui::OpenPopup("Static mesh import parameters");
    
    if (ImGui::BeginPopup("Static mesh import parameters"))
    {
        ImGui::Checkbox("Join Identical Vertices", &m_StaticMeshImportParameters.JoinIdenticalVertices);
        ImGui::Checkbox("Gen UV Coords", &m_StaticMeshImportParameters.GenUVCoords);
        ImGui::Checkbox("Calculate Tangent Space", &m_StaticMeshImportParameters.CalculateTangentSpace);
        ImGui::Checkbox("Fix Infacing Normals", &m_StaticMeshImportParameters.FixInfacingNormals);
                                                 
        if (ImGui::Button("Import"))
        {
            m_AssetManager->ImportMesh(m_StaticMeshImportParameters);
            m_StaticMeshImportPopupIsOpened = false;

            ImGui::CloseCurrentPopup();
        }
                    
        ImGui::EndPopup();
    }
}

void OptionsMenuWidget::TextureImportPopup()
{
    ImGui::OpenPopup("Texture import parameters");

    if (ImGui::BeginPopup("Texture import parameters"))
	{
        if (ImGui::BeginCombo("WrapS", m_TextureImportParameters.WrapS == WrapMode::ClampToEdge ? "Clamp to border" : "Repeat"))
        {
            if (ImGui::Selectable("Clamp to border", m_TextureImportParameters.WrapS == WrapMode::ClampToEdge))
            {
                m_TextureImportParameters.WrapS = WrapMode::ClampToEdge;
            }
            if (ImGui::Selectable("Repeat", m_TextureImportParameters.WrapS == WrapMode::Repeat))
            {
                m_TextureImportParameters.WrapS = WrapMode::Repeat;
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("WrapT", m_TextureImportParameters.WrapT == WrapMode::ClampToEdge ? "Clamp to border" : "Repeat"))
        {
            if (ImGui::Selectable("Clamp to border", m_TextureImportParameters.WrapT == WrapMode::ClampToEdge))
            {
                m_TextureImportParameters.WrapT = WrapMode::ClampToEdge;
            }
            if (ImGui::Selectable("Repeat", m_TextureImportParameters.WrapT == WrapMode::Repeat))
            {
                m_TextureImportParameters.WrapT = WrapMode::Repeat;
            }
            ImGui::EndCombo();
        }
        
        if (ImGui::BeginCombo("Format", m_TextureImportParameters.Format == PixelFormat::RGBA8F ? "RGBA8" : "SRGBA8"))
        {
            if (ImGui::Selectable("RGBA8", m_TextureImportParameters.Format == PixelFormat::RGBA8F))
            {
                m_TextureImportParameters.Format = PixelFormat::RGBA8F;
            }
            if (ImGui::Selectable("SRGBA8", m_TextureImportParameters.Format == PixelFormat::SRGBA8F))
            {
                m_TextureImportParameters.Format = PixelFormat::SRGBA8F;
            }
            ImGui::EndCombo();
        }
        if (ImGui::BeginCombo("Filtering", m_TextureImportParameters.Filtering == FilteringMode::Linear ? "Linear" : "Nearest"))
        {
            if (ImGui::Selectable("Linear", m_TextureImportParameters.Filtering == FilteringMode::Linear))
            {
                m_TextureImportParameters.Filtering = FilteringMode::Linear;
            }
            
            if (ImGui::Selectable("Nearest", m_TextureImportParameters.Filtering == FilteringMode::Nearest))
            {
                m_TextureImportParameters.Filtering = FilteringMode::Nearest;
            }
            ImGui::EndCombo();
        }
        
        ImGui::Checkbox("Generate MipMaps", &m_TextureImportParameters.GenerateMipMaps);

        if (ImGui::Button("Import")) {
            m_AssetManager->ImportTexture(m_TextureImportParameters);

            m_TextureImportPopupIsOpened = false;

            ImGui::CloseCurrentPopup();
        }


        ImGui::EndPopup();
    }
}
