include(${CMAKE_SOURCE_DIR}/tools/cmake/common.cmake)
find_package( Qt5Test)
find_package( Boost REQUIRED COMPONENTS serialization )

set( QEOrmTest_SRCS 
	entity/book.cpp
	entity/chapter.cpp
	QEOrmTest.qrc
	QEOrmTest.cpp)

add_executable( QEOrmTest ${QEOrmTest_SRCS})
target_link_libraries( QEOrmTest
	QEOrm
	Qt5::Core Qt5::Sql Qt5::Test
	Boost::serialization
	${CONAN_LIBS})

target_compile_features( QEOrmTest 
	PUBLIC 
		cxx_auto_type cxx_nullptr cxx_lambdas 
		cxx_uniform_initialization)


set_target_properties( QEOrmTest
	PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_test( NAME QEOrmTest COMMAND  $<TARGET_FILE:QEOrmTest>)
# add_test( NAME QEOrmTest COMMAND QEOrmTest )
