cmake_minimum_required(VERSION 3.15)

project(ARROWS_COLMAP)

###
# ARROWS-COLMAP version
set(ARROWS_COLMAP_VERSION_MAJOR 1)
set(ARROWS_COLMAP_VERSION_MINOR 7)
set(ARROWS_COLMAP_VERSION_PATCH 0)
set(ARROWS_COLMAP_VERSION "${ARROWS_COLMAP_VERSION_MAJOR}.${ARROWS_COLMAP_VERSION_MINOR}.${ARROWS_COLMAP_VERSION_PATCH}")

# Organize target into folders for IDEs that support it
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_FOLDER "COLMAP")
set(ARROWS_COLMAP_SOURCE_DIR           "${CMAKE_SOURCE_DIR}")
set(ARROWS_COLMAP_BINARY_DIR           "${CMAKE_BINARY_DIR}")

# Default library dir
set(ARROWS_COLMAP_DEFAULT_LIBRARY_DIR "lib" CACHE STRING
    "Default library directory for arrows")
mark_as_advanced(ARROWS_COLMAP_DEFAULT_LIBRARY_DIR)

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

set(ARROWS_COLMAP_CMAKE_DIR "${ARROWS_COLMAP_SOURCE_DIR}/CMake")
list(INSERT CMAKE_MODULE_PATH 0 ${KWIVER_CMAKE_DIR})

OPTION(ARROWS_COLMAP_ENABLE_TESTS                "Enable testing" OFF )
OPTION(ARROWS_COLMAP_BUILD_SHARED     "Build ARROWS-COLMAP components shared or not" TRUE )
MARK_AS_ADVANCED( ARROWS_COLMAP_BUILD_SHARED )
set(BUILD_SHARED_LIBS ${ARROWS_COLMAP_BUILT_SHARED})

# Install rules for CMake utilities
#include( kwiver-install-utils )
include( kwiver-utils )
include( kwiver-depends )

# project includes
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories(${KWIVER_INCLUDE_DIRS})

include_directories( SYSTEM ${CMAKE_CURRENT_SOURCE_DIR} )

# TODO Make everything configurable with variables so developers dont have to reconfigure everytime
set(plugin_colmap_headers
    image_container.h
)

kwiver_install_headers(
    SUBDIR colmap
    ${plugin_colmap_headers}
)

kwiver_install_headers(
    ${CMAKE_CURRENT_BINARY_DIR}/kwiver_algo_colmap_export.h
    NOPATH SUBDIR colmap
)

set(
    plugin_colmap_sources
    image_container.cxx
)

kwiver_add_library(
    kwiver_algo_colmap
    ${plugin_colmap_headers}
    ${plugin_colmap_sources}
)

target_include_directories(
    kwiver_algo_colmap PUBLIC
    ${COLMAP_INCLUDE_DIRS}
)

target_link_directories(
    kwiver_algo_colmap PUBLIC
    ${COLMAP_LINK_DIRS}
)

target_link_libraries(
    kwiver_algo_colmap
    PUBLIC vital
    ${COLMAP_LIBRARIES}
)

# Add tests if enabled
if (ARROWS_COLMAP_ENABLE_TESTS)
  enable_testing()
  include(CTest)
  set(BUILD_TESTING    ON)  # needed for compatibility with kwiversys
  mark_as_advanced(BUILD_TESTING)
  include_directories(${KWIVER_TEST_INCLUDE_DIR})
  add_subdirectory( tests )
endif()

# Prepare space-separated list of library names for config
get_property(arrows_colmap_libs GLOBAL PROPERTY kwiver_libraries)
string(REPLACE ";" " " arrows_colmap_libs "${arrows_colmap_libs}")

set(ARROWS_COLMAP_CONFIG_FILE          "${ARROWS_COLMAP_BINARY_DIR}/arrows-colmap-config.cmake")
configure_file(
  "${ARROWS_COLMAP_CMAKE_DIR}/arrows-colmap-config-build.cmake.in"
  "${ARROWS_CONFIG_FILE}"
  @ONLY
  )

#TODO: Define package dependencies
set(ARROWS_COLMAP_DEPS "")

if(EXISTS /etc/redhat-release)
  file(READ /etc/redhat-release RHEL_VERSION)
  string(REGEX REPLACE ".*release ([^\\. ]*).*" "\\1" RHEL_VERSION "${RHEL_VERSION}")
  set(CPACK_SYSTEM_NAME "el${RHEL_VERSION}.${CMAKE_SYSTEM_PROCESSOR}")
  set(CPACK_RPM_PACKAGE_AUTOREQPROV " no")
  set(CPACK_RPM_PACKAGE_REQUIRES "${ARROWS_COLMAP_DEPS}")
else()
  set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(CPACK_PACKAGE_NAME              "ARROWS")
set(CPACK_PACKAGE_VENDOR            "Kitware, Inc.")
set(CPACK_PACKAGE_CONTACT           "kitware@kitware.com")
set(CPACK_MONOLITHIC_INSTALL        true)
set(CPACK_PACKAGE_VERSION_MAJOR     "${ARROWS_COLMAP_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR     "${ARROWS_COLMAP_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH     "${ARROWS_COLMAP_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION           "${ARROWS_COLMAP_VERSION}")
#set(CPACK_RESOURCE_FILE_LICENSE     "${KWIVER_DIR}/LICENSE")
set(CPACK_PACKAGING_INSTALL_PREFIX  "${CMAKE_INSTALL_PREFIX}")
set(CPACK_PACKAGE_FILE_NAME         "${CPACK_PACKAGE_NAME}-"
                                    "${CPACK_PACKAGE_VERSION}-"
                                    "${CPACK_SYSTEM_NAME}")

include (CPack)