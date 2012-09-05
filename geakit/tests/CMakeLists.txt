##test_API
set(TEST_API_SRC
	test_API.cpp
	../src/api/gaccountapi.cpp
	../src/api/grepositoryapi.cpp
	)
set(TEST_HEADS
	../src/api/gaccountapi.h
	../src/api/grepositoryapi.h
	)
qt4_wrap_cpp(TEST_MOC_SRC ${TEST_HEADS})

add_executable(test_api ${TEST_API_SRC} ${TEST_MOC_SRC})
target_link_libraries(test_api git2 qjson ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY})

set(TEST_CONFIG_SRC
	test_config.cpp
	)
add_executable(test_config ${TEST_CONFIG_SRC})
target_link_libraries(test_config git2)
#test branchView
set(TEST_BRANCH_SRC
	test_branchView.cpp
	../src/gui/gbranchview.cpp
	../src/gui/gitcommand.cpp
	../src/gui/gbutton.cpp
	../src/gui/gbranchnamedialog.cpp
	../src/gui/gremotenamedialog.cpp
	)
set(TEST_BRANCH_HEADS
	../src/gui/gbranchview.h
	../src/gui/gitcommand.h
	../src/gui/gbutton.h
	../src/gui/gbranchnamedialog.h
	../src/gui/gremotenamedialog.h
	)
qt4_wrap_cpp(TEST_BRANCH_MOC_SRC ${TEST_BRANCH_HEADS})
add_executable(test_branchview ${TEST_BRANCH_SRC} ${TEST_BRANCH_MOC_SRC})
target_link_libraries(test_branchview git2 ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

set(TEST_BRANCH_SRC1
	test_branch.cpp
	)
add_executable(test_branch ${TEST_BRANCH_SRC1})
target_link_libraries(test_branch git2)
##test git command
set(TEST_COMMAND_SRC 
	test_gitcommand.cpp
	../src/gui/gitcommand.cpp
	)
set(TEST_COMMAND_HEADS ../src/gui/gitcommand.h)
qt4_wrap_cpp(TEST_COMMAND_MOC_SRC ${TEST_COMMAND_HEADS})
add_executable(test_gitcommand ${TEST_COMMAND_SRC} ${TEST_COMMAND_MOC_SRC})
target_link_libraries(test_gitcommand git2 ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
