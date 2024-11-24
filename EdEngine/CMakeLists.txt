cmake_minimum_required(VERSION 3.26)

project(EdEngine)

set(CMAKE_CXX_STANDARD 20)

set(DEPENDENCIES_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies)

if (WIN32)
    add_definitions(/DWINDOWS_PLATFORM=1)
endif (WIN32)

# ---------- Sources ---------

set(SOURCES_PATH "${CMAKE_CURRENT_LIST_DIR}/src")

file(GLOB_RECURSE SOURCES ${SOURCES_PATH}/*.cpp ${SOURCES_PATH}/*.h)
add_library(EdEngine STATIC ${SOURCES})

include_directories(${SOURCES_PATH})
include_directories(${SOURCES_PATH}/Core)

foreach(_SOURCE IN ITEMS ${SOURCES})
    get_filename_component(_SOURCE_PATH "${_SOURCE}" PATH)
    file(RELATIVE_PATH _SOURCE_RELATIVE_PATH "${SOURCES_PATH}" "${_SOURCE_PATH}")
    string(REPLACE "/" "\\" _GROUP_PATH "${_SOURCE_RELATIVE_PATH}")
    source_group("${_GROUP_PATH}" FILES "${_SOURCE}")
endforeach()

# ---------- OpenGL ----------

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
target_link_libraries(EdEngine PUBLIC ${OPENGL_LIBRARIES})

# ---------- GLEW ----------

add_definitions(/DGLEW_STATIC)

add_library(GLEW STATIC IMPORTED)

set_target_properties(GLEW PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${DEPENDENCIES_PATH}/GLEW/include)
set_target_properties(GLEW PROPERTIES IMPORTED_LOCATION ${DEPENDENCIES_PATH}/GLEW/lib/glew32s.lib)

target_link_libraries(EdEngine PUBLIC GLEW)

# ---------- GLFW ----------

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(${DEPENDENCIES_PATH}/glfw ${DEPENDENCIES_PATH}/glfw/build)
target_link_libraries(EdEngine PUBLIC glfw)

set_target_properties(glfw PROPERTIES FOLDER "Dependencies/GLFW3")
set_target_properties(update_mappings PROPERTIES FOLDER "Dependencies/GLFW3")
set_target_properties(uninstall PROPERTIES FOLDER "Dependencies/GLFW3")

# ---------- ImGui --------

set(ImGuiSources ${DEPENDENCIES_PATH}/ImGui/imgui.h ${DEPENDENCIES_PATH}/ImGui/imgui.cpp ${DEPENDENCIES_PATH}/ImGui/imgui_widgets.cpp ${DEPENDENCIES_PATH}/ImGui/imgui_tables.cpp ${DEPENDENCIES_PATH}/ImGui/backends/imgui_impl_opengl3.cpp ${DEPENDENCIES_PATH}/ImGui/backends/imgui_impl_glfw.cpp ${DEPENDENCIES_PATH}/ImGui/imgui_widgets.cpp  ${DEPENDENCIES_PATH}/ImGui/imgui_draw.cpp)
target_sources(EdEngine PUBLIC ${ImGuiSources})
source_group("ImGui" FILES ${ImGuiSources})
include_directories(${DEPENDENCIES_PATH}/ImGui)

# ---------- GLM ----------

add_definitions(/DGLM_ENABLE_EXPERIMENTAL=1)
add_subdirectory(${DEPENDENCIES_PATH}/glm ${DEPENDENCIES_PATH}/glm/build)
target_link_libraries(EdEngine PUBLIC glm::glm)

set_target_properties(glm PROPERTIES FOLDER "Dependencies")

# ---------- SPDLOG ----------

add_subdirectory(${DEPENDENCIES_PATH}/spdlog ${DEPENDENCIES_PATH}/spdlog/build)
target_link_libraries(EdEngine PUBLIC spdlog::spdlog)

set_target_properties(spdlog PROPERTIES FOLDER "Dependencies")

# ---------- ASSIMP ----------

add_subdirectory(${DEPENDENCIES_PATH}/assimp ${DEPENDENCIES_PATH}/assimp/build)
target_link_libraries(EdEngine PUBLIC assimp::assimp)

set_target_properties(assimp PROPERTIES FOLDER "Dependencies/Assimp")
set_target_properties(unit PROPERTIES FOLDER "Dependencies/Assimp")
set_target_properties(zlibstatic PROPERTIES FOLDER "Dependencies/Assimp")
set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER "Dependencies/Assimp")

# ---------- FREETYPE ----------

add_subdirectory(${DEPENDENCIES_PATH}/freetype ${DEPENDENCIES_PATH}/freetype/build)
target_link_libraries(EdEngine PUBLIC freetype)

set_target_properties(freetype PROPERTIES FOLDER "Dependencies")

# ------------------------------

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src ${DEPENDENCIES_PATH}/ImGui CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)