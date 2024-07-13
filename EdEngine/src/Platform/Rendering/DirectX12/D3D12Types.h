#pragma once

#include "EdD3D12Rendering.h"

class D3D12Types
{
public:
  static std::string ConvertFeatureLevelToString(D3D_FEATURE_LEVEL level);
  static uint32_t ConvertFormatDataSize(DXGI_FORMAT format);
};
