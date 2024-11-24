cmake_minimum_required(VERSION 3.26)

project(EdTest)

set(CMAKE_CXX_STANDARD 20)

# ---------- Sources ---------

set(SOURCES_PATH "${CMAKE_CURRENT_LIST_DIR}/src")
file(GLOB_RECURSE SOURCES ${SOURCES_PATH}/*.cpp ${SOURCES_PATH}/*.h)
add_library(EdTest ${SOURCES})

include_directories(${SOURCES_PATH})

foreach(_SOURCE IN ITEMS ${SOURCES})
    get_filename_component(_SOURCE_PATH "${_SOURCE}" PATH)
    file(RELATIVE_PATH _SOURCE_RELATIVE_PATH "${SOURCES_PATH}" "${_SOURCE_PATH}")
    string(REPLACE "/" "\\" _GROUP_PATH "${_SOURCE_RELATIVE_PATH}")
    source_group("${_GROUP_PATH}" FILES "${_SOURCE}")
endforeach()

# ---------- EdEngine ---------

include_directories(${EdEngine_INCLUDE_DIRS})
target_link_libraries(EdTest EdEngine)

# ------------------------------

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)