#include "MathUtils.h"
#include <glm/detail/type_quat.hpp>
#include <glm/gtx/matrix_decompose.hpp>

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
