# Requirements 
include(${CMAKE_SOURCE_DIR}/tools/cmake/common.cmake)

# Source
set( QEOrmSqlGenerator_SRCS
	AbstractGenerator.cpp 
	ANSIGenerator.cpp 
	SQLiteGenerator.cpp 
	MySQLGenerator.cpp
	CachedGenerator.cpp)

# Link
add_library( QEOrmSqlGenerator STATIC ${QEOrmSqlGenerator_SRCS})
target_link_libraries( QEOrmSqlGenerator 
	Qt5::Core  Qt5::Sql ${CONAN_LIBS}) 
target_compile_features( QEOrmSqlGenerator PUBLIC cxx_auto_type cxx_nullptr 
	cxx_lambdas)
