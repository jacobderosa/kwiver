# Build / Install Multi-View Geometry arrow

cmake_minimum_required(VERSION 3.15)

project(ARROWS_MVG)

###
# ARROWS_MVG version
set(ARROWS_MVG_VERSION_MAJOR 1)
set(ARROWS_MVG_VERSION_MINOR 7)
set(ARROWS_MVG_VERSION_PATCH 0)
set(ARROWS_MVG_VERSION "${ARROWS_MVG_VERSION_MAJOR}.${ARROWS_MVG_VERSION_MINOR}.${ARROWS_MVG_VERSION_PATCH}")

# Organize target into folders for IDEs that support it
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_FOLDER "MVG")
set(ARROWS_MVG_SOURCE_DIR           "${CMAKE_SOURCE_DIR}")
set(ARROWS_MVG_BINARY_DIR           "${CMAKE_BINARY_DIR}")

# Default library dir
set(ARROWS_MVG_DEFAULT_LIBRARY_DIR "lib" CACHE STRING
    "Default library directory for arrows")
mark_as_advanced(ARROWS_MVG_DEFAULT_LIBRARY_DIR)

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

set(ARROWS_MVG_CMAKE_DIR "${ARROWS_MVG_SOURCE_DIR}/CMake")
list(INSERT CMAKE_MODULE_PATH 0 ${KWIVER_CMAKE_DIR})

set(BUILD_SHARED_LIBS ${KWIVER_BUILT_SHARED})

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

set( plugin_mvg_headers
  algo/hierarchical_bundle_adjust.h
  algo/integrate_depth_maps.h
  algo/initialize_cameras_landmarks.h
  algo/initialize_cameras_landmarks_basic.h
  algo/triangulate_landmarks.h

  camera_options.h
  epipolar_geometry.h
  interpolate_camera.h
  necker_reverse.h
  metrics.h
  projected_track_set.h
  sfm_utils.h
  transform.h
  triangulate.h
  )

kwiver_install_headers(
  SUBDIR     mvg
  ${plugin_mvg_headers}
  )

kwiver_install_headers(
  ${CMAKE_CURRENT_BINARY_DIR}/kwiver_algo_mvg_export.h
  NOPATH   SUBDIR     mvg
  )

set( plugin_mvg_sources
  algo/hierarchical_bundle_adjust.cxx
  algo/integrate_depth_maps.cxx
  algo/initialize_cameras_landmarks.cxx
  algo/initialize_cameras_landmarks_basic.cxx
  algo/triangulate_landmarks.cxx

  camera_options.cxx
  epipolar_geometry.cxx
  interpolate_camera.cxx
  necker_reverse.cxx
  metrics.cxx
  projected_track_set.cxx
  sfm_utils.cxx
  transform.cxx
  triangulate.cxx
  )

# Address MSVC fatal error C1128:
# number of sections exceeded object file format limit
if(MSVC)
  set_source_files_properties(
    triangulate.cxx
    PROPERTIES COMPILE_FLAGS "/bigobj"
    )
endif()


kwiver_add_library( kwiver_algo_mvg
  ${plugin_mvg_headers}
  ${plugin_mvg_sources}
  )

target_link_libraries( kwiver_algo_mvg
  PUBLIC               vital_algo
  PRIVATE              kwiver_algo_core
                       kwiversys
                       vital_config
  )

algorithms_create_plugin( kwiver_algo_mvg
  register_algorithms.cxx
  )

if (MVG_ENABLE_TESTS)
  include_directories(${KWIVER_TEST_INCLUDE_DIR})
  add_subdirectory(tests)
endif()

if (KWIVER_ENABLE_TOOLS)
  add_subdirectory(applets)
endif()

# Prepare space-separated list of library names for config
get_property(arrows_mvg_libs GLOBAL PROPERTY kwiver_libraries)
string(REPLACE ";" " " arrows_mvg_libs "${arrows_mvg_libs}")

set(ARROWS_MVG_CONFIG_FILE          "${ARROWS_MVG_BINARY_DIR}/arrows-mvg-config.cmake")
configure_file(
  "${ARROWS_MVG_CMAKE_DIR}/arrows-mvg-config-build.cmake.in"
  "${ARROWS_CONFIG_FILE}"
  @ONLY
  )

#TODO: Define package dependencies
set(ARROWS_MVG_DEPS "")

if(EXISTS /etc/redhat-release)
  file(READ /etc/redhat-release RHEL_VERSION)
  string(REGEX REPLACE ".*release ([^\\. ]*).*" "\\1" RHEL_VERSION "${RHEL_VERSION}")
  set(CPACK_SYSTEM_NAME "el${RHEL_VERSION}.${CMAKE_SYSTEM_PROCESSOR}")
  set(CPACK_RPM_PACKAGE_AUTOREQPROV " no")
  set(CPACK_RPM_PACKAGE_REQUIRES "${ARROWS_MVG_DEPS}")
else()
  set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(CPACK_PACKAGE_NAME              "ARROWS")
set(CPACK_PACKAGE_VENDOR            "Kitware, Inc.")
set(CPACK_PACKAGE_CONTACT           "kitware@kitware.com")
set(CPACK_MONOLITHIC_INSTALL        true)
set(CPACK_PACKAGE_VERSION_MAJOR     "${ARROWS_MVG_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR     "${ARROWS_MVG_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH     "${ARROWS_MVG_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION           "${ARROWS_MVG_VERSION}")
#set(CPACK_RESOURCE_FILE_LICENSE     "${KWIVER_DIR}/LICENSE")
set(CPACK_PACKAGING_INSTALL_PREFIX  "${CMAKE_INSTALL_PREFIX}")
set(CPACK_PACKAGE_FILE_NAME         "${CPACK_PACKAGE_NAME}-"
                                    "${CPACK_PACKAGE_VERSION}-"
                                    "${CPACK_SYSTEM_NAME}")

include (CPack)