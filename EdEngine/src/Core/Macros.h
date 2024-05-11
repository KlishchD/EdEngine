#pragma once

#include "Core/LogManager.h"

#define ED_LOG(context, status, ...) LogManager::Get().GetLogger()->log(spdlog::source_loc(#context, __LINE__, __FILE__), spdlog::level::level_enum::status, __VA_ARGS__);
#define ED_ASSERT(condition, ...) if (!(condition)) { ED_LOG(Asserts, err, __VA_ARGS__) __debugbreak(); }
#define ED_ASSERT_CONTEXT(context, condition, ...) if (!condition) { ED_LOG(context, err, __VA_ARGS__) __debugbreak(); }