project(QAnnotationTest)
cmake_minimum_required(VERSION 3.0.0)
find_package( Qt5Core REQUIRED)
find_package( Qt5Sql REQUIRED)

set( CMAKE_AUTOMOC ON)
set( CMAKE_AUTOUIC ON)
set( CMAKE_INCLUDE_CURRENT_DIR ON)


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable( packageTest main.cpp)
target_link_libraries( packageTest Qt5::Core Qt5::Sql ${CONAN_LIBS})

target_compile_features( packageTest PUBLIC cxx_nullptr cxx_range_for)
