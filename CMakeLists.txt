cmake_minimum_required(VERSION 3.26)

Project(Ed)

set(CMAKE_CXX_STANDARD 20)

set(ENABLE_ED_TEST 1)

if (WIN32)
		add_definitions(/DWINDOWS_PLATFORM=1)
		set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif (WIN32)

string(REPLACE "/" "\\\\" RESOURCES_PATH "${CMAKE_CURRENT_LIST_DIR}/resources")
add_definitions(/DRESOURCES_PATH="${RESOURCES_PATH}")

if (${ENABLE_ED_TEST} EQUAL 1)
	add_definitions(/DENABLE_ED_TEST=1)
else()
	add_definitions(/DENABLE_ED_TEST=0)
endif()

add_subdirectory(EdEngine)

if (${RUN_CORE_PLAY_TESTS} EQUAL 1)
	add_definitions(/DPLAY_RECORDINGS_NAMES="fly_over","spin","walk_bottom_floor","walk_top_floor")
	add_definitions(/DRUN_PLAY_TESTS_AND_EXIT=1)

	message("Added core tests")
endif()

if (${ENABLE_ED_TEST} EQUAL 1)
	add_subdirectory(EdTest)
endif()

add_subdirectory(EdEditor)