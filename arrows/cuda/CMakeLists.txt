cmake_minimum_required(VERSION 3.15)
project(ARROWS_CUDA LANGUAGES CXX CUDA)

###
# ARROWS-CUDA version
set(ARROWS_CUDA_VERSION_MAJOR 1)
set(ARROWS_CUDA_VERSION_MINOR 7)
set(ARROWS_CUDA_VERSION_PATCH 0)
set(ARROWS_CUDA_VERSION "${ARROWS_CUDA_VERSION_MAJOR}.${ARROWS_CUDA_VERSION_MINOR}.${ARROWS_CUDA_VERSION_PATCH}")

# Organize target into folders for IDEs that support it
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_FOLDER "CUDA")
set(ARROWS_CUDA_SOURCE_DIR           "${CMAKE_SOURCE_DIR}")
set(ARROWS_CUDA_BINARY_DIR           "${CMAKE_BINARY_DIR}")

# Default library dir
set(ARROWS_CUDA_DEFAULT_LIBRARY_DIR "lib" CACHE STRING
    "Default library directory for arrows")
mark_as_advanced(ARROWS_CUDA_DEFAULT_LIBRARY_DIR)

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

set(ARROWS_CUDA_CMAKE_DIR "${ARROWS_CUDA_SOURCE_DIR}/CMake")
list(INSERT CMAKE_MODULE_PATH 0 ${KWIVER_CMAKE_DIR})

OPTION(ARROWS_CUDA_BUILD_SHARED     "Build ARROWS-CUDA components shared or not" TRUE )
MARK_AS_ADVANCED( ARROWS_CUDA_BUILD_SHARED )
set(BUILD_SHARED_LIBS ${ARROWS_CUDA_BUILT_SHARED})

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

set(header_files
  integrate_depth_maps.h
  )

set(private_header_files
  cuda_error_check.h
  cuda_memory.h
  )

set(source_files
  cuda_error_check.cxx
  integrate_depth_maps.cxx
  integrate_depth_maps.cu
  )

kwiver_install_headers(
  SUBDIR     cuda
  ${header_files}
  )

kwiver_install_headers(
  ${CMAKE_CURRENT_BINARY_DIR}/kwiver_algo_cuda_export.h
  NOPATH   SUBDIR     cuda
  )

kwiver_add_library( kwiver_algo_cuda
  ${header_files}
  ${private_header_files}
  ${source_files}
  )

set_target_properties(kwiver_algo_cuda
  PROPERTIES   POSITION_INDEPENDENT_CODE ON
               CUDA_SEPARABLE_COMPILATION ON)

target_link_libraries( kwiver_algo_cuda
  PUBLIC               vital_algo
  PRIVATE              kwiver_algo_core
)

target_include_directories( kwiver_algo_cuda SYSTEM
  PRIVATE                   ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES}
)

algorithms_create_plugin( kwiver_algo_cuda
  register_algorithms.cxx
)

# Prepare space-separated list of library names for config
get_property(arrows_cuda_libs GLOBAL PROPERTY kwiver_libraries)
string(REPLACE ";" " " arrows_cuda_libs "${arrows_cuda_libs}")

set(ARROWS_CUDA_CONFIG_FILE          "${ARROWS_CUDA_BINARY_DIR}/arrows-cuda-config.cmake")
configure_file(
  "${ARROWS_CUDA_CMAKE_DIR}/arrows-cuda-config-build.cmake.in"
  "${ARROWS_CONFIG_FILE}"
  @ONLY
  )

#TODO: Define package dependencies
set(ARROWS_CUDA_DEPS "")

if(EXISTS /etc/redhat-release)
  file(READ /etc/redhat-release RHEL_VERSION)
  string(REGEX REPLACE ".*release ([^\\. ]*).*" "\\1" RHEL_VERSION "${RHEL_VERSION}")
  set(CPACK_SYSTEM_NAME "el${RHEL_VERSION}.${CMAKE_SYSTEM_PROCESSOR}")
  set(CPACK_RPM_PACKAGE_AUTOREQPROV " no")
  set(CPACK_RPM_PACKAGE_REQUIRES "${ARROWS_CUDA_DEPS}")
else()
  set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(CPACK_PACKAGE_NAME              "ARROWS")
set(CPACK_PACKAGE_VENDOR            "Kitware, Inc.")
set(CPACK_PACKAGE_CONTACT           "kitware@kitware.com")
set(CPACK_MONOLITHIC_INSTALL        true)
set(CPACK_PACKAGE_VERSION_MAJOR     "${ARROWS_CUDA_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR     "${ARROWS_CUDA_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH     "${ARROWS_CUDA_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION           "${ARROWS_CUDA_VERSION}")
#set(CPACK_RESOURCE_FILE_LICENSE     "${KWIVER_DIR}/LICENSE")
set(CPACK_PACKAGING_INSTALL_PREFIX  "${CMAKE_INSTALL_PREFIX}")
set(CPACK_PACKAGE_FILE_NAME         "${CPACK_PACKAGE_NAME}-"
                                    "${CPACK_PACKAGE_VERSION}-"
                                    "${CPACK_SYSTEM_NAME}")

include (CPack)