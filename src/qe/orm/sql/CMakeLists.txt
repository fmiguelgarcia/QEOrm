# Requirements 
include(${CMAKE_SOURCE_DIR}/tools/cmake/common.cmake)

# Source
set( QEOrmSql_SRCS
	generator/ANSIGenerator.cpp generator/SQLiteGenerator.cpp
	SqlHelper.cpp )

# Link
add_library( QEOrmSql STATIC ${QEOrmSql_SRCS})
target_link_libraries( QEOrmSql 
	Qt5::Core  Qt5::Sql ${CONAN_LIBS}) 
target_compile_features( QEOrmSql PUBLIC cxx_auto_type cxx_nullptr cxx_lambdas)
