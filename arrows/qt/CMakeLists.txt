# Build / Install plugin containing Qt support structures

cmake_minimum_required(VERSION 3.15)
project(ARROWS_QT)

###
# ARROWS-QT version
set(ARROWS_QT_VERSION_MAJOR 1)
set(ARROWS_QT_VERSION_MINOR 7)
set(ARROWS_QT_VERSION_PATCH 0)
set(ARROWS_QT_VERSION "${ARROWS_QT_VERSION_MAJOR}.${ARROWS_QT_VERSION_MINOR}.${ARROWS_QT_VERSION_PATCH}")

# Organize target into folders for IDEs that support it
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_FOLDER "Qt")
set(ARROWS_QT_SOURCE_DIR           "${CMAKE_SOURCE_DIR}")
set(ARROWS_QT_BINARY_DIR           "${CMAKE_BINARY_DIR}")

# Default library dir
set(ARROWS_QT_DEFAULT_LIBRARY_DIR "lib" CACHE STRING
    "Default library directory for arrows")
mark_as_advanced(ARROWS_QT_DEFAULT_LIBRARY_DIR)

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

set(ARROWS_QT_CMAKE_DIR "${ARROWS_QT_SOURCE_DIR}/CMake")
list(INSERT CMAKE_MODULE_PATH 0 ${KWIVER_CMAKE_DIR})

OPTION(ARROWS_QT_ENABLE_TESTS                "Enable testing" OFF )
OPTION(ARROWS_QT_BUILD_SHARED     "Build ARROWS-QT components shared or not" TRUE )
MARK_AS_ADVANCED( ARROWS_QT_BUILD_SHARED )
set(BUILD_SHARED_LIBS ${ARROWS_QT_BUILT_SHARED})

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

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTOUIC ON )
set( CMAKE_INCLUDE_CURRENT_DIR ON )

set( qt_sources
  image_container.cxx
  image_io.cxx
  )

set( qt_headers_public
  image_container.h
  image_io.h
  kq_global.h
  )

set( qt_libraries_public
  kwiver_algo_core
  Qt5::Gui
  Qt5::Core
  )

if( KWIVER_ENABLE_PROCESSES )
  list(APPEND qt_sources
    EmbeddedPipelineWorker.cxx
  )
  list(APPEND qt_headers_public
    EmbeddedPipelineWorker.h
  )
  list(APPEND qt_libraries_public
    kwiver_adapter
    sprokit_pipeline
  )
endif()

kwiver_add_library( kwiver_algo_qt
  ${qt_headers_public}
  ${qt_sources}
  )
target_link_libraries( kwiver_algo_qt
  PUBLIC ${qt_libraries_public}
  )

algorithms_create_plugin( kwiver_algo_qt
  register_algorithms.cxx
  )

kwiver_install_headers(
  SUBDIR     qt
  ${qt_headers_public}
  )

kwiver_install_headers(
  ${CMAKE_CURRENT_BINARY_DIR}/kwiver_algo_qt_export.h
  NOPATH   SUBDIR     qt
  )

if( KWIVER_ENABLE_QT_EXT )
  add_subdirectory( widgets )
endif()

if (ARROWS_QT_ENABLE_TESTS)
  enable_testing()
  include(CTest)
  set(BUILD_TESTING    ON)  # needed for compatibility with kwiversys
  mark_as_advanced(BUILD_TESTING)
  include_directories(${KWIVER_TEST_INCLUDE_DIR})
  add_subdirectory( tests )
endif()

if (KWIVER_ENABLE_TOOLS)
  add_subdirectory(applets)
endif()

# Prepare space-separated list of library names for config
get_property(arrows_qt_libs GLOBAL PROPERTY kwiver_libraries)
string(REPLACE ";" " " arrows_qt_libs "${arrows_qt_libs}")

set(ARROWS_QT_CONFIG_FILE          "${ARROWS_QT_BINARY_DIR}/arrows-qt-config.cmake")
configure_file(
  "${ARROWS_QT_CMAKE_DIR}/arrows-qt-config-build.cmake.in"
  "${ARROWS_CONFIG_FILE}"
  @ONLY
  )

#TODO: Define package dependencies
set(ARROWS_QT_DEPS "")

if(EXISTS /etc/redhat-release)
  file(READ /etc/redhat-release RHEL_VERSION)
  string(REGEX REPLACE ".*release ([^\\. ]*).*" "\\1" RHEL_VERSION "${RHEL_VERSION}")
  set(CPACK_SYSTEM_NAME "el${RHEL_VERSION}.${CMAKE_SYSTEM_PROCESSOR}")
  set(CPACK_RPM_PACKAGE_AUTOREQPROV " no")
  set(CPACK_RPM_PACKAGE_REQUIRES "${ARROWS_QT_DEPS}")
else()
  set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(CPACK_PACKAGE_NAME              "ARROWS")
set(CPACK_PACKAGE_VENDOR            "Kitware, Inc.")
set(CPACK_PACKAGE_CONTACT           "kitware@kitware.com")
set(CPACK_MONOLITHIC_INSTALL        true)
set(CPACK_PACKAGE_VERSION_MAJOR     "${ARROWS_QT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR     "${ARROWS_QT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH     "${ARROWS_QT_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION           "${ARROWS_QT_VERSION}")
#set(CPACK_RESOURCE_FILE_LICENSE     "${KWIVER_DIR}/LICENSE")
set(CPACK_PACKAGING_INSTALL_PREFIX  "${CMAKE_INSTALL_PREFIX}")
set(CPACK_PACKAGE_FILE_NAME         "${CPACK_PACKAGE_NAME}-"
                                    "${CPACK_PACKAGE_VERSION}-"
                                    "${CPACK_SYSTEM_NAME}")

include (CPack)