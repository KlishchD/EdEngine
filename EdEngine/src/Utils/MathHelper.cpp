#include "MathHelper.h"
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

float MathHelper::lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

float MathHelper::Halton(uint32_t i, uint32_t b)
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

std::vector<glm::vec3> MathHelper::GenerateHalfSphereSamples(int32_t count, bool bShiftTowardsCenter)
{
    std::vector<glm::vec3> samples(count);

	std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
	std::default_random_engine generator;
	for (int32_t i = 0; i < count; ++i)
	{
		float x = distribution(generator) * 2.0f - 1.0f;
		float y = distribution(generator) * 2.0f - 1.0f;
		float z = distribution(generator);

		glm::vec3 sample(x, y, z);
		sample = glm::normalize(sample) * distribution(generator);

        if (bShiftTowardsCenter)
        {
		    float scale = 1.0f * i / count;
		    sample *= MathHelper::lerp(0.1f, 1.0f, scale * scale);
        }

        samples[i] = sample;
	}

    return samples;
}
