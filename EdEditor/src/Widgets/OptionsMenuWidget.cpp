#include "OptionsMenuWidget.h"
#include "Utils/PlatformUtils.h"
#include "Core/Engine.h"
#include "Core/Assets/AssetManager.h"
#include "Core/Assets/StaticMesh.h"
#include "Core/Assets/Texture2D.h"

#include <imgui.h>

void OptionsMenuWidget::Initialize()
{
    Widget::Initialize();
    
    m_Window = &Window::Get();
    m_Engine = &Engine::Get();
    m_AssetManager = m_Engine->GetManager<AssetManager>();
}

void OptionsMenuWidget::Tick(float DeltaTime)
{
    Widget::Tick(DeltaTime);
    
    if (m_StaticMeshImportPopupIsOpened) StaticMeshImportPopup();
    if (m_TextureImportPopupIsOpened) TextureImportPopup();

        
    if (ImGui::BeginMainMenuBar())
    {
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
                m_TextureImportParameters.ImagePath = PlatformUtils::OpenFileWindow("Texture\0", *m_Window, "Texture");
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
        if (ImGui::BeginCombo("WrapS", m_TextureImportParameters.WrapS == GL_CLAMP_TO_EDGE ? "Clamp to border" : "Repeat"))
        {
            if (ImGui::Selectable("Clamp to border", m_TextureImportParameters.WrapS == GL_CLAMP_TO_EDGE))
            {
                m_TextureImportParameters.WrapS = GL_CLAMP_TO_EDGE;
            }
            
            
            if (ImGui::Selectable("Repeat", m_TextureImportParameters.WrapS == GL_REPEAT))
            {
                m_TextureImportParameters.WrapS = GL_REPEAT;
            }
            ImGui::EndCombo();
        }
        if (ImGui::BeginCombo("WrapT", m_TextureImportParameters.WrapT == GL_CLAMP_TO_EDGE ? "Clamp to border" : "Repeat"))
        {
            if (ImGui::Selectable("Clamp to border", m_TextureImportParameters.WrapT == GL_CLAMP_TO_EDGE))
            {
                m_TextureImportParameters.WrapT = GL_CLAMP_TO_EDGE;
            }
            
            if (ImGui::Selectable("Repeat", m_TextureImportParameters.WrapT == GL_REPEAT))
            {
                m_TextureImportParameters.WrapT = GL_REPEAT;
            }
            ImGui::EndCombo();
        }
        if (ImGui::BeginCombo("Internal format", m_TextureImportParameters.InternalFormat == GL_RGBA ? "RGBA8" : "SRGBA8"))
        {
            if (ImGui::Selectable("RGBA8", m_TextureImportParameters.InternalFormat == GL_RGBA))
            {
                m_TextureImportParameters.InternalFormat = GL_RGBA;
            }
            
            if (ImGui::Selectable("SRGBA8", m_TextureImportParameters.InternalFormat == GL_SRGB_ALPHA))
            {
                m_TextureImportParameters.InternalFormat = GL_SRGB_ALPHA;
            }
            ImGui::EndCombo();
        }
        if (ImGui::BeginCombo("External format", m_TextureImportParameters.ExternalFormat == GL_RGBA ? "RGBA8" : "SRGBA8"))
        {
            if (ImGui::Selectable("RGBA8", m_TextureImportParameters.ExternalFormat == GL_RGBA))
            {
                m_TextureImportParameters.ExternalFormat = GL_RGBA;
            }
            
            if (ImGui::Selectable("SRGBA8", m_TextureImportParameters.ExternalFormat == GL_SRGB_ALPHA))
            {
                m_TextureImportParameters.ExternalFormat = GL_SRGB_ALPHA;
            }
            ImGui::EndCombo();
        }
        if (ImGui::BeginCombo("Filtering", m_TextureImportParameters.Filtering == GL_LINEAR ? "Linear" : "Nearest"))
        {
            if (ImGui::Selectable("Linear", m_TextureImportParameters.Filtering == GL_LINEAR))
            {
                m_TextureImportParameters.Filtering = GL_LINEAR;
            }
            
            if (ImGui::Selectable("Nearest", m_TextureImportParameters.Filtering == GL_NEAREST))
            {
                m_TextureImportParameters.Filtering = GL_NEAREST;
            }
            ImGui::EndCombo();
        }
        if (ImGui::BeginCombo("Data type", m_TextureImportParameters.DataType == GL_UNSIGNED_BYTE ? "Unsigned byte" : ""))
        {
            if (ImGui::Selectable("Unsigned byte", m_TextureImportParameters.DataType == GL_UNSIGNED_BYTE))
            {
                m_TextureImportParameters.DataType = GL_UNSIGNED_BYTE;
            }
            ImGui::EndCombo();
        }
        
        if (ImGui::Button("Import")) {
            m_AssetManager->ImportTexture(m_TextureImportParameters);

            m_TextureImportPopupIsOpened = false;

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}
