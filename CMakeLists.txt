cmake_minimum_required(VERSION 3.26)

project(EdEditor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_GENERATOR_PLATFORM win32)

set(VCPKG_TARGET_TRIPLET x64-windows-static CACHE STRING "VCPKG Target Triplet to use")

file(GLOB_RECURSE SOURCES EdEditor/src/*.cpp EdEngine/src/*.cpp)
add_executable(main ${SOURCES} Dependencies/ImGui/imgui.h Dependencies/ImGui/imgui.cpp Dependencies/ImGui/imgui_widgets.cpp Dependencies/ImGui/imgui_tables.cpp Dependencies/ImGui/backends/imgui_impl_opengl3.cpp Dependencies/ImGui/backends/imgui_impl_glfw.cpp Dependencies/ImGui/imgui_widgets.cpp  Dependencies/ImGui/imgui_draw.cpp)

include_directories(EdEditor/src/ EdEngine/src Dependencies/ImGui)

# ---------- GLFW ----------

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(Dependencies/glfw)
target_link_libraries(main PUBLIC glfw)

# ---------- GLM ----------

add_definitions(/DGLM_ENABLE_EXPERIMENTAL=1)
add_subdirectory(Dependencies/glm)
target_link_libraries(main PUBLIC glm::glm)

# ---------- GLEW ----------

add_definitions(/DGLEW_STATIC)

add_library(GLEW STATIC IMPORTED)

set_target_properties(GLEW PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/GLEW/include)
set_target_properties(GLEW PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/GLEW/lib/glew32s.lib)

target_link_libraries(main PUBLIC GLEW)

install(TARGETS main DESTINATION ${CMAKE_INSTALL_PREFIX})

# ---------- OpenGL ----------

find_package(OpenGL REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS})
target_link_libraries(main PUBLIC ${OPENGL_LIBRARIES})

# ---------- ASSIMP ----------

add_subdirectory(Dependencies/assimp)
target_link_libraries(main PUBLIC assimp::assimp)

# ---------- FREETYPE ----------

add_subdirectory(Dependencies/freetype)
target_link_libraries(main PUBLIC freetype)

# ---------- FMT ----------

#add_subdirectory(Dependencies/fmt)
#target_link_libraries(main PUBLIC fmt::fmt)

# ---------- SPDLOG ----------

add_subdirectory(Dependencies/spdlog)
target_link_libraries(main PUBLIC spdlog::spdlog)