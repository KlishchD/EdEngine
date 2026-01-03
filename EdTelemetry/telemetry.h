#pragma once

using shader_descripor = std::string;
using shaders_descriptors_list = std::unordered_set<shader_descripor>;

class telemetry
{
public:
  bool shaders_reporting_enabled() const;
  void report_shader(const std::string& descriptor);

  void dump_shaders(const Path& path);
private:
  shaders_descriptors_list shaders;
};
