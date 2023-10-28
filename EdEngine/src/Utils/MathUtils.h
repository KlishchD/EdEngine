#pragma once

#include <glm/fwd.hpp>

namespace glm
{
    vec3 translation(mat4 Transform);
    vec3 scale(mat4 Transform);

    mat4 rotationless(mat4 Transform);

    template<typename T, qualifier Q>
    mat<4, 4, T, Q> lookAtI(vec<3, T, Q> const& eye, vec<3, T, Q> const& center, vec<3, T, Q> const& up)
    {
        vec<3, T, Q> const f(normalize(center - eye));
        vec<3, T, Q> const s(normalize(cross(f, up)));
        vec<3, T, Q> const u(cross(s, f));

        mat<4, 4, T, Q> Result(1);
        Result[0][0] = s.x;
        Result[1][0] = s.y;
        Result[2][0] = s.z;
        Result[0][1] = u.x;
        Result[1][1] = u.y;
        Result[2][1] = u.z;
        Result[0][2] =-f.x;
        Result[1][2] =-f.y;
        Result[2][2] =-f.z;
        Result[3][0] =eye.x;
        Result[3][1] =eye.y;
        Result[3][2] =eye.z;
        return Result;
    }
}
