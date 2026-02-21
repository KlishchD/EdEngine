#pragma once

namespace formats
{
  static bool is_depth_format(PixelFormat format)
  {
    return format == PixelFormat::Depth || format == PixelFormat::DepthStencil;
  }
}