#pragma once
#include <vector>
#include <glm/fwd.hpp>

class GeometryBuilder
{
public:
    static std::pair<std::vector<glm::vec3>, std::vector<int32_t>> MakeSphere(float radius, int32_t sectorCount, int32_t stackCount);
    static std::pair<std::vector<glm::vec3>, std::vector<int32_t>> MakeCone(int32_t sectorCount);
};
