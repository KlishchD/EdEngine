#pragma once

#include <memory>
#include <cstdint>
#include <string>
#include <string.h>
#include <vector>
#include <map>
#include <unordered_map>

#include <glm/mat4x4.hpp>
#include "Math/Transform.h"

#include "Utils/SerializationUtils.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "Macros.h"

using UUID = boost::uuids::uuid;
namespace UUIDs = boost::uuids;