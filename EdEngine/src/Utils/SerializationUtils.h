#pragma once

#include <glm/fwd.hpp>
#include <glm/detail/type_quat.hpp>

namespace boost
{
    namespace serialization
    {
        template <class Archive>
        void serialize(Archive& ar, glm::mat4& mat, uint32_t version)
        {
            for (int32_t i = 0; i < 4; ++i)
            {
                for (int32_t j = 0; j < 4; ++j)
                {
                    ar & mat[i][j];
                }
            }
        }

        template <class Archive>
        void serialize(Archive& ar, glm::vec3& vec3, uint32_t version)
        {
            ar & vec3.x;
            ar & vec3.y;
            ar & vec3.z;
        }

        template <class Archive>
        void serialize(Archive& ar, glm::quat& q, uint32_t version)
        {
            ar & q.x;
            ar & q.y;
            ar & q.z;
            ar & q.w;
        }
    }
}
