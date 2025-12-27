#include "EdCore.h"
#include "MathHelper.h"

glm::vec3 glm::translation(mat4 transform)
{
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
                
    glm::decompose(transform, scale, rotation, translation, skew, perspective);

    return translation;
}


glm::vec3 glm::scale(mat4 transform)
{
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
                
    glm::decompose(transform, scale, rotation, translation, skew, perspective);

    return scale;
}

glm::mat4 glm::rotationless(mat4 transform)
{
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
                
	glm::decompose(transform, scale, rotation, translation, skew, perspective);

    return glm::scale(glm::translate(glm::mat4(1.0f), translation), scale);
}

f32 MathHelper::lerp(f32 a, f32 b, f32 f)
{
	return a + f * (b - a);
}

f32 MathHelper::Halton(u32 i, u32 b)
{
	f32 f = 1.0f;
	f32 r = 0.0f;

	while (i > 0)
	{
		f /= static_cast<f32>(b);
		r = r + f * static_cast<f32>(i % b);
		i = static_cast<u32>(floorf(static_cast<f32>(i) / static_cast<f32>(b)));
	}

	return r;
}

Array<glm::vec3> MathHelper::GenerateHalfSphereSamples(i32 count, bool bShiftTowardsCenter)
{
    Array<glm::vec3> samples(count);

	static std::uniform_real_distribution<f32> distribution(0.0f, 1.0f);
	static std::default_random_engine generator;
	for (i32 i = 0; i < count; ++i)
	{
		f32 x = distribution(generator) * 2.0f - 1.0f;
		f32 y = distribution(generator) * 2.0f - 1.0f;
		f32 z = distribution(generator);

		glm::vec3 sample(x, y, z);
		sample = glm::normalize(sample) * distribution(generator);

        if (bShiftTowardsCenter)
        {
		    f32 scale = 1.0f * i / count;
		    sample *= MathHelper::lerp(0.1f, 1.0f, scale * scale);
        }

        samples.Add(sample);
	}

    return samples;
}

Array<glm::vec2> MathHelper::GenerateCircleSamples(i32 count)
{
	Array<glm::vec2> smaples;

	static std::uniform_real_distribution<f32> distribution(0.0f, 1.0f);
	static std::default_random_engine generator;
	for (i32 i = 0; i < count; ++i)
	{
		f32 angle = distribution(generator) * 2.0f * glm::pi<f32>();

		f32 len = distribution(generator);
		glm::vec2 smaple(glm::cos(angle) * len, glm::sin(angle) * len);

		smaples.Add(smaple);
	}

	return smaples;
}

glm::vec2 MathHelper::GenerateRandomNormalizedVec2()
{
    static std::uniform_real_distribution<f32> distribution(0.0f, 1.0f);
    static std::default_random_engine generator;
	return { distribution(generator), distribution(generator) };
}

glm::vec3 MathHelper::MinPerComponent(const glm::vec3& a, const glm::vec3& b)
{
	return {
		glm::min(a.x, b.x),
		glm::min(a.y, b.y),
		glm::min(a.z, b.z)
	};
}

glm::vec3 MathHelper::MaxPerComponent(const glm::vec3& a, const glm::vec3& b)
{
	return {
		glm::max(a.x, b.x),
		glm::max(a.y, b.y),
		glm::max(a.z, b.z)
	};
}