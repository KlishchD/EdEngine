#include "DirectXShaderCompiler.h"

ccstr16 get_shader_target(shader_type type)
{
  switch (type)
  {
  case shader_type::vertex: return L"vs_6_6";
  case shader_type::hull: return L"gs_6_6";
  case shader_type::domain: return L"ds_6_6";
  case shader_type::geometry: return L"gs_6_6";
  case shader_type::pixel: return L"ps_6_6";
  case shader_type::compute: return L"cs_6_6";
  case shader_type::count:
  default: ED_ASSERT(0, "Shader type [{}] has no target.", static_cast<u32>(type));
  }

  return L"None";
}

void check(HRESULT result)
{
  ED_ASSERT(SUCCEEDED(result), "D3D12 error: {}", std::system_category().message(result));
}

directx_shader_compiler::directx_shader_compiler()
{
    check(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils)));
    check(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler)));
    check(utils->CreateDefaultIncludeHandler(&handler));
}

void directx_shader_compiler::add_include(const Path& path)
{
  includes.Add(path);
}

shader_collection directx_shader_compiler::compile(const shader_description& description, compile_options options, compilation_results& results)
{
  shader_collection collection;

  std::string source;

  {
    std::fstream file(description.source.Get());
    std::string line;
    while (std::getline(file, line))
    {
      source.push_back('\n');
      source.append(line);
    }
  }

  u32 types_mask = static_cast<u32>(detect_avaliable_shaders(source.c_str()));

  ED_LOG(directx_shader_compiler, info, "[{}] - {}", description.source.Get(), types_mask);

  DxcBuffer buffer;
  buffer.Ptr = source.data();
  buffer.Size = source.size();
  buffer.Encoding = DXC_CP_UTF8;

  const bool debug_enabled = static_cast<u32>(options) & static_cast<u32>(compile_options::debug);

  Microsoft::WRL::ComPtr<IDxcResult> result;
  Microsoft::WRL::ComPtr<IDxcBlob> blob;
  Microsoft::WRL::ComPtr<IDxcBlobUtf8> errors;

  constexpr u32 types_count = static_cast<u32>(shader_type::count);
  for (u32 index = 0; index < types_count; ++index)
  {
    const u32 type_mask = 1 << index;
    const bool matches = types_mask & type_mask;
    if (!matches) continue;

    const shader_type type = static_cast<shader_type>(type_mask);

    ccstr16 entrypoint = get_shader_entry_wide(type);
    ccstr16 target = get_shader_target(type);

    TemporaryArray<ccstr16> arguments;

    arguments.Add(L"-E");
    arguments.Add(entrypoint);

    arguments.Add(L"-T");
    arguments.Add(target);

    arguments.Add(DXC_ARG_PACK_MATRIX_COLUMN_MAJOR);

    if (debug_enabled)
    {
      arguments.Add(DXC_ARG_WARNINGS_ARE_ERRORS);
      arguments.Add(DXC_ARG_ALL_RESOURCES_BOUND);
      arguments.Add(DXC_ARG_SKIP_OPTIMIZATIONS);
      arguments.Add(DXC_ARG_DEBUG);
      arguments.Add(DXC_ARG_DEBUG_NAME_FOR_SOURCE);
      arguments.Add(L"-Qembed_debug");
    }
    else
    {
      arguments.Add(DXC_ARG_OPTIMIZATION_LEVEL3);
    }

    for (const auto& path : includes)
    {
      arguments.Add(L"-I");
      arguments.Add(Strings::Convert(path.Get(), true));
    }

    for (const auto& define : description.defines)
    {
      arguments.Add(L"-D");

      cstr8 combined = Strings::RequestString(128, true);
      Strings::Concat(combined, define.name, "=", define.value);

      ccstr16 converted = Strings::Convert(combined, true);
      arguments.Add(converted);
    }

    check(compiler->Compile(&buffer, arguments.Get(), arguments.GetSize(), handler.Get(), IID_PPV_ARGS(&result)));
    check(result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr));

    if (errors && errors->GetStringLength())
    {
      results.append(errors->GetStringPointer(), errors->GetStringLength());
      ED_LOG(directx_shader_compiler, err, "{}", results.markers.GetLast());

      continue;
    }

    check(result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&blob), nullptr));
    collection.set_copy(type, blob->GetBufferPointer(), blob->GetBufferSize());

    ED_LOG(directx_shader_compiler, info, "Compiled {} of {}.", get_shader_name(type), description.source.Get());

    if (g_telemetry.shaders_reporting_enabled())
    {
      g_telemetry.report_shader(description.source, arguments);
    }
  }

  return collection;
}
