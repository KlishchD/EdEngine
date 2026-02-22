#include "EdTelemetry.h"

static auto& t_shaders_reporting_path = console::create_path("t_shaders_reporting_path", "Resources\\Reports\\shaders.json", false, false, true).set_mandatory(ED_TELEMETRY == 1);

bool shader_data::operator==(const shader_data& other) const
{
  if (path != other.path) return false;

  const bool arguments_count_match = arguments.GetSize() == other.arguments.GetSize();
  if (!arguments_count_match) return false;

  for (std::size_t index{ 0 }; index < other.arguments.GetSize(); ++index)
  {
    const auto& argument = arguments[index];
    const auto& other_argument = other.arguments[index];
    if (argument != other_argument) return false;
  }

  return true;
}

bool telemetry::shaders_reporting_enabled() const
{
  return ED_TELEMETRY == 1;
}

void telemetry::report_shader(const estd::path& path, const TemporaryArray<ccstr16>& arguments)
{
#if ED_TELEMETRY == 1
  shader_data shader;
  shader.path = path;

  for (const auto& argument : arguments)
  {
    shader.arguments.Add(Strings::Convert(argument, true));
  }

  const bool is_duplcate = shaders.contains(shader);
  if (is_duplcate) return;

  shaders.insert(std::move(shader));
  ED_LOG(telemetry, info, "Reported [{}] {} - {}.", shaders.size(), path.c_str(), shader.arguments.GetSize());
#endif
}

void telemetry::report_shaders()
{
  report_shaders_custom(t_shaders_reporting_path());
}

void telemetry::report_shaders_custom(const estd::path& path)
{
#if ED_TELEMETRY == 1
  if (path.exists())
  {
    ED_LOG(telemetry, info, "Previous shaders report detected, cleaning up.");
    path.remove();
  }

  ED_LOG(telemetry, info, "Gathering shaders to dump in [{}].", path.c_str());

  estd::json result;

  for (const auto& shader : shaders)
  {
    estd::json local_result;
    local_result["Name"] = shader.path.c_str();
    for (const auto& argument : shader.arguments)
    {
      local_result["Arguments"].push_back(argument);
    }

    result.push_back(std::move(local_result));
  }

  estd::write_json(path, result);
#endif
}