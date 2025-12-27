#include "EdEditor.h"
#include "Editor.h"
#include "Widgets/EntityDetatilsWidget.h"
#include "Widgets/AssetDetails.h"
#include "Widgets/CameraDetailsWidget.h"
#include "Widgets/ContentBrowserWidget.h"
#include "Widgets/OptionsMenuWidget.h"
#include "Widgets/SceneTreeWidget.h"
#include "Widgets/ViewportWidget.h"
#include "Widgets/RendererStatsWidget.h"
#include "Helpers/FilesHelper.h"
#include "Helpers/RenderingHelper.h"
#include "Helpers/AssetHelper.h"
#include "Renderer.h"

#include "Device.h"
#include "DescriptorHeap.h"
#include "CommandList.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "RenderingContext.h"

#include "Console.h"

#if RenderAPI == DirectX12API
    #include <d3d12.h>
    #include <dxgi1_6.h>

    #include "GLFW/glfw3.h"

    #include "backends/imgui_impl_dx12.h"
    #include "backends/imgui_impl_glfw.h"
#else
#error Suplied render API is not supported.
#endif

void Editor::Deinitialize()
{
    for (Widget* widget : m_Widgets)
    {
        delete widget;
    }
    m_Widgets.Clear();

#if RenderAPI == OpenGLAPI
    {
        ImGui_ImplGlfw_Shutdown();
        ImGui_ImplOpenGL3_Shutdown();
    }
#elif RenderAPI == DirectX12API
    {
        ImGui_ImplGlfw_Shutdown();
        ImGui_ImplDX12_Shutdown();
    }
#else
#error Suplied render API is not supported.
#endif

    ImGui::DestroyContext();
}

Editor::Editor()
{
    ED_ASSERT(!s_Editor, "Only one editor can be created at once");
    s_Editor = this;
}

void Editor::Initialize(Engine* engine)
{
    m_Engine = engine;
    m_Window = engine->GetWindow();

    m_Camera.CameraObject = Camera(90.0f, 16.0f / 9.0f, 0.1f, 100.0f);

    SetUpInputs(engine);

    m_MousePosition = m_Window->GetMousePosition();

    CreateWidget<OptionsMenuWidget>();
    CreateWidget<CameraDetailsWidget>();
    CreateWidget<ViewportWidget>();
    CreateWidget<ContentBrowserWidget>();
    CreateWidget<SceneTreeWidget>();
    CreateWidget<EntityDetailsWidget>();
    CreateWidget<AssetDetailsWidget>();
//    m_Engine->CreateWidget<RendererStatsWidget>();

    m_Engine->SubscribeToPostUpdate(&Editor::StaticUpdate);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    ImGui::StyleColorsClassic();

#if RenderAPI == OpenGLAPI
    {
        ImGui_ImplGlfw_InitForOpenGL(reinterpret_cast<GLFWwindow*>(m_Window->GetNativeWindow()), true);
        ImGui_ImplOpenGL3_Init("#version 460");
    }
#elif RenderAPI == DirectX12API
    {
        RenderingContext& context = RenderingContext::Get();
        ImGui_ImplGlfw_InitForOther(reinterpret_cast<GLFWwindow*>(m_Window->GetNativeWindow()), true);

        ID3D12Device* deviceRaw = context.GetDevice()->GetNativeHandle<ID3D12Device>();

        DescriptorHeap* SRVHeap = context.GetSRVHeap();
        u32 slot = SRVHeap->ReserveSlot();
        u32 slotSize = deviceRaw->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        ID3D12DescriptorHeap* SRVHeapRaw = SRVHeap->GetNativeHandle<ID3D12DescriptorHeap>();

        D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle = { SRVHeapRaw->GetCPUDescriptorHandleForHeapStart().ptr + slot * slotSize };
        D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle = { SRVHeapRaw->GetGPUDescriptorHandleForHeapStart().ptr + slot * slotSize };

        ImGui_ImplDX12_Init(deviceRaw, 2, DXGI_FORMAT_R8G8B8A8_UNORM, SRVHeapRaw, CPUHandle, GPUHandle);
    }
#else
#error Suplied render API is not supported.
#endif

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
    colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
    colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(8.00f, 8.00f);
    style.FramePadding = ImVec2(5.00f, 2.00f);
    style.CellPadding = ImVec2(6.00f, 6.00f);
    style.ItemSpacing = ImVec2(6.00f, 6.00f);
    style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
    style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
    style.IndentSpacing = 25;
    style.ScrollbarSize = 15;
    style.GrabMinSize = 10;
    style.WindowBorderSize = 1;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 1;
    style.FrameBorderSize = 1;
    style.TabBorderSize = 1;
    style.WindowRounding = 7;
    style.ChildRounding = 4;
    style.FrameRounding = 3;
    style.PopupRounding = 4;
    style.ScrollbarRounding = 9;
    style.GrabRounding = 3;
    style.LogSliderDeadzone = 4;
    style.TabRounding = 4;

    ImGui::LoadIniSettingsFromDisk(Files::GetEditorLayoutPath());
}

namespace Console
{
    template <typename T> requires(std::is_arithmetic_v<T>)
    constexpr ImGuiDataType_ GetImGuiType()
    {
        if constexpr (std::is_same_v<T, i32>)
        {
            return ImGuiDataType_::ImGuiDataType_S32;
        }

        if constexpr (std::is_same_v<T, u32>)
        {
            return ImGuiDataType_::ImGuiDataType_U32;
        }
        
        if constexpr (std::is_same_v<T, i64>)
        {
            return ImGuiDataType_::ImGuiDataType_S64;
        }
        
        if constexpr (std::is_same_v<T, u64>)
        {
            return ImGuiDataType_::ImGuiDataType_U64;
        }
        
        if constexpr (std::is_same_v<T, f32>)
        {
            return ImGuiDataType_::ImGuiDataType_Float;
        }

        if constexpr (std::is_same_v<T, f64>)
        {
            return ImGuiDataType_::ImGuiDataType_Double;
        }
    }

    template <typename T>
    void EditorControls(Variable* variable)
    {
        T value = GetValue<T>(variable);
        T min = GetMin<T>(variable);
        T max = GetMax<T>(variable);
        if (ImGui::SliderScalar(variable->Name, GetImGuiType<T>(), &value, &min, &max))
        {
            SetValue<T>(variable, value);
        }
    }

    void EditorControls(Variable* variable)
    {
        switch (variable->Type)
        {
        case Types::Int: EditorControls<i32>(variable); break;
        case Types::UInt: EditorControls<u32>(variable); break;
        case Types::Float: EditorControls<f32>(variable); break;
        case Types::Double: EditorControls<f64>(variable); break;
        default:
            break;
        }
    }
}

void Editor::Update(f32 deltaSeconds)
{
    UpdateMousePosition(deltaSeconds);

    m_Camera.CameraObject.AddPositionOffset(m_Camera.CameraObject.GetForward() * m_Camera.MovementDirection.z * m_Camera.CameraSpeed * deltaSeconds);
    m_Camera.CameraObject.AddPositionOffset(m_Camera.CameraObject.GetRight() * m_Camera.MovementDirection.x * m_Camera.CameraSpeed * deltaSeconds);
    m_Camera.CameraObject.AddPositionOffset(m_Camera.CameraObject.GetUp() * m_Camera.MovementDirection.y * m_Camera.CameraSpeed * deltaSeconds);

    {
        CameraElement element;
        element.Position = m_Camera.CameraObject.GetPosition();
        element.Rotation = m_Camera.CameraObject.GetRotation();
        element.Fov = m_Camera.CameraObject.GetFOVDegrees();
        element.Near = m_Camera.CameraObject.GetNear();
        element.Far = m_Camera.CameraObject.GetFar();
        element.Aspect = m_Camera.CameraObject.GetAspect();
        m_Engine->OverrideCameraForNextFrame(element);
    }

    //AssetHelper::ImportScene("C:\\dev\\Ed\\Resources\\Models\\pkg_a_curtains\\NewSponza_Curtains_FBX_YUp.fbx", "Models\\pkg_a_curtains\\imported\\", "curtains", true, true);

    BeginUIFrame();

    if (ImGui::Begin("Shaders"))
    {
        if (ImGui::Button("Recompile"))
        {
            RenderingContext::Get().RequestShadersReloading();
        }

        const Array<ccstr8>& entries = RenderingContext::Get().GetShadersCompilationResults();
        if (entries.IsEmpty())
        {
            ImGui::Text("All shaders compiled successfully");
        }
        else
        {
            if (ImGui::BeginListBox("Shaders Compilation Results"))
            {
                for (ccstr8 entry : entries)
                {
                    ImGui::Selectable(entry, false);
                }

                ImGui::EndListBox();
            }
        }
    }
    ImGui::End();

    if (ImGui::Begin("Console"))
    {
        for (Console::Command* command : Console::GetRegisty().Commands)
        {
            ImGui::Text("Command: %s", command->Name);
        }

        for (Console::Variable* variable : Console::GetRegisty().Variables)
        {
            Console::EditorControls(variable);
        }
    }
    ImGui::End();

    for (Widget* widget : m_Widgets)
    {
        widget->Tick(deltaSeconds);
    }

    EndUIFrame();
}

void Editor::SetUpInputs(Engine* engine)
{
    InputManager::Get().SubscribeToInput(InputKey::W, InputAction::PressAndRepeat,          [this](InputKey key, InputAction action) { m_Camera.MovementDirection.z = 1.0f;                                                                             });
    InputManager::Get().SubscribeToInput(InputKey::W, InputAction::Release,                 [this](InputKey key, InputAction action) { m_Camera.MovementDirection.z = 0.0f;                                                                             });

    InputManager::Get().SubscribeToInput(InputKey::S, InputAction::PressAndRepeat,          [this](InputKey key, InputAction action) { m_Camera.MovementDirection.z = -1.0f;                                                                            });
    InputManager::Get().SubscribeToInput(InputKey::S, InputAction::Release,                 [this](InputKey key, InputAction action) { m_Camera.MovementDirection.z = 0.0f;                                                                             });

    InputManager::Get().SubscribeToInput(InputKey::A, InputAction::PressAndRepeat,          [this](InputKey key, InputAction action) { m_Camera.MovementDirection.x = -1.0f;                                                                            });
    InputManager::Get().SubscribeToInput(InputKey::A, InputAction::Release,                 [this](InputKey key, InputAction action) { m_Camera.MovementDirection.x = 0.0f;                                                                             });

    InputManager::Get().SubscribeToInput(InputKey::D, InputAction::PressAndRepeat,          [this](InputKey key, InputAction action) { m_Camera.MovementDirection.x = 1.0f;                                                                             });
    InputManager::Get().SubscribeToInput(InputKey::D, InputAction::Release,                 [this](InputKey key, InputAction action) { m_Camera.MovementDirection.x = 0.0f;                                                                             });

    InputManager::Get().SubscribeToInput(InputKey::E, InputAction::PressAndRepeat,          [this](InputKey key, InputAction action) { m_Camera.MovementDirection.y = 1.0f;                                                                             });
    InputManager::Get().SubscribeToInput(InputKey::E, InputAction::Release,                 [this](InputKey key, InputAction action) { m_Camera.MovementDirection.y = 0.0f;                                                                             });

    InputManager::Get().SubscribeToInput(InputKey::Q, InputAction::PressAndRepeat,          [this](InputKey key, InputAction action) { m_Camera.MovementDirection.y = -1.0f;                                                                            });
    InputManager::Get().SubscribeToInput(InputKey::Q, InputAction::Release,                 [this](InputKey key, InputAction action) { m_Camera.MovementDirection.y = 0.0f;                                                                             });
    
    InputManager::Get().SubscribeToInput(InputKey::MouseButton1, InputAction::Press,        [this](InputKey key, InputAction action) { m_IsLeftMouseButtonClicked = true;                                                                               });
    InputManager::Get().SubscribeToInput(InputKey::MouseButton1, InputAction::Release,      [this](InputKey key, InputAction action) { m_IsLeftMouseButtonClicked = false;                                                                              });
    
    InputManager::Get().SubscribeToInput(InputKey::MouseButton2, InputAction::Press,        [this](InputKey key, InputAction action) { m_IsRightMouseButtonClicked = true;                                                                              });
    InputManager::Get().SubscribeToInput(InputKey::MouseButton2, InputAction::Release,      [this](InputKey key, InputAction action) { m_IsRightMouseButtonClicked = false;                                                                             });

    InputManager::Get().SubscribeToInput(InputKey::U, InputAction::Press,                   [this](InputKey key, InputAction action) { Renderer::Get().SetSSAOEnabled(!Renderer::Get().IsSSAOEnabled());                                                });
    InputManager::Get().SubscribeToInput(InputKey::B, InputAction::Press,                   [this](InputKey key, InputAction action) { Renderer::Get().SetAAMethod(Renderer::Get().GetAAMethod() == AAMethod::None ? AAMethod::TAA : AAMethod::None);   });

#if DEBUG_BUILD || DEVELOPMENT_BUILD
    InputManager::Get().SubscribeToInput(InputKey::P, InputAction::Press,                   [this](InputKey key, InputAction action) { RenderingContext::Get().GetDevice()->LogDeviceMemoryInformation();                                               });
#endif

    InputManager::Get().SubscribeToInput(InputKey::R, InputAction::Press,                   [this](InputKey key, InputAction action) { RenderingContext::Get().RequestShadersReloading();                                                               });
}

Editor::~Editor()
{
    s_Editor = nullptr;
}

// 
// void Editor::SetSelectedActor(std::shared_ptr<Actor> actor)
// {
//     if (m_SelectedActor == actor)
//     {
//         m_SelectedActor.reset();
//     }
//     else
//     {
//         m_SelectedActor = actor;
//     }
// 
//     m_SelectedComponent.reset();
// }
// 
// std::shared_ptr<Actor> Editor::GetSelectedActor() const
// {
//     return m_SelectedActor;
// }
// 
// void Editor::SetSelectedComponent(std::shared_ptr<Component> component)
// {
//     if (m_SelectedComponent == component)
//     {
//         m_SelectedComponent.reset();
//     }
//     else
//     {
//         m_SelectedComponent = component;
//     }
// }
// 
// std::shared_ptr<Component> Editor::GetSelectedComponent() const
// {
//     return m_SelectedComponent;
// }

void Editor::UpdateMousePosition(f32 DeltaTime)
{
    glm::vec2 newMousePosition = m_Window->GetMousePosition() / static_cast<glm::vec2>(m_ViewportSize);

    if (m_MousePosition != newMousePosition && m_IsRightMouseButtonClicked)
    {
        glm::vec2 delta = m_MousePosition - newMousePosition;

        f32 yaw = DeltaTime * m_Camera.CameraRotationSpeed.x * delta.x;
        f32 pitch = DeltaTime * m_Camera.CameraRotationSpeed.y * delta.y;

        m_Camera.CameraObject.AddRotation(glm::vec3(pitch, yaw, 0.0f));
    }

    m_MousePosition = newMousePosition;
}

void Editor::BeginUIFrame()
{
#if RenderAPI == OpenGLAPI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
#elif RenderAPI == DirectX12API
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplGlfw_NewFrame();
#else
#error Suplied render API is not supported.
#endif

    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
}

void Editor::EndUIFrame()
{
    ImGui::Render();

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        glfwMakeContextCurrent(reinterpret_cast<GLFWwindow*>(m_Window->GetNativeWindow()));
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

#if RenderAPI == OpenGLAPI
    {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
#elif RenderAPI == DirectX12API
    {
        RenderingContext& context = RenderingContext::Get();

        CommandQueue* queue = context.GetGraphicsCommandQueue();

        GPUScopedEvent _(queue, "ImGui");

        queue->CPUWait();

        CommandList* list = context.GetGraphicsCommandList();

        list->Reset();

        ResourceView displayBuffer = context.GetDisplayBufferRTV();

        list->Transition(displayBuffer, ResourceState::RenderTarget);
        list->SetRenderTargets(displayBuffer, {});
        list->SetDescriptorHeap(context.GetSRVHeap());

        ImGui::Render();

        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), list->GetNativeHandle<ID3D12GraphicsCommandList>());

        list->Transition(displayBuffer, ResourceState::Present);

        list->Close();

        queue->Execute(list);
    }
#else
#error Suplied render API is not supported.
#endif

}

