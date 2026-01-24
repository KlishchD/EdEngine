#pragma once

#pragma comment(lib, "glm.lib")
#pragma comment(lib, "spdlog.lib")

#if !defined(BUILD_TYPE)
#   error Build type was not defined
#endif

#if BUILD_TYPE < 1 && BUILD_TYPE > 3
#   error Engine supports only 3 build types
#endif

#if BUILD_TYPE == 1
#   define DEBUG_BUILD 1
#   define DEVELOPMENT_BUILD 0
#   define RELEASE_BUILD 0
#elif BUILD_TYPE == 2
#   define DEBUG_BUILD 0
#   define DEVELOPMENT_BUILD 1
#   define RELEASE_BUILD 0
#else
#   define DEBUG_BUILD 0
#   define DEVELOPMENT_BUILD 0
#   define RELEASE_BUILD 1
#endif

//TODO: Add separation for Game and Editor

#define ED_EDITOR 1

#ifndef NOMINMAX
    #define NOMINMAX
#endif

#include <memory>
#include <widemath.h>
#include <cstdint>
#include <string>
#include <locale.h>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <filesystem>
#include <queue>
#include <random>
#include <tuple>
#include <iterator>
#include <cstddef>
#include <thread>

typedef void* HANDLE;

typedef int8_t i8;
typedef uint8_t u8;

typedef int16_t i16;
typedef uint16_t u16;

typedef int32_t i32;
typedef uint32_t u32;

typedef int64_t i64;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef intptr_t iptr;
typedef uintptr_t uptr;

typedef char c8;
typedef wchar_t c16;

typedef unsigned char uc8;

using cstr8 = c8*;
using cstr16 = c16*;
using ccstr8 = const c8*;
using ccstr16 = const c16*;

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "glm/mat4x4.hpp"
#include "glm/fwd.hpp"
#include "glm/detail/type_quat.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/matrix_decompose.hpp"

namespace glm
{
    inline vec3 forward = vec3(0.0f, 0.0f, 1.0f);
    inline vec3 up = vec3(0.0f, 1.0f, 0.0f);
    inline vec3 right = vec3(1.0f, 0.0f, 0.0f);

    inline vec3 globalDirections[3] = { forward, up, right };
};

#include "LogManager.h"
#include "Macros.h"
#include "EdTime.h"

#include "Memory.h"
#include "FreeListAllocator.h"
#include "BitMask.h"
#include "PoolAllocator.h"

FREE_LIST_ALLOCATOR(DataStructuresAllocator, 2 * Memory::BytesToGB)
FREE_LIST_ALLOCATOR(AssetDataAllocator, 2 * Memory::BytesToGB)
FREE_LIST_ALLOCATOR(SerializationDataAllocator, 0.5 * Memory::BytesToGB)

template <typename A, typename B>
struct Pair
{
    A First;
    B Second;
};

#include "Archive.h"

// TODO: Bring it back after allocator system is made more robust.
using AssetArchive = Archive<>;

#include "Collections/Array.h"
#include "File.h"

#include "Class.h"
#include "GameObject.h"
#include "GameObjectFactory.h"

#include "InputManager.h"

#undef far
#undef near

class Engine;

#include "Math/Transform.h"
#include "Math/Camera.h"

constexpr inline f64 pi()
{
    return 3.1415926535897932384626433832795;
}

#include "Strings.h"

#pragma message("I don't really like that I need to include them for some to work (even though I need them all for my engine).")
#include "estd/exceptions.h"
#include "estd/strings.h"
#include "estd/shell.h"
#include "estd/threading.h"
#include "estd/memory.h"
#include "estd/json.h"
#include "estd/hasing.h"
#include "estd/platform.h"