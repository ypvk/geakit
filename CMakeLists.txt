project(Geakit)

cmake_minimum_required(VERSION 2.6)

set(GEAKIT_CLI_VERSION_MAJOR 0)
set(GEAKIT_CLI_VERSION_MINOR 1)
set(GEAKIT_CLI_VERSION_RELEASE 1)

find_package(Qt4 4.5 REQUIRED)

set(LIBGIT2_INCLUDES ${CMAKE_SOURCE_DIR}/libgit2/include)

add_subdirectory(libgit2)
add_subdirectory(geakit)
