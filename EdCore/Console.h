#pragma once

// Current system is fun but it is not ready because it is difficult to debug.
// Debugging solution implemented or system reinvented.
namespace console
{
  using console = estd::console::console;

  console& get_console();

  estd::console::path_parameter& create_path(const char* name, const char* default_value, bool exists, bool directory, bool file);
  estd::console::path_parameter& create_mandatory_path(const char* name, const char* default_value, bool exists, bool directory, bool file);

  estd::console::float_parameter& create_f32(const char* name, f32 default_value, f32 min, f32 max);
  estd::console::unsigned_integer_parameter& create_u32(const char* name, u32 default_value, u32 min, u32 max);
  estd::console::bool_parameter& create_bool(const char* name, bool default_value);

  bool save(const estd::path& destination);
  bool load(const estd::path& source);
};