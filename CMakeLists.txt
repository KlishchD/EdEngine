cmake_minimum_required(VERSION 3.26)

Project(Ed)

set(CMAKE_CXX_STANDARD 20)

if (WIN32)
		add_definitions(/DWINDOWS_PLATFORM=1)
		set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif (WIN32)

string(REPLACE "/" "\\\\" RESOURCES_PATH "${CMAKE_CURRENT_LIST_DIR}/resources")
add_definitions(/DRESOURCES_PATH="${RESOURCES_PATH}")

add_subdirectory(EdEngine)
add_subdirectory(EdEditor)