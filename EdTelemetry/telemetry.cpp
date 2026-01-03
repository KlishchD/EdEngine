#include "EdTelemetry.h"

bool telemetry::shaders_reporting_enabled() const
{
  return ED_TELEMETRY == 1;
}

void telemetry::report_shader(const std::string& descriptor)
{
#if ED_TELEMETRY == 1
  shaders.insert(descriptor);
#endif
}

void telemetry::dump_shaders(const Path& path)
{
#if ED_TELEMETRY == 1
  ED_LOG(Telemetry, info, "Dumping shaders [{}] to [{}].", shaders.size(), path.Get());

  std::ofstream file(path.Get(), std::ios_base::out);

  for (const auto& descriptor : shaders)
  {
    file << descriptor << "\n";
  }
#endif
}