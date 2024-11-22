cmake_minimum_required(VERSION 3.26)

project(EdEditor)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES EdEditor/src/*.cpp)

include_directories(EdEditor/src/ EdEngine/src Dependencies/ImGui)

if (WIN32)
    add_definitions(/DWINDOWS_PLATFORM=1)
endif (WIN32)


# ---------- Sources ---------

set(SOURCES_PATH "${CMAKE_CURRENT_LIST_DIR}/src")
file(GLOB_RECURSE SOURCES ${SOURCES_PATH}/*.cpp ${SOURCES_PATH}/*.h)
add_executable(EdEditor ${SOURCES})

include_directories(${SOURCES_PATH})

foreach(_SOURCE IN ITEMS ${SOURCES})
    get_filename_component(_SOURCE_PATH "${_SOURCE}" PATH)
    file(RELATIVE_PATH _SOURCE_RELATIVE_PATH "${SOURCES_PATH}" "${_SOURCE_PATH}")
    string(REPLACE "/" "\\" _GROUP_PATH "${_SOURCE_RELATIVE_PATH}")
    source_group("${_GROUP_PATH}" FILES "${_SOURCE}")
endforeach()

# ---------- EdEngine ---------

include_directories(${EdEngine_INCLUDE_DIRS})
target_link_libraries(EdEditor EdEngine)