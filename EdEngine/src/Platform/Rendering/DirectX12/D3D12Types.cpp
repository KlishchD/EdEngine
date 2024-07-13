#include "D3D12Types.h"

std::string D3D12Types::ConvertFeatureLevelToString(D3D_FEATURE_LEVEL level)
{
  switch (level)
  {
    case D3D_FEATURE_LEVEL_1_0_CORE: return "D3D_FEATURE_LEVEL_1_0_CORE";
    case D3D_FEATURE_LEVEL_9_1:      return "D3D_FEATURE_LEVEL_9_1";
    case D3D_FEATURE_LEVEL_9_2:      return "D3D_FEATURE_LEVEL_9_2";
    case D3D_FEATURE_LEVEL_9_3:      return "D3D_FEATURE_LEVEL_9_3";
    case D3D_FEATURE_LEVEL_10_0:     return "D3D_FEATURE_LEVEL_10_0";
    case D3D_FEATURE_LEVEL_10_1:     return "D3D_FEATURE_LEVEL_10_1";
    case D3D_FEATURE_LEVEL_11_0:     return "D3D_FEATURE_LEVEL_11_0";
    case D3D_FEATURE_LEVEL_11_1:     return "D3D_FEATURE_LEVEL_11_1";
    case D3D_FEATURE_LEVEL_12_0:     return "D3D_FEATURE_LEVEL_12_0";
    case D3D_FEATURE_LEVEL_12_1:     return "D3D_FEATURE_LEVEL_12_1";
    case D3D_FEATURE_LEVEL_12_2:     return "D3D_FEATURE_LEVEL_12_2";
    default:
      ED_ASSERT(0, "There is no such feature level");
  }

  return "";
}

uint32_t D3D12Types::ConvertFormatDataSize(DXGI_FORMAT format)
{
  switch (format)
  {
  case DXGI_FORMAT_R8G8B8A8_UNORM: return 4 * sizeof(uint8_t);
  case DXGI_FORMAT_R32_FLOAT:      return sizeof(uint32_t);
  default:
    ED_ASSERT(0, "Format is not supported");
  }

  return 0;
}

