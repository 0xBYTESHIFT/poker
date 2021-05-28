cmake_minimum_required(VERSION 3.18)

enable_testing()
cmake_policy(SET CMP0048 NEW)
set(CMAKE_PROJECT_VERSION_MAJOR 1)
set(CMAKE_PROJECT_VERSION_MINOR 0)
set(CMAKE_PROJECT_VERSION_PATCH 0)
set(CMAKE_PROJECT_VERSION "${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH}")
PROJECT(poker VERSION "${CMAKE_PROJECT_VERSION}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
else ()
    # stack track
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic")
endif ()

option(USE_TRACY "code profiling" ON)
option(CONAN_EXPERIMENTAL_INCLUDE "CONAN EXPERIMENT INCLUDE" ON)

if ((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    add_compile_options(
            #-Werror
            #           -Wall
            #           -Wextra
            #           -Wpedantic
            #            -Wcast-align
            #            -Wcast-qual
            #            -Wconversion
            #            -Wctor-dtor-privacy
            #            -Wenum-compare
            #            -Wfloat-equal
            #            -Wnon-virtual-dtor
            #            -Wold-style-cast
            #            -Woverloaded-virtual
            #            -Wredundant-decls
            #            -Wsign-conversion
            #            -Wsign-promo
  )
endif ()

# configuration
#############################################################################################
#conan

if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.15/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake")
endif ()

include(${CMAKE_BINARY_DIR}/conan.cmake)

set(CONAN_SYSTEM_INCLUDES "On")

set(CURSES_NEED_NCURSES TRUE CACHE BOOL "") 
find_package(Curses REQUIRED)

if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else ()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif ()

message(STATUS "LIBS = ${CONAN_LIBS}")

get_filename_component(TRACY_PATH "${CMAKE_CURRENT_LIST_DIR}/tracy" ABSOLUTE)
get_filename_component(API_PATH "${CMAKE_CURRENT_LIST_DIR}/api" ABSOLUTE)
get_filename_component(PROJECT_ROOT_PATH "${CMAKE_CURRENT_LIST_DIR}/." ABSOLUTE)

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -lm")
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBIG} -Og -fno-omit-frame-pointer -fsanitize=address")

add_compile_definitions(RAPIDJSON_HAS_STDSTRING=1)

include_directories(${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(client)
add_subdirectory(tests)
add_subdirectory(service)
