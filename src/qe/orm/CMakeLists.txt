# Requirements 
include(${CMAKE_SOURCE_DIR}/tools/cmake/common.cmake)

# Source
set( QEOrm_MAYOR_VERSION 0)
set( QEOrm_MINOR_VERSION 1)
set( QEOrm_BUILD_VERSION 0)

set( QEOrm_SRCS
	LoadHelper.cpp SaveHelper.cpp FindHelper.cpp
	ResultSet.cpp
	QEOrm.cpp )
add_subdirectory( sql)
add_subdirectory( serialization)

# Link
add_library( QEOrm SHARED ${QEOrm_SRCS})
target_link_libraries( QEOrm 
	QEOrmS11n QEOrmSql
	Qt5::Core  Qt5::Sql ${CONAN_LIBS}) 
set_target_properties( QEOrm PROPERTIES 
	VERSION ${QEOrm_MAYOR_VERSION}.${QEOrm_MINOR_VERSION}.${QEOrm_BUILD_VERSION}
	SOVERSION ${QEOrm_MAYOR_VERSION})
target_compile_features( QEOrm PUBLIC cxx_auto_type cxx_nullptr cxx_lambdas)
