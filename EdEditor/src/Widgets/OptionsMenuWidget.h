#pragma once

#include <memory>
#include "Core/Widget.h"
#include "Core/Rendering/Textures/Texture2D.h"
#include "Core/Assets/StaticMesh.h"

class OptionsMenuWidget: public Widget
{
public:
    virtual void Initialize() override;
    virtual void Tick(float DeltaTime) override;
private:
    class Engine* m_Engine = nullptr;
    std::shared_ptr<class Window> m_Window;
    std::shared_ptr<class AssetManager> m_AssetManager;
    
    StaticMeshImportParameters m_StaticMeshImportParameters;
    bool m_StaticMeshImportPopupIsOpened = false;

    Texture2DImportParameters m_TextureImportParameters;
    bool m_TextureImportPopupIsOpened = false;

    void StaticMeshImportPopup();
    void TextureImportPopup();
};
