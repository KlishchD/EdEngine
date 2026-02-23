#include "Core.h"
#include "b_visualization.h"

#define GROUP_SIZE 16

cbuffer Parameters : register(b0, space1)
{
  uint mode;
  uint texture_target0;
  uint texture_target1;
  uint texture_target2;
};

RWTexture2D<float4> backbuffer : register(u0);

[numthreads(GROUP_SIZE, GROUP_SIZE, 1)]
void CSMain(uint3 thread_id : SV_DispatchThreadID)
{
  uint2 location = thread_id.xy;

  [branch]
  if (mode == visualization::none) return;

  float4 result = 1.0f;

  switch (mode)
  {
  case visualization::gbuffer_albedo:
  {
    Texture2D<float4> source = GetTextureFloat4(texture_target0);

    result.rgb = source[location].rgb;

    break;
  }
  case visualization::gbuffer_normals:
  {
    Texture2D<float4> source = GetTextureFloat4(texture_target0);

    float3 normals = source[location].rgb;
    result.rgb = pow(normals, 1.0f / 2.2);

    break;
  }
  }

  backbuffer[location] = result;
}