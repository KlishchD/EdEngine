#include "EdShadersCompiler.h"

shader_type shader_compiler::detect_avaliable_shaders(ccstr8 code) const
{
  // Could become expensive one time and will may need improvement ;)

  u32 result = 0;

  constexpr u32 count = static_cast<u32>(shader_type::count);
  for (u32 index = 0; index < count; ++index)
  {
    const auto type = static_cast<shader_type>(1 << index);
    const auto mask = 1 << index;

    ccstr8 entry = get_shader_entry(type);
    const bool contains = std::strstr(code, entry);

    result |= mask * contains;
  }

  constexpr u32 graphics_mask = static_cast<u32>(shader_type::vertex) | static_cast<u32>(shader_type::pixel);
  constexpr u32 compute_mask = static_cast<u32>(shader_type::compute);
  ED_ASSERT(bool(result & graphics_mask) ^ bool(result & compute_mask), "Shader file has incompetable entry points.");

  return static_cast<shader_type>(result);
}
