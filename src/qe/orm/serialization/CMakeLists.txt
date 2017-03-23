# Requirements 
include(${CMAKE_SOURCE_DIR}/tools/cmake/common.cmake)

# Source
set( QEOrmS11n_SRCS
	SerializedItem.cpp )

# Link
add_library( QEOrmS11n STATIC ${QEOrmS11n_SRCS})
target_link_libraries( QEOrmS11n 
	Qt5::Core Qt5::Sql ${CONAN_LIBS}) 
target_compile_features( QEOrmS11n PUBLIC cxx_auto_type cxx_nullptr cxx_lambdas)
