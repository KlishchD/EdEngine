#pragma once

#include <glm/fwd.hpp>
#include <glm/detail/type_quat.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <boost/serialization/export.hpp>
#include <boost/serialization/assume_abstract.hpp>

#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>

#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/binary_object.hpp>

#include <boost/uuid/uuid_serialize.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <boost/serialization/serialization.hpp>

#include "Core/Rendering/Types.h"

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
        void serialize(Archive& ar, glm::vec3& vec, uint32_t version)
        {
            ar & vec.x;
            ar & vec.y;
            ar & vec.z;
        }

		template <class Archive>
		void serialize(Archive& ar, glm::vec4& vec, uint32_t version)
		{
			ar& vec.x;
			ar& vec.y;
			ar& vec.z;
			ar& vec.w;
		}

        template <class Archive>
        void serialize(Archive& ar, glm::quat& q, uint32_t version)
        {
            ar & q.x;
            ar & q.y;
            ar & q.z;
            ar & q.w;
        }

		template <class Archive>
		void serialize(Archive& ar, Vertex& vertex, uint32_t version)
		{
			ar & vertex.Position;
			ar & vertex.Color;
			ar & vertex.TextureCoordinates;
			ar & vertex.Normal;
			ar & vertex.Tangent;
			ar & vertex.Bitangent;
		}
    }
}

class Serialization
{
public:
	template<class T>
	static void LoadDescriptor(const std::string& path, const std::shared_ptr<T>& descriptor, bool bLoadData = true);

	template<class T>
	static void SaveDescriptor(const std::string& path, const std::shared_ptr<T>& descriptor);

};

template<class T>
inline void Serialization::LoadDescriptor(const std::string& path, const std::shared_ptr<T>& descriptor, bool bLoadData)
{
	if (!descriptor->HasData())
	{
		descriptor->SetShouldHaveData(bLoadData);
		{
			std::ifstream file(path, std::ios_base::binary);
			boost::archive::binary_iarchive ia(file);
			ia & *descriptor;
		}
		descriptor->SetShouldHaveData(false);
	}
}

template<class T>
inline void Serialization::SaveDescriptor(const std::string& path, const std::shared_ptr<T>& descriptor)
{
	std::ofstream file(path, std::ios_base::binary);
	boost::archive::binary_oarchive oa(file);
	oa & *descriptor.get();
}
