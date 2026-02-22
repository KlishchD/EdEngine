#pragma once

#include "EdCore.h"
#include "EdTelemetry.h"
#include "shaders.h"

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

  void set_source_path(const estd::path& path)
  {
    shader_source = path;
    add_include(path);
  }

  virtual void add_include(const estd::path& path) = 0;
  virtual shaders::collection compile(const shaders::description& description, compile_options options, compilation_results& results) = 0;

  virtual ~shader_compiler() = default;
protected:
  shaders::types detect_avaliable_shaders(ccstr8 code) const;
protected:
  estd::path shader_source;
};