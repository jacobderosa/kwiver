###
# CMake file for matlab bindings

cmake_minimum_required(VERSION 3.15)
project(ARROWS_MATLAB)

###
# ARROWS-MATLAB version
set(ARROWS_MATLAB_VERSION_MAJOR 1)
set(ARROWS_MATLAB_VERSION_MINOR 7)
set(ARROWS_MATLAB_VERSION_PATCH 0)
set(ARROWS_MATLAB_VERSION "${ARROWS_MATLAB_VERSION_MAJOR}.${ARROWS_MATLAB_VERSION_MINOR}.${ARROWS_MATLAB_VERSION_PATCH}")

# Organize target into folders for IDEs that support it
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_FOLDER "Matlab")

# Default library dir
set(ARROWS_MATLAB_DEFAULT_LIBRARY_DIR "lib" CACHE STRING
    "Default library directory for arrows")
mark_as_advanced(ARROWS_MATLAB_DEFAULT_LIBRARY_DIR)

###
# Look for and use KWIVER
#
set( kwiver_DIR "" CACHE PATH "Path to build directory of KWIVER" )
if ( IS_DIRECTORY ${kwiver_DIR} )
  find_package( kwiver NO_MODULE )
  if ( kwiver_FOUND )
    # Search for libraries in the kwiver install root
    list(APPEND CMAKE_PREFIX_PATH "${KWIVER_DIR}")
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

set(ARROWS_MATLAB_CMAKE_DIR "${ARROWS_MATLAB_SOURCE_DIR}/CMake")
list(INSERT CMAKE_MODULE_PATH 0 ${KWIVER_CMAKE_DIR})

OPTION(ARROWS_MATLAB_ENABLE_TESTS                "Enable testing" OFF )
OPTION(ARROWS_MATLAB_BUILD_SHARED     "Build ARROWS-MATLAB components shared or not" TRUE )
MARK_AS_ADVANCED( ARROWS_MATLAB_BUILD_SHARED )
set(BUILD_SHARED_LIBS ${ARROWS_MATLAB_BUILT_SHARED})

# Install rules for CMake utilities
#include( kwiver-install-utils )
include( kwiver-utils )
include( kwiver-depends )

set( sources
  matlab_util.cxx
  matlab_exception.cxx
  matlab_engine.cxx
  matlab_image_object_detector.cxx
  matlab_image_filter.cxx
  matlab_detection_output.cxx
  )

set( public_headers
  mxtypes.h
  mxarray.h
  matlab_util.h
  matlab_exception.h
  matlab_engine.h
  matlab_image_object_detector.h
  matlab_image_filter.h
  matlab_detection_output.h
)

set( private_headers
  )

include_directories( "${Matlab_INCLUDE_DIRS}" )

kwiver_install_headers(
  ${public_headers}
  SUBDIR     arrows/matlab
  )

kwiver_install_headers(
  ${CMAKE_CURRENT_BINARY_DIR}/kwiver_algo_matlab_export.h
  SUBDIR     arrows/matlab
  NOPATH
  )

kwiver_add_library( kwiver_algo_matlab
  ${public_headers}
  ${private_headers}
  ${sources}
  )

target_link_libraries( kwiver_algo_matlab
  PUBLIC               kwiver::vital_algo
                       ${Matlab_LIBRARIES}
                       kwiver::kwiver_algo_ocv
                       kwiver::kwiversys
  )

target_include_directories( kwiver_algo_matlab SYSTEM
  PUBLIC                    ${KWIVER_INCLUDE_DIRS}
  )

algorithms_create_plugin( kwiver_algo_matlab
  register_algorithms.cxx
  )

# Add tests if enabled
if (ARROWS_MATLAB_ENABLE_TESTS)
  enable_testing()
  include(CTest)
  set(BUILD_TESTING    ON)  # needed for compatibility with kwiversys
  mark_as_advanced(BUILD_TESTING)
  include_directories(${KWIVER_TEST_INCLUDE_DIR})
  add_subdirectory( tests )
endif()

# Prepare space-separated list of library names for config
get_property(arrows_matlab_libs GLOBAL PROPERTY kwiver_libraries)
string(REPLACE ";" " " arrows_matlab_libs "${arrows_matlab_libs}")

set(ARROWS_MATLAB_CONFIG_FILE          "${ARROWS_MATLAB_BINARY_DIR}/arrows-matlab-config.cmake")
configure_file(
  "${ARROWS_MATLAB_CMAKE_DIR}/arrows-matlab-config-build.cmake.in"
  "${ARROWS_CONFIG_FILE}"
  @ONLY
  )

kwiver_export_targets("${ARROWS_MATLAB_BINARY_DIR}/arrows-matlab-config-targets.cmake")

#TODO: Define package dependencies
set(ARROWS_MATLAB_DEPS "")

if(EXISTS /etc/redhat-release)
  file(READ /etc/redhat-release RHEL_VERSION)
  string(REGEX REPLACE ".*release ([^\\. ]*).*" "\\1" RHEL_VERSION "${RHEL_VERSION}")
  set(CPACK_SYSTEM_NAME "el${RHEL_VERSION}.${CMAKE_SYSTEM_PROCESSOR}")
  set(CPACK_RPM_PACKAGE_AUTOREQPROV " no")
  set(CPACK_RPM_PACKAGE_REQUIRES "${ARROWS_MATLAB_DEPS}")
else()
  set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(CPACK_PACKAGE_NAME              "ARROWS")
set(CPACK_PACKAGE_VENDOR            "Kitware, Inc.")
set(CPACK_PACKAGE_CONTACT           "kitware@kitware.com")
set(CPACK_MONOLITHIC_INSTALL        true)
set(CPACK_PACKAGE_VERSION_MAJOR     "${ARROWS_MATLAB_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR     "${ARROWS_MATLAB_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH     "${ARROWS_MATLAB_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION           "${ARROWS_MATLAB_VERSION}")
#set(CPACK_RESOURCE_FILE_LICENSE     "${KWIVER_DIR}/LICENSE")
set(CPACK_PACKAGING_INSTALL_PREFIX  "${CMAKE_INSTALL_PREFIX}")
set(CPACK_PACKAGE_FILE_NAME         "${CPACK_PACKAGE_NAME}-"
                                    "${CPACK_PACKAGE_VERSION}-"
                                    "${CPACK_SYSTEM_NAME}")

include (CPack)