#pragma once

#pragma comment(lib, "glfw.lib")

#include "EdCore.h"
#include "RenderTypes.h"

#define ALLOW_PSO_RECREATION 1

#ifndef ALLOW_PSO_RECREATION
    #define ALLOW_PSO_RECREATION 0
#endif 

#if RELEASE_BUILD == 1 
    #undef ALLOW_PSO_RECREATION
    #define ALLOW_PSO_RECREATION 0
#endif

#if ALLOW_PSO_RECREATION == 1
    FREE_LIST_ALLOCATOR(PSODescriptionsAllocator, Memory::BytesToMB)
#endif 

class RenderingContext;

class Resource;

struct ResourceView
{
    Resource* Viewed = nullptr;
    u64 CPUHandle = 0;
    u64 GPUHandle = 0;

    u32 Size = 0;
    DescriptorHeapType Type = DescriptorHeapType::Count;
    u8 Uploaded = 0;

    operator bool() const { return Viewed; }
    bool IsResourceValid() const { return Viewed; }

    void* GPUHandlePtr() const { return reinterpret_cast<void*>(GPUHandle); }
};

inline MemoryDeallocationPredicate MakeResourceUploadedPredicate(ResourceView* view)
{
    return [view]() -> bool
        {
            return view->Uploaded != 0;
        };
}

// TODO: ADD BACK DEBUG NAMES FOR DEBUG AND DEVELOPMENT, THIS WILL REALLY HELP
#define DEBUG_NAME_PARAMETER , const std::string& name 