# Requirements 
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
find_package( Qt5Core REQUIRED)
find_package( Qt5Sql REQUIRED)
set( CMAKE_AUTOMOC ON)
set( CMAKE_INCLUDE_CURRENT_DIR ON)

# Source
set( QEOrm_MAYOR_VERSION 0)
set( QEOrm_MINOR_VERSION 1)
set( QEOrm_BUILD_VERSION 0)

set( QEOrm_SRCS
	QEOrm.cpp
	QEOrmModel.cpp QEOrmModelPrivate.cpp
	QEOrmColumnDef.cpp QEOrmColumnDefPrivate.cpp
	DBDriver/SQLGenerator.cpp DBDriver/SQliteGenerator.cpp )
if( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	add_definitions( -Wall)
endif()

# Link
add_library( QEOrm SHARED ${QEOrm_SRCS})
target_link_libraries( QEOrm Qt5::Core  Qt5::Sql ${CONAN_LIBS}) 
set_target_properties( QEOrm PROPERTIES 
	VERSION ${QEOrm_MAYOR_VERSION}.${QEOrm_MINOR_VERSION}.${QEOrm_BUILD_VERSION}
	SOVERSION ${QEOrm_MAYOR_VERSION}
	CXX_STANDARD 11)