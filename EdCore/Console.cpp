#include "EdCore.h"
#include "Console.h"

estd::console::console& console::get_console()
{
  static estd::console::console instance;
  return instance;
}

estd::console::float_parameter& console::create_f32(const char* name, f32 default_value, f32 min, f32 max)
{
  return get_console()
    .add_parameter<estd::console::inline_float_parameter>(name, default_value)
    .set_range(min, max);
}

estd::console::unsigned_integer_parameter& console::create_u32(const char* name, u32 default_value, u32 min, u32 max)
{
  return get_console()
    .add_parameter<estd::console::inline_unsigned_integer_parameter>(name, default_value)
    .set_range(min, max);
}

bool console::save(const estd::path& destination)
{
  ED_LOG(Engine, info, "Saving console to [{}].", destination.c_str());

  if (destination.exists())
  {
    ED_LOG(Engine, warn, "Overwriting existing config [{}].", destination.c_str());
  }

  estd::json object;

  for (const auto& [name, parameter] : get_console().get_parameters())
  {
    estd::json& value = object[name];
    bool succeeded = parameter->extract(value);

    ED_LOG(Engine, info, "Parameter[{}]: {} {}.", succeeded, name, value.dump());
    if (!succeeded) return false;
  }

  estd::write_json(destination, object);

  return true;
}

bool console::load(const estd::path& source)
{
  ED_LOG(Engine, info, "Loading console from [{}].", source.c_str());

  if (!source.exists())
  {
    ED_LOG(Engine, warn, "Config file [{}] was not found.", source.c_str());
    return false;
  }

  if (!source.is_file())
  {
    ED_LOG(Engine, warn, "Config file [{}] is not a file.", source.c_str());
    return false;
  }

  estd::json config = estd::read_json(source);

  for (const auto& item : config.items())
  {
    const char* key = item.key().c_str();
    const auto& value = item.value();

    const bool succeeded = get_console().insert(key, value);

    ED_LOG(Engine, info, "Parameter[{}]: {} {}.", succeeded, key, value.dump());
    if (!succeeded) return false;
  }

  return true;
}
