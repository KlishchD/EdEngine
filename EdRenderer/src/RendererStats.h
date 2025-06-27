#pragma once

#include "EdRenderer.h"

struct RenderStats
{
    u32 StaticMeshesDrawn = 0;

    u32 PointLightsShaded = 0;
    u32 PointLightsShadowsDrawn = 0;

    u32 SpotLightsShaded = 0;
    u32 SpotLightsShadowsDrawn = 0;

    u32 DirectionalLightsShaded = 0;
    u32 DirectionalLightsShadowsDrawn = 0;

    void Reset()
    {
        StaticMeshesDrawn = 0;

        PointLightsShaded = 0;
        PointLightsShadowsDrawn = 0;

        SpotLightsShaded = 0;
        SpotLightsShadowsDrawn = 0;

        DirectionalLightsShaded = 0;
        DirectionalLightsShadowsDrawn = 0;
    }
};