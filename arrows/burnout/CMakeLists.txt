# Build/install plugin for burnout CNN support
#

cmake_minimum_required(VERSION 3.15)

project(ARROWS_BURNOUT)

###
# ARROWS_BURNOUT version
set(ARROWS_BURNOUT_VERSION_MAJOR 1)
set(ARROWS_BURNOUT_VERSION_MINOR 7)
set(ARROWS_BURNOUT_VERSION_PATCH 0)
set(ARROWS_BURNOUT_VERSION "${ARROWS_BURNOUT_VERSION_MAJOR}.${ARROWS_BURNOUT_VERSION_MINOR}.${ARROWS_BURNOUT_VERSION_PATCH}")

# Organize target into folders for IDEs that support it
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_FOLDER "BurnOut")
set(ARROWS_BURNOUT_SOURCE_DIR           "${CMAKE_SOURCE_DIR}")
set(ARROWS_BURNOUT_BINARY_DIR           "${CMAKE_BINARY_DIR}")

# Default library dir
set(ARROWS_BURNOUT_DEFAULT_LIBRARY_DIR "lib" CACHE STRING
    "Default library directory for arrows")
mark_as_advanced(ARROWS_BURNOUT_DEFAULT_LIBRARY_DIR)

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
# Look for and use ARROWS OCV
#
set( arrows_OCV_DIR "" CACHE PATH "Path to build directory of ARROWS OCV" )
if ( IS_DIRECTORY ${arrows_OCV_DIR} )
  find_package( arrows_ocv NO_MODULE )
  if ( arrows_OCV_FOUND )
    # Search for libraries in the arrows ocv install root
    list(APPEND CMAKE_PREFIX_PATH "${ARROWS_OCV_DIR}")
  endif()
endif()

###
# Look for and use ARROWS VXL
#
set( arrows_VXL_DIR "" CACHE PATH "Path to build directory of ARROWS VXL" )
if ( IS_DIRECTORY ${arrows_VXL_DIR} )
  find_package( arrows_vxl NO_MODULE )
  if ( arrows_vxl_FOUND )
    # Search for libraries in the arrows vxl install root
    list(APPEND CMAKE_PREFIX_PATH "${ARROWS_VXL_DIR}")
  endif()
endif()

set(ARROWS_BURNOUT_CMAKE_DIR "${ARROWS_BURNOUT_SOURCE_DIR}/CMake")
list(INSERT CMAKE_MODULE_PATH 0 ${KWIVER_CMAKE_DIR})

OPTION(ARROWS_BURNOUT_BUILD_SHARED     "Build ARROWS-BURNOUT components shared or not" TRUE )
MARK_AS_ADVANCED( ARROWS_BURNOUT_BUILD_SHARED )
set(BUILD_SHARED_LIBS ${ARROWS_BURNOUT_BUILT_SHARED})

set( kwiver_plugin_algorithm_subdir               ${KWIVER_PLUGIN_ALGORITHM_DIR} )

# Install rules for CMake utilities
#include( kwiver-install-utils )
include( kwiver-utils )
include( kwiver-depends )

# project includes
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories(${KWIVER_INCLUDE_DIRS})
include_directories(${ARROWS_OCV_INCLUDE_DIRS})
include_directories($(ARROWS_VXL_INCLUDE_DIRS))

include_directories( SYSTEM ${CMAKE_CURRENT_SOURCE_DIR} )

set( headers
  burnout_image_enhancer.h
  burnout_pixel_classification.h
  burnout_track_descriptors.h
  )

set( sources
  burnout_image_enhancer.cxx
  burnout_pixel_classification.cxx
  burnout_track_descriptors.cxx
  )

include_directories( SYSTEM ${VIDTK_INCLUDE_DIRS} )

kwiver_install_headers(
  SUBDIR     burnout
  ${headers}
  )
kwiver_install_headers(
  ${CMAKE_CURRENT_BINARY_DIR}/kwiver_algo_burnout_export.h
  NOPATH   SUBDIR     burnout
  )

kwiver_add_library( kwiver_algo_burnout
  ${headers}
  ${sources}
  )

target_link_libraries( kwiver_algo_burnout
  PUBLIC               vital_algo
  PRIVATE              vidtk_descriptor vidtk_utilities_no_process vidtk_pipelines
                       kwiversys
                       ${Boost_SYSTEM_LIBRARY}
                       ${Boost_FILESYSTEM_LIBRARY}
                       kwiver_algo_ocv
                       kwiver_algo_vxl
  )

algorithms_create_plugin( kwiver_algo_burnout
  register_algorithms.cxx
  )

  # Prepare space-separated list of library names for config
get_property(arrows_burnout_libs GLOBAL PROPERTY kwiver_libraries)
string(REPLACE ";" " " arrows_burnout_libs "${arrows_burnout_libs}")

set(ARROWS_BURNOUT_CONFIG_FILE          "${ARROWS_BURNOUT_BINARY_DIR}/arrows-burnout-config.cmake")
configure_file(
  "${ARROWS_BURNOUT_CMAKE_DIR}/arrows-burnout-config-build.cmake.in"
  "${ARROWS_CONFIG_FILE}"
  @ONLY
  )

#TODO: Define package dependencies
set(ARROWS_BURNOUT_DEPS "")

if(EXISTS /etc/redhat-release)
  file(READ /etc/redhat-release RHEL_VERSION)
  string(REGEX REPLACE ".*release ([^\\. ]*).*" "\\1" RHEL_VERSION "${RHEL_VERSION}")
  set(CPACK_SYSTEM_NAME "el${RHEL_VERSION}.${CMAKE_SYSTEM_PROCESSOR}")
  set(CPACK_RPM_PACKAGE_AUTOREQPROV " no")
  set(CPACK_RPM_PACKAGE_REQUIRES "${ARROWS_BURNOUT_DEPS}")
else()
  set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(CPACK_PACKAGE_NAME              "ARROWS")
set(CPACK_PACKAGE_VENDOR            "Kitware, Inc.")
set(CPACK_PACKAGE_CONTACT           "kitware@kitware.com")
set(CPACK_MONOLITHIC_INSTALL        true)
set(CPACK_PACKAGE_VERSION_MAJOR     "${ARROWS_BURNOUT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR     "${ARROWS_BURNOUT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH     "${ARROWS_BURNOUT_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION           "${ARROWS_BURNOUT_VERSION}")
#set(CPACK_RESOURCE_FILE_LICENSE     "${KWIVER_DIR}/LICENSE")
set(CPACK_PACKAGING_INSTALL_PREFIX  "${CMAKE_INSTALL_PREFIX}")
set(CPACK_PACKAGE_FILE_NAME         "${CPACK_PACKAGE_NAME}-"
                                    "${CPACK_PACKAGE_VERSION}-"
                                    "${CPACK_SYSTEM_NAME}")

include (CPack)
