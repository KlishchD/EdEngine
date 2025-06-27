#include "EdEngine.h"
#include "GeometryBuilder.h"

void GeometryBuilder::MakeSphere(f32 radius, i32 sectorCount, i32 stackCount, TemporaryArray<glm::vec3>& vertices, TemporaryArray<u32>& indices)
{
    // origin: https://www.songho.ca/opengl/gl_sphere.html
    
    f32 x, y, z, xy;                              // vertex position

    f32 sectorStep = 2.0f * f32(pi()) / sectorCount;
    f32 stackStep = f32(pi()) / stackCount;
    f32 sectorAngle, stackAngle;

    for(i32 i = 0; i <= stackCount; ++i)
    {
        stackAngle = f32(pi()) / 2.0f - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // first and last vertices have same position and normal, but different tex coords
        for(i32 j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            vertices.Add(glm::vec3(x, y, z));
        }
    }
    
    // generate CW index list of sphere triangles
    // k1--k1+1
    // |  / |
    // | /  |
    // k2--k2+1
    i32 k1, k2;
    for(i32 i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for(i32 j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 + 1 => k2 => k1
            if(i != 0)
            {
                indices.Add(k1 + 1);
                indices.Add(k1);
                indices.Add(k2);
            }

            // k1 + 1 => k2 + 1 => k2
            if(i != (stackCount-1))
            {
                indices.Add(k1 + 1);
                indices.Add(k2);
                indices.Add(k2 + 1);
            }
        }
    }
}

void GeometryBuilder::MakeCone(i32 sectorCount, TemporaryArray<glm::vec3>& vertices, TemporaryArray<u32>& indices)
{
    for (i32 i = 0; i < sectorCount; ++i)
    {
        f32 fraction = 1.0f * i / sectorCount;
        f32 angle = fraction * 2.0f * glm::pi<f32>();

        vertices.Add(glm::vec3(glm::cos(angle), glm::sin(angle), 1.0f));
    }

    vertices.Add(glm::vec3(0.0f));
    vertices.Add(glm::forward);

    for (i32 i = 0; i < sectorCount; ++i)
    {
        indices.Add(i);
        indices.Add(sectorCount);
        indices.Add((i + 1) % sectorCount);
    }

	for (i32 i = 0; i < sectorCount; ++i)
	{
        indices.Add(i);
        indices.Add((i + 1) % sectorCount);
        indices.Add(sectorCount + 1);
	}
}
