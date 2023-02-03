# Build / Install plugin containing GDAL algorithm implementations + support
# structures

cmake_minimum_required(VERSION 3.15)

project(ARROWS_GDAL)

set(CMAKE_FOLDER "GDAL")

###
# Look for and use KWIVER
#
set( kwiver_DIR "" CACHE PATH "Path to build directory of KWIVER" )
if ( IS_DIRECTORY ${kwiver_DIR} )
  find_package( kwiver NO_MODULE )
  if ( kwiver_FOUND )
    # Search for libraries in the kwiver install root
    list(APPEND CMAKE_PREFIX_PATH "${KWIVER_DIR}")
    list(APPEND CMAKE_PREFIX_PATH "${fletch_DIR}")
  endif()
endif()

###
# Look for and use ARROWS CORE
#
set( arrows_DIR "" CACHE PATH "Path to build directory of ARROWS" )
if ( IS_DIRECTORY ${arrows_DIR} )
  find_package( arrows NO_MODULE )
  if ( arrows_FOUND )
    # Search for libraries in the arrows install root
    list(APPEND CMAKE_PREFIX_PATH "${ARROWS_DIR}")
  endif()
endif()

set(ARROWS_CMAKE_DIR "${ARROWS_SOURCE_DIR}/CMake")
list(INSERT CMAKE_MODULE_PATH 0 ${KWIVER_CMAKE_DIR})

set(BUILD_SHARED_LIBS ${KWIVER_BUILD_SHARED})

# Install rules for CMake utilities
#include( kwiver-install-utils )
include( kwiver-utils )
include( kwiver-depends )

# project includes
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories(${KWIVER_INCLUDE_DIRS})
include_directories(${ARROWS_INCLUDE_DIRS})
include_directories("/kwiver/vital/internal")

include_directories( SYSTEM ${CMAKE_CURRENT_SOURCE_DIR} )

set( gdal_headers_public
  image_container.h
  image_io.h
  )

kwiver_install_headers(
  SUBDIR     gdal
  ${gdal_headers_public}
  )

kwiver_install_headers(
  ${CMAKE_CURRENT_BINARY_DIR}/kwiver_algo_gdal_export.h
  NOPATH   SUBDIR     gdal
  )

set( gdal_sources
  image_container.cxx
  image_io.cxx
  )

kwiver_add_library( kwiver_algo_gdal
  ${gdal_headers_public}
  ${gdal_sources}
  )
target_link_libraries( kwiver_algo_gdal
  PUBLIC               vital_algo
  PRIVATE              ${GDAL_LIBRARY}
  )

algorithms_create_plugin( kwiver_algo_gdal
  register_algorithms.cxx
  )

if (KWIVER_ENABLE_TESTS)
  add_subdirectory(tests)
endif()
