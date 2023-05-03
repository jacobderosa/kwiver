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

set(ARROWS_MVG_CMAKE_DIR "${ARROWS_MVG_SOURCE_DIR}/CMake")
list(INSERT CMAKE_MODULE_PATH 0 ${KWIVER_CMAKE_DIR})

OPTION(ARROWS_MVG_ENABLE_TESTS                "Enable testing" OFF )
OPTION(ARROWS_MVG_BUILD_SHARED     "Build ARROWS-MVG components shared or not" TRUE )
MARK_AS_ADVANCED( ARROWS_MVG_BUILD_SHARED )
set(BUILD_SHARED_LIBS ${ARROWS_MVG_BUILD_SHARED})

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
  PUBLIC               kwiver::vital_algo
  PRIVATE              kwiver::kwiver_algo_core
                       kwiver::kwiversys
                       kwiver::vital_config
  )

target_include_directories( kwiver_algo_mvg SYSTEM
  PUBLIC                    ${KWIVER_INCLUDE_DIRS}
  )

algorithms_create_plugin( kwiver_algo_mvg
  register_algorithms.cxx
  )

# Add tests if enabled
if (ARROWS_MVG_ENABLE_TESTS)
  enable_testing()
  include(CTest)
  set(BUILD_TESTING    ON)  # needed for compatibility with kwiversys
  mark_as_advanced(BUILD_TESTING)
  add_subdirectory( tests )
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

kwiver_export_targets("${ARROWS_MVG_BINARY_DIR}/arrows-mvg-config-targets.cmake")

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