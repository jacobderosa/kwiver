# Build / Install Super3D arrow

cmake_minimum_required(VERSION 3.15)

project(ARROWS_SUPER3D)

###
# ARROWS_SUPER3D version
set(ARROWS_SUPER3D_VERSION_MAJOR 1)
set(ARROWS_SUPER3D_VERSION_MINOR 7)
set(ARROWS_SUPER3D_VERSION_PATCH 0)
set(ARROWS_SUPER3D_VERSION "${ARROWS_SUPER3D_VERSION_MAJOR}.${ARROWS_SUPER3D_VERSION_MINOR}.${ARROWS_SUPER3D_VERSION_PATCH}")

# Organize target into folders for IDEs that support it
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_FOLDER "Super3D")
set(ARROWS_SUPER3D_SOURCE_DIR           "${CMAKE_SOURCE_DIR}")
set(ARROWS_SUPER3D_BINARY_DIR           "${CMAKE_BINARY_DIR}")

# Default library dir
set(ARROWS_SUPER3D_DEFAULT_LIBRARY_DIR "lib" CACHE STRING
    "Default library directory for arrows")
mark_as_advanced(ARROWS_SUPER3D_DEFAULT_LIBRARY_DIR)

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
# Look for and use ARROWS VXL
#
set( arrows_VXL_DIR "" CACHE PATH "Path to build directory of ARROWS VXL" )
if ( IS_DIRECTORY ${arrows_VXL_DIR} )
  find_package( arrows_vxl NO_MODULE )
  if ( kwiver_FOUND )
    # Search for libraries in the arrows vxl install root
    list(APPEND CMAKE_PREFIX_PATH "${ARROWS_VXL_DIR}")
  endif()
endif()

set(ARROWS_SUPER3D_CMAKE_DIR "${ARROWS_SUPER3D_SOURCE_DIR}/CMake")
list(INSERT CMAKE_MODULE_PATH 0 ${KWIVER_CMAKE_DIR})

OPTION(ARROWS_SUPER3D_BUILD_SHARED     "Build ARROWS-SUPER3D components shared or not" TRUE )
MARK_AS_ADVANCED( ARROWS_SUPER3D_BUILD_SHARED )
set(BUILD_SHARED_LIBS ${ARROWS_SUPER3D_BUILT_SHARED})

set( kwiver_plugin_algorithm_subdir               ${KWIVER_PLUGIN_ALGORITHM_DIR} )

# Install rules for CMake utilities
#include( kwiver-install-utils )
include( kwiver-utils )
include( kwiver-depends )

# project includes
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories(${KWIVER_INCLUDE_DIRS})
include_directories(${ARROWS_VXL_INCLUDE_DIRS})

include_directories( SYSTEM ${CMAKE_CURRENT_SOURCE_DIR} )

set(header_files
  compute_depth.h
  cost_volume.h
  util.h
  tv_refine_search.h
  warp_image.h
  world_angled_frustum.h
  world_space.h
  )

set(cxx_files
  compute_depth.cxx
  cost_volume.cxx
  util.cxx
  tv_refine_search.cxx
  warp_image.hxx
  warp_image.cxx
  warp_image_instances.cxx
  world_angled_frustum.cxx
  world_space.cxx
  )

kwiver_install_headers(
  SUBDIR     super3d
  ${header_files}
  )

kwiver_install_headers(
  ${CMAKE_CURRENT_BINARY_DIR}/kwiver_algo_super3d_export.h
  NOPATH   SUBDIR     super3d
  )

kwiver_add_library(kwiver_algo_super3d
  ${header_files}
  ${cxx_files}
  )
target_link_libraries(kwiver_algo_super3d
  PUBLIC              vital_algo
                      vil vpgl vnl vgl_algo
  PRIVATE             kwiver_algo_vxl
                      vil_algo vbl
  )

algorithms_create_plugin( kwiver_algo_super3d
  register_algorithms.cxx
  )

# Prepare space-separated list of library names for config
get_property(arrows_super3d_libs GLOBAL PROPERTY kwiver_libraries)
string(REPLACE ";" " " arrows_super3d_libs "${arrows_super3d_libs}")

set(ARROWS_SUPER3D_CONFIG_FILE          "${ARROWS_SUPER3D_BINARY_DIR}/arrows-super3d-config.cmake")
configure_file(
  "${ARROWS_SUPER3D_CMAKE_DIR}/arrows-super3d-config-build.cmake.in"
  "${ARROWS_CONFIG_FILE}"
  @ONLY
  )

#TODO: Define package dependencies
set(ARROWS_SUPER3D_DEPS "")

if(EXISTS /etc/redhat-release)
  file(READ /etc/redhat-release RHEL_VERSION)
  string(REGEX REPLACE ".*release ([^\\. ]*).*" "\\1" RHEL_VERSION "${RHEL_VERSION}")
  set(CPACK_SYSTEM_NAME "el${RHEL_VERSION}.${CMAKE_SYSTEM_PROCESSOR}")
  set(CPACK_RPM_PACKAGE_AUTOREQPROV " no")
  set(CPACK_RPM_PACKAGE_REQUIRES "${ARROWS_SUPER3D_DEPS}")
else()
  set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(CPACK_PACKAGE_NAME              "ARROWS")
set(CPACK_PACKAGE_VENDOR            "Kitware, Inc.")
set(CPACK_PACKAGE_CONTACT           "kitware@kitware.com")
set(CPACK_MONOLITHIC_INSTALL        true)
set(CPACK_PACKAGE_VERSION_MAJOR     "${ARROWS_SUPER3D_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR     "${ARROWS_SUPER3D_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH     "${ARROWS_SUPER3D_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION           "${ARROWS_SUPER3D_VERSION}")
#set(CPACK_RESOURCE_FILE_LICENSE     "${KWIVER_DIR}/LICENSE")
set(CPACK_PACKAGING_INSTALL_PREFIX  "${CMAKE_INSTALL_PREFIX}")
set(CPACK_PACKAGE_FILE_NAME         "${CPACK_PACKAGE_NAME}-"
                                    "${CPACK_PACKAGE_VERSION}-"
                                    "${CPACK_SYSTEM_NAME}")

include (CPack)