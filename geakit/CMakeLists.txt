project(geakit)

set(GEAKIT_VERSION_MAJOR 0)
set(GEAKIT_VERSION_MINOR 1)
set(GEAKIT_VERSION_RELEASE 1)

include_directories(src ${LIBGIT2_INCLUDES} ${GSHELL_INCLUDES} ${QT_INCLUDES})

#Build options
option (BUILD_TESTS "Build Tests" OFF)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug" cache string "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

set(GEAKIT_SRCS
	src/main.cpp
	)

set(GEAKIT_HEADERS
	)

set(GEAKIT_UIS
	)

set(GEAKIT_RCCS
	)

set(GEAKIT_TSS
	)

qt4_wrap_ui(GEAKIT_UI_HEADERS ${GEAKIT_UIS})
qt4_wrap_cpp(GEAKIT_MOC_SRCS ${GEAKIT_HEADERS} ${GEAKIT_UI_HEADERS})
qt4_add_resources(GEAKIT_RCC_SRCS ${GEAKIT_RCCS})
qt4_create_translation(GEAKIT_QMS ${GEAKIT_SRCS} ${GEAKITTSS})

add_executable(geakit ${GEAKIT_SRCS} ${GEAKIT_MOC_SRCS} ${GEAKIT_RCC_SRCS} ${GEAKIT_QMS})
target_link_libraries(geakit gshell ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

if(BUILD_TESTS)
endif()
