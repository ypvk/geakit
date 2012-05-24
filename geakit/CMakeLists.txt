project(geakit)

include_directories(src ${CMAKE_CURRENT_BINARY_DIR} ${LIBGIT2_INCLUDES} ${QT_INCLUDES} ${QJSON_INCLUDES})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/version.h)

#Build options
option (BUILD_TESTS "Build Tests" ON)

SET(QT_USE_QTWEBKIT TRUE)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug" cache string "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

set(GEAKIT_SRCS
	src/main.cpp
	#src/test1.cpp
	src/gui/gmainwindow.cpp
	src/gui/gsettingsdialog.cpp
	src/api/gaccountapi.cpp
	src/data/account.cpp
	src/gui/gprojectdialog.cpp
	src/gui/gcodeview.cpp
	src/gui/gcommitview.cpp
	src/gui/gbranchview.cpp
	src/gui/gpatchview.cpp
	src/gui/gitcommand.cpp
	src/gui/gbutton.cpp
	src/gui/gcommitdialog.cpp
	)

set(GEAKIT_HEADERS
	src/gui/gmainwindow.h
	src/gui/gsettingsdialog.h
	src/api/gaccountapi.h
	src/gui/gprojectdialog.h
	src/gui/gcodeview.h
	src/gui/gcommitview.h
	src/gui/gbranchview.h
	src/gui/gpatchview.h
	src/gui/gitcommand.h
	src/gui/gbutton.h
	src/gui/gcommitdialog.h
	)

set(GEAKIT_UIS
	uis/gmainwindow.ui
	uis/gsettingsdialog.ui
	)

set(GEAKIT_RCCS
	src/gui/html_resource.qrc
	)

set(GEAKIT_TSS
	)

qt4_wrap_ui(GEAKIT_UI_HEADERS ${GEAKIT_UIS})
qt4_wrap_cpp(GEAKIT_MOC_SRCS ${GEAKIT_HEADERS} ${GEAKIT_UI_HEADERS})
qt4_add_resources(GEAKIT_RCC_SRCS ${GEAKIT_RCCS})
qt4_create_translation(GEAKIT_QMS ${GEAKIT_SRCS} ${GEAKITTSS})

add_executable(geakit ${GEAKIT_SRCS} ${GEAKIT_MOC_SRCS} ${GEAKIT_RCC_SRCS} ${GEAKIT_QMS})
target_link_libraries(geakit git2 qjson ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTWEBKIT_LIBRARY})

if(BUILD_TESTS)
	#funtion to compile the test arg1:name, arg2:fileList
	FUNCTION(TEST ARGS)
	list(GET ARGV 0 TEST_NAME)
	message(${TEST_NAME})
	list(REMOVE_AT ARGV 0)
	set(TEST_FILES ${ARGV})
	add_executable(${TEST_NAME} ${TEST_FILES} ${GEAKIT_MOC_SRCS} ${GEAKIT_RCC_SRCS} ${GEAKIT_QMS})
	target_link_libraries(${TEST_NAME} git2 qjson ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTWEBKIT_LIBRARY} ${QT_QTNETWORK_LIRARY}) 
	ENDFUNCTION(TEST ARGS)
	#end function
	#	add_subdirectory(testsset(TEST_SRC test1.cpp)
	set(TEST_SRC 
		tests/test1.cpp	
		src/gui/gmainwindow.cpp
		src/gui/gsettingsdialog.cpp
		src/api/gaccountapi.cpp
		src/data/account.cpp
		src/gui/gprojectdialog.cpp
		src/gui/gcodeview.cpp
		src/gui/gcommitview.cpp
		src/gui/gbranchview.cpp
		src/gui/gpatchview.cpp
		src/gui/gitcommand.cpp
		src/gui/gbutton.cpp
		src/gui/gcommitdialog.cpp
		)
	TEST(test2 ${TEST_SRC})
	##test_API
	set(TEST_API_SRC
		tests/test_API.cpp
		src/api/gaccountapi.cpp
		)
	set(TEST_HEADS
		src/api/gaccountapi.h
		)
	qt4_wrap_cpp(TEST_MOC_SRC ${TEST_HEADS})
		
	add_executable(test_api ${TEST_API_SRC} ${TEST_MOC_SRC})
	target_link_libraries(test_api git2 qjson ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY})
endif()
