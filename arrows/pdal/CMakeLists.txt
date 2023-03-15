# Build / Install plugin containing PDAL algorithm implementations + support
# structures

cmake_minimum_required(VERSION 3.15)

project(ARROWS_PDAL)

###
# ARROWS-PDAL version
set(ARROWS_PDAL_VERSION_MAJOR 1)
set(ARROWS_PDAL_VERSION_MINOR 7)
set(ARROWS_PDAL_VERSION_PATCH 0)
set(ARROWS_PDAL_VERSION "${ARROWS_PDAL_VERSION_MAJOR}.${ARROWS_PDAL_VERSION_MINOR}.${ARROWS_PDAL_VERSION_PATCH}")

# Organize target into folders for IDEs that support it
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_FOLDER "PDAL")
set(ARROWS_PDAL_SOURCE_DIR           "${CMAKE_SOURCE_DIR}")
set(ARROWS_PDAL_BINARY_DIR           "${CMAKE_BINARY_DIR}")

# Default library dir
set(ARROWS_PDAL_DEFAULT_LIBRARY_DIR "lib" CACHE STRING
    "Default library directory for arrows")
mark_as_advanced(ARROWS_PDAL_DEFAULT_LIBRARY_DIR)

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

set(ARROWS_PDAL_CMAKE_DIR "${ARROWS_PDAL_SOURCE_DIR}/CMake")
list(INSERT CMAKE_MODULE_PATH 0 ${KWIVER_CMAKE_DIR})

OPTION(ARROWS_PDAL_ENABLE_TESTS                "Enable testing" OFF )
OPTION(ARROWS_PDAL_BUILD_SHARED     "Build ARROWS-PDAL components shared or not" TRUE )
MARK_AS_ADVANCED( ARROWS_PDAL_BUILD_SHARED )
set(BUILD_SHARED_LIBS ${ARROWS_PDAL_BUILT_SHARED})

# Install rules for CMake utilities
#include( kwiver-install-utils )
include( kwiver-utils )
include( kwiver-depends )

# project includes
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories(${KWIVER_INCLUDE_DIRS})

include_directories( SYSTEM ${CMAKE_CURRENT_SOURCE_DIR} )

set( pdal_headers_public
  algo/pointcloud_io.h
  )

kwiver_install_headers(
  SUBDIR     pdal
  ${pdal_headers_public}
  )

kwiver_install_headers(
  ${CMAKE_CURRENT_BINARY_DIR}/kwiver_algo_pdal_export.h
  NOPATH   SUBDIR     pdal
  )

set( pdal_sources
  algo/pointcloud_io.cxx
  )

kwiver_add_library( kwiver_algo_pdal SHARED
  ${pdal_headers_public}
  ${pdal_sources}
  )

target_link_libraries( kwiver_algo_pdal
  PUBLIC               vital_algo
  PRIVATE              ${PDAL_LIBRARIES}
  )

target_include_directories( kwiver_algo_pdal SYSTEM
  PRIVATE                   ${PDAL_INCLUDE_DIRS}
                            ${PDAL_INCLUDE_DIRS}/pdal
  )

algorithms_create_plugin( kwiver_algo_pdal
  register_algorithms.cxx
  )

if (ARROWS_PDAL_ENABLE_TESTS)
  enable_testing()
  include(CTest)
  set(BUILD_TESTING    ON)  # needed for compatibility with kwiversys
  mark_as_advanced(BUILD_TESTING)
  include_directories(${KWIVER_TEST_INCLUDE_DIR})
  add_subdirectory( tests )
endif()

# Prepare space-separated list of library names for config
get_property(arrows_pdal_libs GLOBAL PROPERTY kwiver_libraries)
string(REPLACE ";" " " arrows_pdal_libs "${arrows_pdal_libs}")

set(ARROWS_PDAL_CONFIG_FILE          "${ARROWS_PDAL_BINARY_DIR}/arrows-pdal-config.cmake")
configure_file(
  "${ARROWS_PDAL_CMAKE_DIR}/arrows-pdal-config-build.cmake.in"
  "${ARROWS_CONFIG_FILE}"
  @ONLY
  )

#TODO: Define package dependencies
set(ARROWS_PDAL_DEPS "")

if(EXISTS /etc/redhat-release)
  file(READ /etc/redhat-release RHEL_VERSION)
  string(REGEX REPLACE ".*release ([^\\. ]*).*" "\\1" RHEL_VERSION "${RHEL_VERSION}")
  set(CPACK_SYSTEM_NAME "el${RHEL_VERSION}.${CMAKE_SYSTEM_PROCESSOR}")
  set(CPACK_RPM_PACKAGE_AUTOREQPROV " no")
  set(CPACK_RPM_PACKAGE_REQUIRES "${ARROWS_PDAL_DEPS}")
else()
  set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(CPACK_PACKAGE_NAME              "ARROWS")
set(CPACK_PACKAGE_VENDOR            "Kitware, Inc.")
set(CPACK_PACKAGE_CONTACT           "kitware@kitware.com")
set(CPACK_MONOLITHIC_INSTALL        true)
set(CPACK_PACKAGE_VERSION_MAJOR     "${ARROWS_PDAL_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR     "${ARROWS_PDAL_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH     "${ARROWS_PDAL_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION           "${ARROWS_PDAL_VERSION}")
#set(CPACK_RESOURCE_FILE_LICENSE     "${KWIVER_DIR}/LICENSE")
set(CPACK_PACKAGING_INSTALL_PREFIX  "${CMAKE_INSTALL_PREFIX}")
set(CPACK_PACKAGE_FILE_NAME         "${CPACK_PACKAGE_NAME}-"
                                    "${CPACK_PACKAGE_VERSION}-"
                                    "${CPACK_SYSTEM_NAME}")

include (CPack)