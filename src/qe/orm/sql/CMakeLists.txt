# Requirements 
include(${CMAKE_SOURCE_DIR}/tools/cmake/common.cmake)
add_subdirectory( generator)

# Source
set( QEOrmSql_SRCS
	GeneratorRepository.cpp
	Executor.cpp )

# Link
add_library( QEOrmSql STATIC ${QEOrmSql_SRCS})
target_link_libraries( QEOrmSql 
	QEOrmSqlGenerator
	Qt5::Core  Qt5::Sql ${CONAN_LIBS}) 
target_compile_features( QEOrmSql PUBLIC cxx_auto_type cxx_nullptr cxx_lambdas)
