#pragma once

#include "EdRenderer.h"

#if DEBUG_BUILD || DEVELOPMENT_BUILD
    #pragma comment(lib, "assimp-vc143-mtd.lib")
#else
    #pragma comment(lib, "assimp-vc143-mt.lib")
#endif

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Helpers/stb_image.h"

#include "Widget.h"

#include "Assets/Asset.h"
#include "Assets/StaticMeshAsset.h"
#include "Assets/MaterialAsset.h"
#include "Assets/TextureAsset.h"
#include "Assets/PrefabAsset.h"
#include "Assets/AssetManager.h"

#include "Entity/Entity.h"
#include "Entity/Features.h"
#include "Entity/EntityManager.h"

#include "Engine.h"

inline MemoryDeallocationPredicate MakeFramePredicate(u32 count)
{
    return [count, creation = Engine::Get().GetFrame()]() -> bool {
        u32 current = Engine::Get().GetFrame();
        return creation - current > count;
    };
}
