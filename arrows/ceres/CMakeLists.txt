# Build / Install plugin containing Ceres algorithm implementations + support
# structures

cmake_minimum_required(VERSION 3.15)

project(ARROWS_CERES)

###
# ARROWS_CERES version
set(ARROWS_CERES_VERSION_MAJOR 1)
set(ARROWS_CERES_VERSION_MINOR 7)
set(ARROWS_CERES_VERSION_PATCH 0)
set(ARROWS_CERES_VERSION "${ARROWS_CERES_VERSION_MAJOR}.${ARROWS_CERES_VERSION_MINOR}.${ARROWS_CERES_VERSION_PATCH}")

# Organize target into folders for IDEs that support it
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_FOLDER "Ceres")

# Default library dir
set(ARROWS_CERES_DEFAULT_LIBRARY_DIR "lib" CACHE STRING
    "Default library directory for arrows")
mark_as_advanced(ARROWS_CERES_DEFAULT_LIBRARY_DIR)

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
# Look for and use ARROWS MVG
#
set( arrows_MVG_DIR "" CACHE PATH "Path to build directory of ARROWS MVG" )
if ( IS_DIRECTORY ${arrows_MVG_DIR} )
  find_package( arrows_mvg NO_MODULE )
  if ( arrows_MVG_FOUND )
    # Search for libraries in the arrows mvg install root
    list(APPEND CMAKE_PREFIX_PATH "${ARROWS_MVG_DIR}")
  endif()
endif()

set(ARROWS_CERES_CMAKE_DIR "${ARROWS_CERES_SOURCE_DIR}/CMake")
list(INSERT CMAKE_MODULE_PATH 0 ${KWIVER_CMAKE_DIR})

OPTION(ARROWS_CERES_ENABLE_TESTS                "Enable testing" OFF )
OPTION(ARROWS_CERES_BUILD_SHARED     "Build ARROWS-CERES components shared or not" TRUE )
MARK_AS_ADVANCED( ARROWS_CERES_BUILD_SHARED )
set(BUILD_SHARED_LIBS ${ARROWS_CERES_BUILT_SHARED})

# Set directories where loadable modules are stored.
# These subdirs are under .../lib/
set( kwiver_plugin_subdir                         kwiver/plugins )
set( kwiver_plugin_process_subdir                 ${kwiver_plugin_subdir}/processes )
set( kwiver_plugin_algorithm_subdir               ${kwiver_plugin_subdir}/algorithms )
set( kwiver_plugin_process_instrumentation_subdir ${kwiver_plugin_subdir}/processes )
set( kwiver_plugin_scheduler_subdir               ${kwiver_plugin_subdir}/processes )
set( kwiver_plugin_module_subdir                  ${kwiver_plugin_subdir}/modules )
set( kwiver_plugin_plugin_explorer_subdir         ${kwiver_plugin_subdir}/plugin_explorer )
set( kwiver_plugin_logger_subdir                  ${kwiver_plugin_subdir}/logger )
set( kwiver_plugin_applets_subdir                 ${kwiver_plugin_subdir}/applets )

# Install rules for CMake utilities
#include( kwiver-install-utils )
include( kwiver-utils )
include( kwiver-depends )

set( ceres_headers_public
  bundle_adjust.h
  optimize_cameras.h
  reprojection_error.h
  camera_intrinsic_prior.h
  camera_position.h
  camera_smoothness.h
  )
set( ceres_headers_private
  lens_distortion.h
  options.h
  types.h
  )

kwiver_install_headers(
  SUBDIR     arrows/ceres
  ${ceres_headers_public}
  )

kwiver_install_headers(
  ${CMAKE_CURRENT_BINARY_DIR}/kwiver_algo_ceres_export.h
  NOPATH   SUBDIR     arrows/ceres
  )

set( ceres_sources
  options.cxx
  bundle_adjust.cxx
  optimize_cameras.cxx
  reprojection_error.cxx
  types.cxx
  )

kwiver_add_library( kwiver_algo_ceres
  ${ceres_headers_public}
  ${ceres_headers_private}
  ${ceres_sources}
  )
target_link_libraries( kwiver_algo_ceres
  PUBLIC               kwiver::vital_algo
  PRIVATE              ceres
                       kwiver::kwiver_algo_mvg
  )
if(NOT CERES_USES_MINIGLOG)
  target_link_libraries( kwiver_algo_ceres
    PRIVATE              glog
    )
endif()

target_include_directories( kwiver_algo_ceres SYSTEM
  PUBLIC                    ${KWIVER_INCLUDE_DIRS}
  )

algorithms_create_plugin( kwiver_algo_ceres
  register_algorithms.cxx
  )

# Add tests if enabled
if (ARROWS_CERES_ENABLE_TESTS)
  enable_testing()
  include(CTest)
  set(BUILD_TESTING    ON)  # needed for compatibility with kwiversys
  mark_as_advanced(BUILD_TESTING)
  include_directories(${KWIVER_TEST_INCLUDE_DIR})
  add_subdirectory( tests )
endif()

# Prepare space-separated list of library names for config
get_property(arrows_ceres_libs GLOBAL PROPERTY kwiver_libraries)
string(REPLACE ";" " " arrows_ceres_libs "${arrows_ceres_libs}")

set(ARROWS_CERES_CONFIG_FILE          "${ARROWS_CERES_BINARY_DIR}/arrows-ceres-config.cmake")
configure_file(
  "${ARROWS_CERES_CMAKE_DIR}/arrows-ceres-config-build.cmake.in"
  "${ARROWS_CONFIG_FILE}"
  @ONLY
  )

kwiver_export_targets("${ARROWS_CERES_BINARY_DIR}/arrows-ceres-config-targets.cmake")

#TODO: Define package dependencies
set(ARROWS_CERES_DEPS "")

if(EXISTS /etc/redhat-release)
  file(READ /etc/redhat-release RHEL_VERSION)
  string(REGEX REPLACE ".*release ([^\\. ]*).*" "\\1" RHEL_VERSION "${RHEL_VERSION}")
  set(CPACK_SYSTEM_NAME "el${RHEL_VERSION}.${CMAKE_SYSTEM_PROCESSOR}")
  set(CPACK_RPM_PACKAGE_AUTOREQPROV " no")
  set(CPACK_RPM_PACKAGE_REQUIRES "${ARROWS_CERES_DEPS}")
else()
  set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(CPACK_PACKAGE_NAME              "ARROWS")
set(CPACK_PACKAGE_VENDOR            "Kitware, Inc.")
set(CPACK_PACKAGE_CONTACT           "kitware@kitware.com")
set(CPACK_MONOLITHIC_INSTALL        true)
set(CPACK_PACKAGE_VERSION_MAJOR     "${ARROWS_CERES_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR     "${ARROWS_CERES_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH     "${ARROWS_CERES_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION           "${ARROWS_CERES_VERSION}")
#set(CPACK_RESOURCE_FILE_LICENSE     "${KWIVER_DIR}/LICENSE")
set(CPACK_PACKAGING_INSTALL_PREFIX  "${CMAKE_INSTALL_PREFIX}")
set(CPACK_PACKAGE_FILE_NAME         "${CPACK_PACKAGE_NAME}-"
                                    "${CPACK_PACKAGE_VERSION}-"
                                    "${CPACK_SYSTEM_NAME}")

include (CPack)