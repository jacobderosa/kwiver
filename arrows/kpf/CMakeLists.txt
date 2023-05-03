cmake_minimum_required(VERSION 3.15)
project(ARROWS_KPF)

###
# ARROWS-KPF version
set(ARROWS_KPF_VERSION_MAJOR 1)
set(ARROWS_KPF_VERSION_MINOR 7)
set(ARROWS_KPF_VERSION_PATCH 0)
set(ARROWS_KPF_VERSION "${ARROWS_KPF_VERSION_MAJOR}.${ARROWS_KPF_VERSION_MINOR}.${ARROWS_KPF_VERSION_PATCH}")

# Organize target into folders for IDEs that support it
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_FOLDER "KPF")

# Default library dir
set(ARROWS_KPF_DEFAULT_LIBRARY_DIR "lib" CACHE STRING
    "Default library directory for arrows")
mark_as_advanced(ARROWS_KPF_DEFAULT_LIBRARY_DIR)

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

set(ARROWS_KPF_CMAKE_DIR "${ARROWS_KPF_SOURCE_DIR}/CMake")
list(INSERT CMAKE_MODULE_PATH 0 ${KWIVER_CMAKE_DIR})

OPTION(ARROWS_KPF_ENABLE_TESTS                "Enable testing" OFF )
OPTION(ARROWS_KPF_BUILD_SHARED     "Build ARROWS-KPF components shared or not" TRUE )
MARK_AS_ADVANCED( ARROWS_KPF_BUILD_SHARED )
set(BUILD_SHARED_LIBS ${ARROWS_KPF_BUILT_SHARED})

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


find_package( yaml-cpp REQUIRED )

add_subdirectory(yaml)

# Build/install plugin for kpf support for vital objects
#

set( headers
  detected_object_set_input_kpf.h
  detected_object_set_output_kpf.h
)

set( sources
  vital_kpf_adapters.h
  detected_object_set_input_kpf.cxx
  detected_object_set_output_kpf.cxx
  )

set( kpf_linked_libs
  kwiversys)


kwiver_install_headers(
  SUBDIR     arrows/kpf
  ${headers}
  )
kwiver_install_headers(
  ${CMAKE_CURRENT_BINARY_DIR}/kwiver_algo_kpf_export.h
  NOPATH   SUBDIR     arrows/kpf
  )


kwiver_add_library( kwiver_algo_kpf
  ${headers}
  ${sources}
  )

target_link_libraries( kwiver_algo_kpf
  PUBLIC               kwiver::vital_algo kpf_yaml
  PRIVATE              ${YAML_CPP_LIBRARIES}
  )

target_include_directories( kwiver_algo_kpf SYSTEM
  PUBLIC                    ${KWIVER_INCLUDE_DIRS}
  )

algorithms_create_plugin( kwiver_algo_kpf
  register_algorithms.cxx
  )

# Python Support?
#if( KWIVER_ENABLE_PYTHON )
#  kwiver_create_python_init( kwiver/arrows/kpf )
#
#  kwiver_add_python_module( ${CMAKE_CURRENT_SOURCE_DIR}/generate_headers.py
#    kwiver/arrows/kpf
#    generate_headers )
#endif()

# Prepare space-separated list of library names for config
get_property(arrows_kpf_libs GLOBAL PROPERTY kwiver_libraries)
string(REPLACE ";" " " arrows_kpf_libs "${arrows_kpf_libs}")

set(ARROWS_KPF_CONFIG_FILE          "${ARROWS_KPF_BINARY_DIR}/arrows-kpf-config.cmake")
configure_file(
  "${ARROWS_KPF_CMAKE_DIR}/arrows-kpf-config-build.cmake.in"
  "${ARROWS_CONFIG_FILE}"
  @ONLY
  )

kwiver_export_targets("${ARROWS_KPF_BINARY_DIR}/arrows-kpf-config-targets.cmake")

#TODO: Define package dependencies
set(ARROWS_KPF_DEPS "")

if(EXISTS /etc/redhat-release)
  file(READ /etc/redhat-release RHEL_VERSION)
  string(REGEX REPLACE ".*release ([^\\. ]*).*" "\\1" RHEL_VERSION "${RHEL_VERSION}")
  set(CPACK_SYSTEM_NAME "el${RHEL_VERSION}.${CMAKE_SYSTEM_PROCESSOR}")
  set(CPACK_RPM_PACKAGE_AUTOREQPROV " no")
  set(CPACK_RPM_PACKAGE_REQUIRES "${ARROWS_KPF_DEPS}")
else()
  set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(CPACK_PACKAGE_NAME              "ARROWS")
set(CPACK_PACKAGE_VENDOR            "Kitware, Inc.")
set(CPACK_PACKAGE_CONTACT           "kitware@kitware.com")
set(CPACK_MONOLITHIC_INSTALL        true)
set(CPACK_PACKAGE_VERSION_MAJOR     "${ARROWS_KPF_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR     "${ARROWS_KPF_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH     "${ARROWS_KPF_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION           "${ARROWS_KPF_VERSION}")
#set(CPACK_RESOURCE_FILE_LICENSE     "${KWIVER_DIR}/LICENSE")
set(CPACK_PACKAGING_INSTALL_PREFIX  "${CMAKE_INSTALL_PREFIX}")
set(CPACK_PACKAGE_FILE_NAME         "${CPACK_PACKAGE_NAME}-"
                                    "${CPACK_PACKAGE_VERSION}-"
                                    "${CPACK_SYSTEM_NAME}")

include (CPack)