cmake_minimum_required(VERSION 3.7)
project(rvnjsonresource)

include(CTest)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

option(BUILD_SHARED_LIBS "Set to ON to build shared libraries; OFF for static libraries." OFF)
option(WARNING_AS_ERROR "Set to ON to build with -Werror" ON)

option(BUILD_TEST_COVERAGE "Set to ON to build while generating coverage information. Will put source on the build directory." OFF)

find_package(Boost 1.49 REQUIRED)

add_library(rvnjsonresource
  src/metadata.cpp
  src/reader.cpp
  src/writer.cpp
)

target_compile_options(rvnjsonresource PRIVATE -W -Wall -Wextra -Wmissing-include-dirs -Wunknown-pragmas -Wpointer-arith -Wmissing-field-initializers -Wno-multichar -Wreturn-type)

if(WARNING_AS_ERROR)
  target_compile_options(rvnjsonresource PRIVATE -Werror)
endif()

if(BUILD_TEST_COVERAGE)
  target_compile_options(rvnjsonresource PRIVATE -g -O0 --coverage -fprofile-arcs -ftest-coverage)
  target_link_libraries(rvnjsonresource PRIVATE gcov)
endif()

target_include_directories(rvnjsonresource
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(rvnjsonresource
  PUBLIC
    Boost::boost
)

set(PUBLIC_HEADERS
  include/metadata.h
  include/reader.h
  include/writer.h
)

set_target_properties(rvnjsonresource PROPERTIES
  PUBLIC_HEADER "${PUBLIC_HEADERS}"
  POSITION_INDEPENDENT_CODE ON
)

include(GNUInstallDirs)
install(TARGETS rvnjsonresource
  EXPORT rvnjsonresource-export
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)

install(EXPORT rvnjsonresource-export
  FILE
    rvnjsonresource-config.cmake
  DESTINATION
    ${CMAKE_INSTALL_DATADIR}/cmake/rvnjsonresource
)

enable_testing()
add_subdirectory(test)
