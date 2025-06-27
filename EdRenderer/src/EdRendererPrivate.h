#pragma once

#include "EdRenderer.h"

#include "EdRenderApiPrivate.h"
#include "RenderScene.h"
#include "RenderTarget.h"
#include "UAVTarget.h"
#include "Passes/RenderPass.h"
#include "RenderGraph.h"

#define LOCAL_SHADOW_MAPS_DIMENTIONS 4096
#define MAX_SHADOWED_POINT_LIGHT_COUNT 16
#define MAX_SHADOWED_SPOT_LIGHTS_COUNT 32

// In total it will be 16 maps with 512x512 each
#define MAX_POINT_LIGHTS_IN_COLUMN 1
#define MAX_SPOT_LIGHTS_IN_COLUMN 2

#define MAX_VIEWPORTS_COUNT 6

#define LOCAL_SHADOW_MAP_DIMENTIONS (LOCAL_SHADOW_MAPS_DIMENTIONS / (MAX_POINT_LIGHTS_IN_COLUMN * 6 + MAX_SPOT_LIGHTS_IN_COLUMN))
