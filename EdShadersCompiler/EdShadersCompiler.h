#pragma once

#include "EdCore.h"
#include "EdTelemetry.h"

enum class shader_type : u8
{
  none = 0,
  vertex = (1 << 0),
  hull = (1 << 1),
  domain = (1 << 2),
  geometry = (1 << 3),
  pixel = (1 << 4),
  compute = (1 << 5),
  count = 6
};

inline ccstr16 get_shader_entry_wide(shader_type type)
{
  switch (type)
  {
  case shader_type::vertex: return L"VSMain";
  case shader_type::hull: return L"HSMain";
  case shader_type::domain: return L"DSMain";
  case shader_type::geometry: return L"GSMain";
  case shader_type::pixel: return L"PSMain";
  case shader_type::compute: return L"CSMain";
  default: ED_ASSERT(0, "Shader type [{}] is not supported.", static_cast<u32>(type));
  }

  return L"NoEntry";
}

inline ccstr8 get_shader_entry(shader_type type)
{
  switch (type)
  {
  case shader_type::vertex: return "VSMain";
  case shader_type::hull: return "HSMain";
  case shader_type::domain: return "DSMain";
  case shader_type::geometry: return "GSMain";
  case shader_type::pixel: return "PSMain";
  case shader_type::compute: return "CSMain";
  default: ED_ASSERT(0, "Shader type [{}] is not supported.", static_cast<u32>(type));
  }

  return "NoEntry";
}

inline ccstr8 get_shader_name(shader_type type)
{
  switch (type)
  {
  case shader_type::vertex: return "VS";
  case shader_type::hull: return "HS";
  case shader_type::domain: return "DS";
  case shader_type::geometry: return "GS";
  case shader_type::pixel: return "PS";
  case shader_type::compute: return "CS";
  }

  return "None";
}

struct shader_define
{
  ccstr8 name;
  ccstr8 value;
};

using shader_defines = Array<shader_define>;

struct shader_description
{
  ShaderPath source;
  shader_defines defines;

  void append_define(ccstr8 name, ccstr8 value)
  {
    shader_define& define = defines.Add();
    define.name = name;
    define.value = value;
  }

  void append_define(ccstr8 name, i32 value)
  {
    shader_define& define = defines.Add();
    define.name = name;

    std::string converted = std::to_string(value);
    define.value = Strings::RequestString(converted.c_str(), converted.size(), true);
  }
};

struct shader
{
  u8* bytecode;
  u64 size;

  shader() : bytecode(nullptr), size(0)
  { }

  shader(u8* bytecode, u64 size) : bytecode(bytecode), size(size)
  { }

  shader(const shader& other)
  {
    if (other.size)
    {
      size = other.size;
      bytecode = new u8[size];

      std::memcpy(bytecode, other.bytecode, size);
    }
    else
    {
      bytecode = nullptr;
      size = 0;
    }
  }

  shader(shader&& other) : bytecode(other.bytecode), size(other.size)
  {
    other.bytecode = nullptr;
    other.size = 0;
  }

  shader& operator=(const shader& other)
  {
    if (other.size)
    {
      size = other.size;
      bytecode = new u8[size];

      std::memcpy(bytecode, other.bytecode, size);
    }
    else
    {
      bytecode = nullptr;
      size = 0;
    }

    return *this;
  }

  shader& operator=(shader&& other)
  {
    bytecode = other.bytecode;
    size = other.size;

    other.bytecode = nullptr;
    other.size = 0;

    return *this;
  }

  ~shader()
  {
    if (bytecode)
    {
      delete bytecode;
      bytecode = nullptr;
    }

    size = 0;
  }
};

struct shader_collection
{
  shader vertex;
  shader hull;
  shader domain;
  shader geometry;
  shader pixel;
  shader compute;

  shader_collection()
  { }

  shader_collection(const shader_collection& other)
    : vertex(other.vertex),
    hull(other.hull),
    domain(other.domain),
    geometry(other.geometry),
    pixel(other.pixel),
    compute(other.compute)
  { }

  shader_collection(shader_collection&& other) noexcept
    : vertex(std::move(other.vertex)),
    hull(std::move(other.hull)),
    domain(std::move(other.domain)),
    geometry(std::move(other.geometry)),
    pixel(std::move(other.pixel)),
    compute(std::move(other.compute))
  { }

  shader_collection& operator=(const shader_collection& other)
  {
    vertex = other.vertex;
    hull = other.hull;
    domain = other.domain;
    geometry = other.geometry;
    pixel = other.pixel;
    compute = other.compute;
    return *this;
  }

  shader_collection& operator=(shader_collection&& other)
  {
    vertex = std::move(other.vertex);
    hull = std::move(other.hull);
    domain = std::move(other.domain);
    geometry = std::move(other.geometry);
    pixel = std::move(other.pixel);
    compute = std::move(other.compute);
    return *this;
  }

  shader& get(shader_type type)
  {
    switch (type)
    {
    case shader_type::vertex: return vertex;
    case shader_type::hull: return hull;
    case shader_type::domain: return domain;
    case shader_type::geometry: return geometry;
    case shader_type::pixel: return pixel;
    case shader_type::compute: return compute;
    default: ED_ASSERT(0, "Can not retrive a shader type [{}].", static_cast<u32>(type));
    }

    return vertex;
  }

  void set_copy(shader_type type, void* bytecode, u64 size)
  {
    shader& result = get(type);
    result.size = size;

    result.bytecode = new u8[size];
    std::memcpy(result.bytecode, bytecode, size);
  }
};

struct compilation_results
{
  Array<ccstr8> markers;
  cstr8 buffer;
  u32 offset;
  u32 size;

  compilation_results(u32 size)
    : buffer(Strings::RequestString(size, false, true)),
    offset(0),
    size(size)
  { }

  compilation_results(const compilation_results&) = delete;
  compilation_results(compilation_results&&) = delete;

  compilation_results& operator=(const compilation_results&) = delete;
  compilation_results& operator=(compilation_results&&) = delete;

  bool empty() { return offset == 0; }

  void append(ccstr8 message, u32 message_size)
  {
    ED_ASSERT(offset + message_size < size, "Compilation results capacity reached [{}] > [{}].", offset + message_size, size);
    markers.Add(buffer);

    std::memcpy(buffer + offset, message, message_size);
    offset += message_size + 1;
  }

  void clear()
  {
    markers.Clear();
    offset = 0;
  }

  ~compilation_results()
  {
    clear();
    //delete buffer;
  }
};

class shader_compiler
{
public:
  enum compile_options
  {
    debug,
  };

  virtual void add_include(const Path& path) = 0;
  virtual shader_collection compile(const shader_description& description, compile_options options, compilation_results& results) = 0;

  virtual ~shader_compiler() = default;
protected:
  shader_type detect_avaliable_shaders(ccstr8 code) const;
};