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
