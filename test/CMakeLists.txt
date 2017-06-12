include(${CMAKE_SOURCE_DIR}/tools/cmake/common.cmake)
find_package( Qt5Test)
set( CMAKE_AUTORCC ON)

set( QEOrmTest_SRCS 
	entity/book.cpp
	entity/chapter.cpp
	QEOrmTest.qrc
	QEOrmTest.cpp)

add_executable( QEOrmTest ${QEOrmTest_SRCS})
target_link_libraries( QEOrmTest QEOrm Qt5::Core Qt5::Sql Qt5::Test ${CONAN_LIBS})

add_test( NAME QEOrmTest COMMAND  $<TARGET_FILE:QEOrmTest>)
