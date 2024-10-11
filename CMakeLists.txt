cmake_minimum_required(VERSION 3.26)

project(EdEditor)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES EdEditor/src/*.cpp EdEngine/src/*.cpp)
add_executable(main ${SOURCES} Dependencies/ImGui/imgui.cpp Dependencies/ImGui/imgui_widgets.cpp Dependencies/ImGui/imgui_tables.cpp Dependencies/ImGui/backends/imgui_impl_opengl3.cpp Dependencies/ImGui/backends/imgui_impl_glfw.cpp)

include_directories(EdEditor/src/ EdEngine/src Dependencies/ImGui)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(main PRIVATE glfw)

find_package(glm CONFIG REQUIRED)
target_link_libraries(main PRIVATE glm::glm)

find_package(GLEW REQUIRED)
target_link_libraries(main PRIVATE GLEW::GLEW)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(main PRIVATE assimp::assimp)

find_package(Freetype REQUIRED)
target_link_libraries(main PRIVATE Freetype::Freetype)

find_package(glslang CONFIG REQUIRED)
target_link_libraries(main PRIVATE glslang::OSDependent glslang::glslang glslang::MachineIndependent glslang::GenericCodeGen glslang::glslang-default-resource-limits glslang::OGLCompiler glslang::SPVRemapper glslang::SPIRV glslang::HLSL)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(main PRIVATE fmt::fmt)


set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

find_package(Boost REQUIRED serialization)
target_link_libraries(main PRIVATE Boost::boost Boost::serialization Boost::uuids)