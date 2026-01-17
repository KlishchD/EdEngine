#include "SSAO.h"

[numthreads(SSAO_TILE_SIZE, SSAO_TILE_SIZE, 1)]
void CSMain(uint2 screenPosition : SV_DispatchThreadID)
{
    if (all(screenPosition < ScreenSize))
    {
        float AO = 0;

        // Stupid simple blur, may cause artifacts, so better one should be selected.
        [unroll]
        for (int i = -SSAO_BLUR_FILTER_HALF_SIZE; i <= SSAO_BLUR_FILTER_HALF_SIZE; ++i)
        {
            [unroll]
            for (int j = -SSAO_BLUR_FILTER_HALF_SIZE; j <= SSAO_BLUR_FILTER_HALF_SIZE; ++j)
            {
                AO += CurrentAO[screenPosition + uint2(i, j)].r;
            }
        }

        uint filterSize = 2 * SSAO_BLUR_FILTER_HALF_SIZE + 1;
        HistoryAO[screenPosition] = AO / (filterSize * filterSize);
    }
}