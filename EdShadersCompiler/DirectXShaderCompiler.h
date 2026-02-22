#pragma once

#include "EdShadersCompiler.h"

#pragma comment(lib, "dxcompiler.lib")

#include "windows.h"
#include "dxcapi.h"
#include "wrl/client.h"

class directx_shader_compiler : public shader_compiler
{
public:
  using inherited = shader_compiler;

  directx_shader_compiler();

  virtual void add_include(const estd::path& path) override;
  virtual shader_collection compile(const shader_description& description, compile_options options, compilation_results& results) override;
private:
  Microsoft::WRL::ComPtr<IDxcUtils> utils;
  Microsoft::WRL::ComPtr<IDxcCompiler3> compiler;
  Microsoft::WRL::ComPtr<IDxcIncludeHandler> handler;
  Array<estd::path> includes;
};
