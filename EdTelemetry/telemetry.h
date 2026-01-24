#pragma once

struct shader_data
{
  estd::stack_string_512 path;
  Array<estd::stack_string_512> arguments;

  bool operator==(const shader_data& other) const;
};

namespace std
{
  template<>
  class hash<shader_data>
  {
  public:
    std::size_t operator()(const shader_data& data) const
    {
      std::size_t result = estd::crc32_append_string(0, data.path);

      for (const auto& argument : data.arguments)
      {
        result = estd::crc32_append_string(0, argument);
      }

      return result;
    }
  };
}

class telemetry
{
public:
  bool shaders_reporting_enabled() const;
  void report_shader(const ShaderPath& path, const TemporaryArray<ccstr16>& arguments);

  void dump_shaders(const Path& path);
private:
  std::unordered_set<shader_data> shaders;
};