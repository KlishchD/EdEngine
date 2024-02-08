#include "GeometryBuilder.h"
#include <glm/ext/vector_float3.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/scalar_constants.hpp>

#define PI 3.1415926535897932384626433832795

std::pair<std::vector<glm::vec3>, std::vector<int32_t>> GeometryBuilder::MakeSphere(float radius, int32_t sectorCount, int32_t stackCount)
{
    // origin: https://www.songho.ca/opengl/gl_sphere.html
    
    std::vector<glm::vec3> vertices;
    std::vector<int32_t> indices;
    
    float x, y, z, xy;                              // vertex position

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    for(int i = 0; i <= stackCount; ++i)
    {
        stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // first and last vertices have same position and normal, but different tex coords
        for(int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            vertices.emplace_back(x, y, z);
        }
    }
    
    // generate CCW index list of sphere triangles
    // k1--k1+1
    // |  / |
    // | /  |
    // k2--k2+1
    int k1, k2;
    for(int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if(i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if(i != (stackCount-1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
    
    return std::make_pair(vertices, indices);
}

std::pair<std::vector<glm::vec3>, std::vector<int32_t>> GeometryBuilder::MakeCone(int32_t sectorCount)
{
    std::vector<glm::vec3> vertices;
    std::vector<int32_t> indices;

    for (int32_t i = 0; i < sectorCount; ++i)
    {
        float fraction = 1.0f * i / sectorCount;
        float angle = fraction * 2.0f * glm::pi<float>();

        vertices.emplace_back(glm::cos(angle), -1.0f, glm::sin(angle));
    }

    vertices.emplace_back(0.0f);

	vertices.emplace_back(0.0f, -1.0f, 0.0f);

    for (int32_t i = 0; i < sectorCount; ++i)
    {
        indices.push_back((i + 1) % sectorCount);
        indices.push_back(i);
        indices.push_back(sectorCount);
    }

	for (int32_t i = 0; i < sectorCount; ++i)
	{
		indices.push_back(i);
		indices.push_back((i + 1) % sectorCount);
		indices.push_back(sectorCount + 1);
	}

    return std::make_pair(vertices, indices);
}
