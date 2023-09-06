cmake_minimum_required(VERSION 3.23)
project(BRender)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Prefer glvnd
cmake_policy(SET CMP0072 NEW)

cmake_policy(SET CMP0079 NEW)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Perl REQUIRED)

if (BUILD_SHARED_LIBS)
    message(FATAL_ERROR "BUILD_SHARED_LIBS is not supported at this time.")
endif ()

option(BRENDER_DISABLE_FINDSDL "Disable searching for SDL2" OFF)
if (NOT BRENDER_DISABLE_FINDSDL)
    find_package(SDL2)
endif ()

option(BRENDER_BUILD_DRIVERS "Build Drivers" ON)
option(BRENDER_BUILD_SOFT "Build Software Renderer" OFF)

add_subdirectory(core)

if (BRENDER_BUILD_DRIVERS)
    add_subdirectory(drivers)
endif ()

##
# Core BRender, no drivers, no DDI.
##
add_library(BRender::Core ALIAS brender)

##
# Full BRender, with drivers, no DDI.
##
add_library(BRender::Full ALIAS brender-full)

##
# Core BRender, no drivers, with DDI.
##
add_library(BRender::DDI ALIAS brender-ddi)


option(BRENDER_BUILD_TOOLS "Build Tools" ON)
option(BRENDER_BUILD_EXAMPLES "Build Examples" ON)
option(BRENDER_DISABLE_INSTALL "Disable installation" OFF)

if (BRENDER_BUILD_TOOLS)
    add_subdirectory(tools)
endif ()

if (BRENDER_BUILD_EXAMPLES AND BRENDER_BUILD_DRIVERS AND TARGET SDL2::SDL2)
    add_subdirectory(examples)
endif ()

if (NOT BRENDER_DISABLE_INSTALL)
    include(cmake/packaging.cmake)
endif ()
