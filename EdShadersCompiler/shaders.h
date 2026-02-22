#pragma once

namespace shaders
{
  enum types : u8
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

  inline ccstr16 get_entry_wide(types type)
  {
    switch (type)
    {
    case vertex: return L"VSMain";
    case hull: return L"HSMain";
    case domain: return L"DSMain";
    case geometry: return L"GSMain";
    case pixel: return L"PSMain";
    case compute: return L"CSMain";
    default: ED_ASSERT(0, "Shader type [{}] is not supported.", static_cast<u32>(type));
    }

    return L"NoEntry";
  }

  inline ccstr8 get_entry(types type)
  {
    switch (type)
    {
    case vertex: return "VSMain";
    case hull: return "HSMain";
    case domain: return "DSMain";
    case geometry: return "GSMain";
    case pixel: return "PSMain";
    case compute: return "CSMain";
    default: ED_ASSERT(0, "Shader type [{}] is not supported.", static_cast<u32>(type));
    }

    return "NoEntry";
  }

  inline ccstr8 get_name(types type)
  {
    switch (type)
    {
    case vertex: return "VS";
    case hull: return "HS";
    case domain: return "DS";
    case geometry: return "GS";
    case pixel: return "PS";
    case compute: return "CS";
    }

    return "None";
  }

  inline estd::stack_string_128 get_preaty_names(types combined_type)
  {
    estd::stack_string_128 result;

    const auto mask = static_cast<u32>(combined_type);
    for (u32 index = none; index < count; ++index)
    {
      const auto type = static_cast<types>(1 << index);
      const bool matches = mask & type;
      if (!matches) continue;

      if (result.size())
      {
        result.append(" | ");
      }

      result.append(get_name(type));
    }

    if (result.empty())
    {
      result.append("None");
    }

    return result;
  }

  struct define_description
  {
    ccstr8 name;
    ccstr8 value;
  };

  using defines_list = Array<define_description>;

  struct description
  {
    estd::path source;
    defines_list defines;

    void append_define(ccstr8 name, ccstr8 value)
    {
      auto& define = defines.Add();
      define.name = name;
      define.value = value;
    }

    void append_define(ccstr8 name, i32 value)
    {
      auto& define = defines.Add();
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
    {
    }

    shader(u8* bytecode, u64 size) : bytecode(bytecode), size(size)
    {
    }

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
      if (size)
      {
        delete bytecode;
        size = 0;
      }

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
      if (size)
      {
        delete bytecode;
        size = 0;
      }

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

  struct collection
  {
    shader vertex;
    shader hull;
    shader domain;
    shader geometry;
    shader pixel;
    shader compute;

    collection()
    {
    }

    collection(const collection& other)
      : vertex(other.vertex),
      hull(other.hull),
      domain(other.domain),
      geometry(other.geometry),
      pixel(other.pixel),
      compute(other.compute)
    {
    }

    collection(collection&& other) noexcept
      : vertex(std::move(other.vertex)),
      hull(std::move(other.hull)),
      domain(std::move(other.domain)),
      geometry(std::move(other.geometry)),
      pixel(std::move(other.pixel)),
      compute(std::move(other.compute))
    {
    }

    collection& operator=(const collection& other)
    {
      vertex = other.vertex;
      hull = other.hull;
      domain = other.domain;
      geometry = other.geometry;
      pixel = other.pixel;
      compute = other.compute;
      return *this;
    }

    collection& operator=(collection&& other)
    {
      vertex = std::move(other.vertex);
      hull = std::move(other.hull);
      domain = std::move(other.domain);
      geometry = std::move(other.geometry);
      pixel = std::move(other.pixel);
      compute = std::move(other.compute);
      return *this;
    }

    shader& get(types type)
    {
      switch (type)
      {
      case types::vertex: return vertex;
      case types::hull: return hull;
      case types::domain: return domain;
      case types::geometry: return geometry;
      case types::pixel: return pixel;
      case types::compute: return compute;
      default: ED_ASSERT(0, "Can not retrive a shader type [{}].", static_cast<u32>(type));
      }

      return vertex;
    }

    void set_copy(types type, void* bytecode, u64 size)
    {
      shader& result = get(type);
      result.size = size;

      result.bytecode = new u8[size];
      std::memcpy(result.bytecode, bytecode, size);
    }
  };
}