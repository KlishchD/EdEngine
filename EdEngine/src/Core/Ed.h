#pragma once

#include <memory>
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#include <glm/mat4x4.hpp>
#include "Math/Transform.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

#include "../Utils/SerializationUtils.h"

#include <format>

#define ED_LOG(...) 
#define ED_ASSERT(condition, ...) if (!(condition)) { ED_LOG(__VA_ARGS__) __debugbreak(); }

//assert( (std::format( __VA_ARGS__ ), (condition)) );