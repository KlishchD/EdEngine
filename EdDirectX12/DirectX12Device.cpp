#include "EdDirectX12.h"

void D3DMessageCallback(D3D12_MESSAGE_CATEGORY category, D3D12_MESSAGE_SEVERITY severity, D3D12_MESSAGE_ID id, LPCSTR pDescription, void* pContext)
{
    if (severity >= D3D12_MESSAGE_SEVERITY_WARNING && g_Device)
    {
        ED_LOG(DirectX12, err, "[{}] [{}] [{}] {}", static_cast<u32>(category), static_cast<u32>(severity), static_cast<u32>(id), pDescription);
    }
}

Device::Device() : m_NativeHandle(0)
{
    ED_ASSERT(!g_Device, "Can only create one D3D12 device.");

#if D3D_DEBUG
    {
        Microsoft::WRL::ComPtr<ID3D12DeviceRemovedExtendedDataSettings1> pDredSettings;
        D3D::Check(D3D12GetDebugInterface(IID_PPV_ARGS(&pDredSettings)));

        // Turn on AutoBreadcrumbs and Page Fault reporting
        pDredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
        pDredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);

        static Microsoft::WRL::ComPtr<ID3D12Debug6> debugLayer;
        D3D::Check(D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer)));
        debugLayer->EnableDebugLayer();
        debugLayer->SetEnableGPUBasedValidation(true);

        static Microsoft::WRL::ComPtr<IDXGIDebug1> dxgiDebugLayer;
        D3D::Check(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebugLayer)));
        dxgiDebugLayer->EnableLeakTrackingForThread();

        static Microsoft::WRL::ComPtr<IDXGIInfoQueue> queue;
        D3D::Check(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&queue)));

        D3D::Check(queue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true));
        D3D::Check(queue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true));
        D3D::Check(queue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING, false));
        D3D::Check(queue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO, false));
        D3D::Check(queue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_MESSAGE, false));
    }
#endif

    u32 flags = D3D_DEBUG ? DXGI_CREATE_FACTORY_DEBUG : 0;
    D3D::Check(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&g_Factory)));

    // TODO: Add monitor detection.

    for (uint32_t i = 0; SUCCEEDED(g_Factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&g_Adapter))); ++i)
    {
        D3D::Check(D3D12CreateDevice(g_Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&g_Device)));

        if (g_Device)
        {
            m_NativeHandle = reinterpret_cast<uptr>(g_Device.Get());
            break;
        }
    }
   
    DXGI_ADAPTER_DESC adapterDescription;
    D3D::Check(g_Adapter->GetDesc(&adapterDescription));

    ED_LOG(Device, info, "Adapter description: {}.", Strings::Convert(adapterDescription.Description, 128, true));
    ED_LOG(Device, info, "Adapter vendorID: {}.", adapterDescription.VendorId);
    ED_LOG(Device, info, "Adapter deviceID: {}.", adapterDescription.DeviceId);

    ED_LOG(Device, info, "Adapter dedicated video memory: {:L} GB.", adapterDescription.DedicatedVideoMemory / Memory::BytesToGB);
    ED_LOG(Device, info, "Adapter dedicated system memory: {:L} GB.", adapterDescription.DedicatedSystemMemory / Memory::BytesToGB);
    ED_LOG(Device, info, "Adapter shared system memory: {:L} GB.", adapterDescription.SharedSystemMemory / Memory::BytesToGB);

    DeviceMemoryInformation memory = QueryDeviceMemory();
    D3D::Check(g_Adapter->SetVideoMemoryReservation(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, memory.Reservable));

#if DEBUG_BUILD || DEVELOPMENT_BUILD
    LogDeviceMemoryInformation();
#endif

#if D3D_DEBUG
    {
        Microsoft::WRL::ComPtr<ID3D12InfoQueue1> queue;
        D3D::Check(g_Device->QueryInterface<ID3D12InfoQueue1>(&queue));

        D3D::Check(queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true));
        D3D::Check(queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true));
        D3D::Check(queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false));
        D3D::Check(queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_INFO, false));
        D3D::Check(queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_MESSAGE, false));

        DWORD word;
        D3D::Check(queue->RegisterMessageCallback(D3DMessageCallback, D3D12_MESSAGE_CALLBACK_FLAG_NONE, nullptr, &word));
    }
#endif
}

DeviceMemoryInformation Device::QueryDeviceMemory() const
{
    DXGI_QUERY_VIDEO_MEMORY_INFO query;
    g_Adapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &query);

    return { query.Budget, query.CurrentUsage, query.AvailableForReservation, query.CurrentReservation };
}

#if DEBUG_BUILD || DEVELOPMENT_BUILD

void Device::LogDeviceMemoryInformation() const
{
    DeviceMemoryInformation query = QueryDeviceMemory();

    ED_LOG(Device, info, "{} GB budget.", 1.0f * query.Budget / Memory::BytesToGB);
    ED_LOG(Device, info, "{} GB used.", 1.0f * query.Usage / Memory::BytesToGB);
    ED_LOG(Device, info, "{} GB reservable.", 1.0f * query.Reservable / Memory::BytesToGB);
    ED_LOG(Device, info, "{} GB reserved.", 1.0f * query.Reserved / Memory::BytesToGB);
}

#endif

Device::~Device()
{
    g_Device = nullptr;
    g_Factory = nullptr;
}
