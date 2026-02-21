#include "EdDirectX12.h"

#define GetQueue() reinterpret_cast<ID3D12CommandQueue*>(m_NativeHandle)
#define GetFence() reinterpret_cast<ID3D12Fence*>(m_FenceNativeHandle)

CommandQueue::CommandQueue(CommandListType type, bool hightPriority, ccstr8 name)
{
    D3D12_COMMAND_QUEUE_DESC description;
    description.Type = DirectX12Types::ConvertCommandListType(type);
    description.Priority = hightPriority ? D3D12_COMMAND_QUEUE_PRIORITY_HIGH : D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    description.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    description.NodeMask = 0;
    D3D::Check(g_Device->CreateCommandQueue(&description, __uuidof(ID3D12CommandQueue), reinterpret_cast<void**>(&m_NativeHandle)));

    SetDebugName(name);

    D3D::Check(g_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), reinterpret_cast<void**>(&m_FenceNativeHandle)));
    m_Handle = CreateEvent(nullptr, false, false, TEXT("Fence"));

    SetObjectDebugName<ID3D12Fence>(GetFence(), name);
}

NAME_METHODS_TEMPLATE(CommandQueue, ID3D12CommandQueue)

void CommandQueue::Execute(CommandList* list)
{
    ID3D12CommandList* nativeList = list->GetNativeHandle<ID3D12CommandList>();
    GetNativeHandle<ID3D12CommandQueue>()->ExecuteCommandLists(1, &nativeList);
}

void CommandQueue::BeginEvent(ccstr8 name)
{
#if D3D_DEBUG == 0 && D3D_FORCE_EVENTS_OFF == 0
    GetQueue()->BeginEvent(1, name, strnlen(name, 1024));
#endif
}

void CommandQueue::EndEvent()
{
#if D3D_DEBUG == 0 && D3D_FORCE_EVENTS_OFF == 0
    GetQueue()->EndEvent();
#endif
}

u32 CommandQueue::Signal() const
{
    static u32 signal = 1;
    GetQueue()->Signal(GetFence(), signal);
    return signal++; // Never did it but just could not resist making it here, may regrent)
}

void CommandQueue::GPUWait(CommandQueue* queue, u32 value)
{
    ID3D12Fence* fence = reinterpret_cast<ID3D12Fence*>(queue->m_FenceNativeHandle);
    GetQueue()->Wait(fence, value);
}

void CommandQueue::CPUWait()
{
    u32 signal = Signal();
    if (GetFence()->GetCompletedValue() < signal)
    {
        GetFence()->SetEventOnCompletion(signal, m_Handle);
        WaitForSingleObject(m_Handle, INFINITE);
    }
}

CommandQueue::~CommandQueue()
{
    GetNativeHandle<ID3D12CommandQueue>()->Release();
}
