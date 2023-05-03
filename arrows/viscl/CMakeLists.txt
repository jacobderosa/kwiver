# Build / install plugin containing VisCL algorithm implementations + support
# structures

cmake_minimum_required(VERSION 3.15)

project(ARROWS_VISCL)

###
# ARROWS_VISCL version
set(ARROWS_VISCL_VERSION_MAJOR 1)
set(ARROWS_VISCL_VERSION_MINOR 7)
set(ARROWS_VISCL_VERSION_PATCH 0)
set(ARROWS_VISCL_VERSION "${ARROWS_VISCL_VERSION_MAJOR}.${ARROWS_VISCL_VERSION_MINOR}.${ARROWS_VISCL_VERSION_PATCH}")

# Organize target into folders for IDEs that support it
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_FOLDER "VISCL")

# Default library dir
set(ARROWS_VISCL_DEFAULT_LIBRARY_DIR "lib" CACHE STRING
    "Default library directory for arrows")
mark_as_advanced(ARROWS_VISCL_DEFAULT_LIBRARY_DIR)

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

set(ARROWS_VISCL_CMAKE_DIR "${ARROWS_VISCL_SOURCE_DIR}/CMake")
list(INSERT CMAKE_MODULE_PATH 0 ${KWIVER_CMAKE_DIR})

OPTION(ARROWS_VISCL_ENABLE_TESTS                "Enable testing" OFF )
OPTION(ARROWS_VISCL_BUILD_SHARED     "Build ARROWS-VISCL components shared or not" TRUE )
MARK_AS_ADVANCED( ARROWS_VISCL_BUILD_SHARED )
set(BUILD_SHARED_LIBS ${ARROWS_VISCL_BUILT_SHARED})

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

find_package(Boost) ##+ temp

set( viscl_headers_public
  convert_image.h
  descriptor_set.h
  detect_features.h
  extract_descriptors.h
  feature_set.h
  image_container.h
  match_features.h
  match_set.h
  utils.h
  )
kwiver_install_headers(
  ${viscl_headers_public}
  SUBDIR     arrows/viscl
  )

kwiver_install_headers(
  ${CMAKE_CURRENT_BINARY_DIR}/kwiver_algo_viscl_export.h
  NOPATH   SUBDIR     arrows/viscl
  )

set( viscl_sources
  convert_image.cxx
  descriptor_set.cxx
  detect_features.cxx
  extract_descriptors.cxx
  feature_set.cxx
  image_container.cxx
  match_features.cxx
  match_set.cxx
  utils.cxx
  )

kwiver_add_library( kwiver_algo_viscl
  ${viscl_headers_public}
  ${viscl_sources}
  )

target_link_libraries( kwiver_algo_viscl
  PUBLIC           kwiver::vital_algo
                   viscl_core viscl_tasks viscl_vxl
  )

target_include_directories( kwiver_algo_viscl SYSTEM
  PUBLIC                    ${KWIVER_INCLUDE_DIRS}
  )

algorithms_create_plugin( kwiver_algo_viscl
  register_algorithms.cxx
  )

# Prepare space-separated list of library names for config
get_property(arrows_viscl_libs GLOBAL PROPERTY kwiver_libraries)
string(REPLACE ";" " " arrows_viscl_libs "${arrows_viscl_libs}")

set(ARROWS_VISCL_CONFIG_FILE          "${ARROWS_VISCL_BINARY_DIR}/arrows-viscl-config.cmake")
configure_file(
  "${ARROWS_VISCL_CMAKE_DIR}/arrows-viscl-config-build.cmake.in"
  "${ARROWS_CONFIG_FILE}"
  @ONLY
  )

kwiver_export_targets("${ARROWS_VISCL_BINARY_DIR}/arrows-viscl-config-targets.cmake")

#TODO: Define package dependencies
set(ARROWS_VISCL_DEPS "")

if(EXISTS /etc/redhat-release)
  file(READ /etc/redhat-release RHEL_VERSION)
  string(REGEX REPLACE ".*release ([^\\. ]*).*" "\\1" RHEL_VERSION "${RHEL_VERSION}")
  set(CPACK_SYSTEM_NAME "el${RHEL_VERSION}.${CMAKE_SYSTEM_PROCESSOR}")
  set(CPACK_RPM_PACKAGE_AUTOREQPROV " no")
  set(CPACK_RPM_PACKAGE_REQUIRES "${ARROWS_VISCL_DEPS}")
else()
  set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(CPACK_PACKAGE_NAME              "ARROWS")
set(CPACK_PACKAGE_VENDOR            "Kitware, Inc.")
set(CPACK_PACKAGE_CONTACT           "kitware@kitware.com")
set(CPACK_MONOLITHIC_INSTALL        true)
set(CPACK_PACKAGE_VERSION_MAJOR     "${ARROWS_VISCL_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR     "${ARROWS_VISCL_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH     "${ARROWS_VISCL_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION           "${ARROWS_VISCL_VERSION}")
#set(CPACK_RESOURCE_FILE_LICENSE     "${KWIVER_DIR}/LICENSE")
set(CPACK_PACKAGING_INSTALL_PREFIX  "${CMAKE_INSTALL_PREFIX}")
set(CPACK_PACKAGE_FILE_NAME         "${CPACK_PACKAGE_NAME}-"
                                    "${CPACK_PACKAGE_VERSION}-"
                                    "${CPACK_SYSTEM_NAME}")

include (CPack)