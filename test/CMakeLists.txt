include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package( Qt5Core)
find_package( Qt5Test)
set( CMAKE_AUTOMOC ON)
set( CMAKE_INCLUDE_CURRENT_DIR ON)

set( QEJsonS11nTest_SRCS
	AnnotateClassOne.cpp
	QEJsonS11nTest.cpp)
include_directories( ${PROJECT_SOURCE_DIR}/src )

add_executable( QEJsonS11nTest ${QEJsonS11nTest_SRCS})
target_link_libraries( QEJsonS11nTest QEJsonS11n Qt5::Core Qt5::Test ${CONAN_LIBS})
set_target_properties( QEJsonS11nTest PROPERTIES CXX_STANDARD 11)

add_test( NAME QEJsonS11nTest COMMAND  $<TARGET_FILE:QEJsonS11nTest>)
