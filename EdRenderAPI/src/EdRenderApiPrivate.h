#pragma once

#include "EdRenderApi.h"

#ifndef NOMINMAX
#define NOMINMAX  // prevent windows redefining min/max
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "Window.h"

#include "Device.h"
#include "Resource.h"
#include "Heap.h"
#include "DescriptorHeap.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "RootSignature.h"
#include "PipelineStateObject.h"
#include "CommandAllocator.h"
#include "CommandList.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "RenderingContext.h"
