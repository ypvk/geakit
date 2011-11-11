project(gshell)

set(GSHELL_VERSION_MAJOR 0)
set(GSHELL_VERSION_MINOR 1)
set(GSHELL_VERSION_RELEASE 1)

include_directories(src ${LIBGIT2_INCLUDES} ${GSHELL_INCLUDES} ${QT_INCLUDES})

# Build options
option (BUILD_SHARED_LIBS "Build Shared Library (OFF for Static)" ON)
#option (THREADSAFE "Build as threadsafe" OFF)
option (BUILD_TESTS "Build Tests" OFF)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug" cache string "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

add_definitions(-DGSHELL_LIBRARY)

set(GSHELL_SRCS
	src/high.cpp
	src/low.cpp
	)

set(GSHELL_HEADERS
	)

qt4_wrap_cpp(GSHELL_HEADERS_MOC ${GSHELL_HEADERS})

add_library(gshell ${GSHELL_SRCS} ${GSHELL_HEADERS_MOC})

target_link_libraries(gshell git2 ${QT_QTCORE_LIBRARY})

if(BUILD_TESTS)
endif()
