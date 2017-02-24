include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package( Qt5Core)
find_package( Qt5Sql)
find_package( Qt5Test)
set( CMAKE_AUTOMOC ON)
set( CMAKE_INCLUDE_CURRENT_DIR ON)

set( QEOrmTest_SRCS
        Person.cpp ContactInfo.cpp
	QEOrmTest.cpp)
include_directories( ${PROJECT_SOURCE_DIR}/src )

add_executable( QEOrmTest ${QEOrmTest_SRCS})
target_link_libraries( QEOrmTest QEOrm Qt5::Core Qt5::Sql Qt5::Test ${CONAN_LIBS})
set_target_properties( QEOrmTest PROPERTIES CXX_STANDARD 11)

add_test( NAME QEOrmTest COMMAND  $<TARGET_FILE:QEOrmTest>)
