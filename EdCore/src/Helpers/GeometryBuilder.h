#pragma once

class GeometryBuilder
{
public:
    static void MakeSphere(f32 radius, i32 sectorCount, i32 stackCount, TemporaryArray<glm::vec3>& vertices, TemporaryArray<u32>& indices);
    static void MakeCone(i32 sectorCount, TemporaryArray<glm::vec3>& vertices, TemporaryArray<u32>& indices);
};
