#include "EdDirectX12.h"

//#pragma warning "dfsf"

//kai;

CommandAllocator::CommandAllocator(CommandListType type)
{
    D3D::Check(g_Device->CreateCommandAllocator(DirectX12Types::ConvertCommandListType(type), __uuidof(ID3D12CommandAllocator), reinterpret_cast<void**>(&m_NativeHandle)));
}

NAME_METHODS_TEMPLATE(CommandAllocator, ID3D12CommandAllocator)

void CommandAllocator::Reset()
{
    D3D::Check(GetNativeHandle<ID3D12CommandAllocator>()->Reset());
}

CommandAllocator::~CommandAllocator()
{
    GetNativeHandle<ID3D12CommandAllocator>()->Release();
}
