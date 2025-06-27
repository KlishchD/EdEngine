// Sources for improvement
// https://shuangz.com/projects/nmfilter-egsr17/nmfilter-main-ld.pdf
// https://kosmonautblog.wordpress.com/2018/09/17/roughness-mip-maps-based-on-normal-maps/
// https://developer.download.nvidia.com/whitepapers/2006/Mipmapping_Normal_Maps.pdf

#ifndef TILE_SIZE
    #define TILE_SIZE 32
#endif

#define ALBEDO_DOWNSCALING 0
#define NORMAL_DOWNSCALING 1
#define MATERIAL_DOWNSCALING 2

#ifndef DOWSCALING_MODE
    #error DOWSCALING_MODE must be supplied
#endif

#ifndef MIP_COUNT
    #define MIP_COUNT 5
#endif 

Texture2D<float4> Input : register(t0);
RWTexture2D<float4> Mip[5] : register(u3);

groupshared float4 mip[33][33];

float4 Downscale(uint2 location)
{
    float4 a00 = mip[location.x * 2    ][location.y * 2    ];
    float4 a10 = mip[location.x * 2 + 1][location.y * 2    ];
    float4 a01 = mip[location.x * 2    ][location.y * 2 + 1];
    float4 a11 = mip[location.x * 2 + 1][location.y * 2 + 1];

    float4 result = (a00 + a11 + a01 + a11) / 4.0f;

    #if DOWSCALING_MODE == NORMAL_DOWNSCALING
        result.xyz = normalize(result.xyz);
    #endif

    return result;
}

float4 Load(uint2 location)
{
    float4 sample = Input[location];
//#if DOWSCALING_MODE == ALBEDO_DOWNSCALING
//    sample.rgb *= sample.a;
//#endif

    return sample;
}

[numthreads(TILE_SIZE, TILE_SIZE, 1)]
void CSMain(uint2 local : SV_GroupThreadID, uint2 group : SV_GroupID)
{
    uint2 origin = group * TILE_SIZE + local;

    // TODO: Undefined behavior for targets with size non-divisible by 32.
    mip[local.x][local.y] = Load(origin);

    [branch]
    if (all(local == TILE_SIZE))
    {
        mip[local.x + 1][local.y    ] = Load(origin + uint2(1, 0));
        mip[local.x    ][local.y + 1] = Load(origin + uint2(0, 1));
        mip[local.x + 1][local.y + 1] = Load(origin + uint2(1, 1));
    }
    else
    {
        [branch]
        if (local.x == TILE_SIZE)
        {
            mip[local.x + 1][local.y] = Load(origin + uint2(1, 0));
        }
        else
        {
            [branch]
            if (local.y == TILE_SIZE)
            {
                mip[local.x][local.y + 1] = Load(origin + uint2(0, 1));
            }
        }
    }

    GroupMemoryBarrierWithGroupSync();

    [branch]
    if (any(local.xy * 2 >= TILE_SIZE)) return;
    Mip[0][group * (TILE_SIZE / 2) + local] = Downscale(local);

#if MIP_COUNT > 1
    uint tile = TILE_SIZE;

    [unroll(MIP_COUNT - 1)]
    for (uint i = 1; i < MIP_COUNT; ++i)
    {
        tile >>= 1;
        origin = group * tile + local;

        mip[local.x][local.y] = Mip[i - 1][origin];

        [branch]
        if (all(local == tile))
        {
            mip[local.x + 1][local.y    ] = Mip[i - 1][origin + uint2(1, 0)];
            mip[local.x    ][local.y + 1] = Mip[i - 1][origin + uint2(0, 1)];
            mip[local.x + 1][local.y + 1] = Mip[i - 1][origin + uint2(1, 1)];
        }
        else
        {
            [branch]
            if (local.x == tile)
            {
                mip[local.x + 1][local.y] = Mip[i - 1][origin + uint2(1, 0)];
            }
            else
            {
                [branch]
                if (local.y == tile)
                {
                    mip[local.x][local.y + 1] = Mip[i - 1][origin + uint2(0, 1)];
                }
            }
        }

        GroupMemoryBarrierWithGroupSync();

        [branch]
        if (any(local.xy * 2 >= tile)) return;
        Mip[i][group * (tile / 2) + local] = Downscale(local);
    }
#endif
}