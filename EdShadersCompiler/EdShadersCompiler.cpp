#include "EdShadersCompiler.h"

shaders::types shader_compiler::detect_avaliable_shaders(ccstr8 code) const
{
  // Could become expensive one time and will may need improvement ;)

  u32 result = 0;
  for (u32 index = shaders::none; index < shaders::count; ++index)
  {
    const auto type = static_cast<shaders::types>(1 << index);
    const auto mask = 1 << index;

    ccstr8 entry = shaders::get_entry(type);
    const bool contains = std::strstr(code, entry);

    result |= mask * contains;
  }

  constexpr auto graphics_mask = static_cast<u32>(shaders::vertex) | static_cast<u32>(shaders::pixel);
  constexpr auto compute_mask = static_cast<u32>(shaders::compute);
  ED_ASSERT(bool(result & graphics_mask) ^ bool(result & compute_mask), "Shader file has incompetable entry points.");

  return static_cast<shaders::types>(result);
}
