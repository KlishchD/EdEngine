#include "MathUtils.h"
#include <glm/detail/type_quat.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <random>

glm::vec3 glm::translation(mat4 Transform)
{
    vec3 scale;
    quat rotation;
    vec3 translation;
    vec3 skew;
    vec4 perspective;
                
    decompose(Transform, scale, rotation, translation, skew, perspective);

    return translation;
}


glm::vec3 glm::scale(mat4 Transform)
{
    vec3 scale;
    quat rotation;
    vec3 translation;
    vec3 skew;
    vec4 perspective;
                
    decompose(Transform, scale, rotation, translation, skew, perspective);

    return scale;
}

glm::mat4 glm::rotationless(mat4 Transform)
{
    vec3 scale;
    quat rotation;
    vec3 translation;
    vec3 skew;
    vec4 perspective;
                
    decompose(Transform, scale, rotation, translation, skew, perspective);

    return glm::scale(glm::translate(glm::mat4(1.0f), translation), scale);
}

float Math::lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

float Math::Halton(uint32_t i, uint32_t b)
{
	float f = 1.0f;
	float r = 0.0f;

	while (i > 0)
	{
		f /= static_cast<float>(b);
		r = r + f * static_cast<float>(i % b);
		i = static_cast<uint32_t>(floorf(static_cast<float>(i) / static_cast<float>(b)));
	}

	return r;
}

std::vector<glm::vec3> Math::GenerateHalfSphereSamples(int32_t count)
{
    static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    static std::default_random_engine generator;

    std::vector<glm::vec3> result(count);
    for (int32_t i = 0; i < count; ++i)
    {
        float x = distribution(generator) * 2.0f - 1.0f;
        float y = distribution(generator) * 2.0f - 1.0f;
        float z = distribution(generator);

        glm::vec3 sample(x, y, z);
        sample = glm::normalize(sample) * distribution(generator);

        float scale = 1.0f * i / count;
        sample *= Math::lerp(0.1f, 1.0f, scale * scale);

        result[i] = sample;
    }

    return result;
}
