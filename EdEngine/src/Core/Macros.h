#pragma once

#include "Core/LogManager.h"


#define ED_LOG(context, status, format, ...) LogManager::Get().GetLogger()->log(spdlog::source_loc(#context, __LINE__, __FILE__), spdlog::level::level_enum::status, format, __VA_ARGS__);
#define ED_ASSERT(condition, format, ...) if (!(condition)) { ED_LOG(Asserts, err, format, __VA_ARGS__) __debugbreak(); }
