#
# Arrows Algorithms CMake file
#

cmake_minimum_required(VERSION 3.15)

project(ARROWS)

###
# KWIVER-ARROWS version
set(ARROWS_VERSION_MAJOR 1)
set(ARROWS_VERSION_MINOR 7)
set(ARROWS_VERSION_PATCH 0)
set(ARROWS_VERSION "${ARROWS_VERSION_MAJOR}.${ARROWS_VERSION_MINOR}.${ARROWS_VERSION_PATCH}")

# Organize target into folders for IDEs that support it
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

###
# Look for and use KWIVER
#
set( kwiver_DIR "" CACHE PATH "Path to build directory of KWIVER" )
if ( IS_DIRECTORY ${kwiver_DIR} )
  find_package( kwiver NO_MODULE )
  if ( kwiver_FOUND )
    # Search for libraries in the kwiver install root
    list(APPEND CMAKE_PREFIX_PATH "${kwiver_DIR}")
    list(APPEND CMAKE_PREFIX_PATH "${fletch_DIR}")
  endif()
endif()

# Default library dir
set(ARROWS_DEFAULT_LIBRARY_DIR "lib" CACHE STRING
    "Default library directory for arrows")
mark_as_advanced(ARROWS_DEFAULT_LIBRARY_DIR)

set(ARROWS_CMAKE_DIR "${ARROWS_SOURCE_DIR}/CMake")
list(INSERT CMAKE_MODULE_PATH 0 ${KWIVER_CMAKE_DIR})
set( kwiver_plugin_algorithm_subdir               ${KWIVER_PLUGIN_ALGORITHM_DIR} )

OPTION(ARROWS_BUILD_SHARED     "Build ARROWS components shared or not" TRUE )
MARK_AS_ADVANCED( ARROWS_BUILD_SHARED )
set(BUILD_SHARED_LIBS ${ARROWS_BUILD_SHARED})

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

# project includes
include_directories("${ARROWS_SOURCE_DIR}")
include_directories("${ARROWS_BINARY_DIR}")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories(${KWIVER_INCLUDE_DIRS})
include_directories("/kwiver/vital/internal")

include_directories( SYSTEM ${CMAKE_CURRENT_SOURCE_DIR} )

# Base plug-in with default implementations
add_subdirectory( core )

add_subdirectory( serialize )

# Prepare space-separated list of library names for config
get_property(arrows_libs GLOBAL PROPERTY kwiver_libraries)
string(REPLACE ";" " " arrows_libs "${arrows_libs}")

set(ARROWS_CONFIG_FILE          "${ARROWS_BINARY_DIR}/arrows-config.cmake")
configure_file(
  "${ARROWS_CMAKE_DIR}/arrows-config-build.cmake.in"
  "${ARROWS_CONFIG_FILE}"
  @ONLY
  )

# Configure install-tree CMake config file and export associated targets file
#set(ARROWS_CONFIG_INSTALL_FILE "${ARROWS_BINARY_DIR}/arrows-config-install.cmake")
#configure_file(
#  "${ARROWS_CMAKE_DIR}/arrows-config-install.cmake.in"
#  "${ARROWS_CONFIG_INSTALL_FILE}"
#  @ONLY
#  )

#kwiver_export_targets("${ARROWS_BINARY_DIR}/arrows-config-targets.cmake")

#if(NOT SKBUILD)
#  kwiver_install(
#    FILES       "${ARROWS_CONFIG_INSTALL_FILE}"
#    DESTINATION "${arrows_cmake_install_dir}"
#    RENAME      arrows-config.cmake
#    )

#  kwiver_install(
#    EXPORT      ${arrows_export_name}
#    NAMESPACE   arrows::
#    DESTINATION "${arrows_cmake_install_dir}"
#    FILE        arrows-config-targets.cmake
#    )
#endif()

kwiver_export_targets("${ARROWS_BINARY_DIR}/arrows-config-targets.cmake")

#TODO: Define package dependencies
set(ARROWS_DEPS "")

if(EXISTS /etc/redhat-release)
  file(READ /etc/redhat-release RHEL_VERSION)
  string(REGEX REPLACE ".*release ([^\\. ]*).*" "\\1" RHEL_VERSION "${RHEL_VERSION}")
  set(CPACK_SYSTEM_NAME "el${RHEL_VERSION}.${CMAKE_SYSTEM_PROCESSOR}")
  set(CPACK_RPM_PACKAGE_AUTOREQPROV " no")
  set(CPACK_RPM_PACKAGE_REQUIRES "${ARROWS_DEPS}")
else()
  set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(CPACK_PACKAGE_NAME              "ARROWS")
set(CPACK_PACKAGE_VENDOR            "Kitware, Inc.")
set(CPACK_PACKAGE_CONTACT           "kitware@kitware.com")
set(CPACK_MONOLITHIC_INSTALL        true)
set(CPACK_PACKAGE_VERSION_MAJOR     "${ARROWS_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR     "${ARROWS_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH     "${ARROWS_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION           "${ARROWS_VERSION}")
#set(CPACK_RESOURCE_FILE_LICENSE     "${KWIVER_DIR}/LICENSE")
set(CPACK_PACKAGING_INSTALL_PREFIX  "${CMAKE_INSTALL_PREFIX}")
set(CPACK_PACKAGE_FILE_NAME         "${CPACK_PACKAGE_NAME}-"
                                    "${CPACK_PACKAGE_VERSION}-"
                                    "${CPACK_SYSTEM_NAME}")

include (CPack)