#pragma once
#include <vector>

class GeometryBuilder
{
public:
    static std::pair<std::vector<float>, std::vector<int32_t>> MakeSphere(float radius, int32_t sectorCount, int32_t stackCount);
};
