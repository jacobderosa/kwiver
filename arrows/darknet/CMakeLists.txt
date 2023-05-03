# Build/install plugin for darknet CNN support
#

cmake_minimum_required(VERSION 3.15)

project(ARROWS_DARKNET)

###
# ARROWS_DARKNET version
set(ARROWS_DARKNET_VERSION_MAJOR 1)
set(ARROWS_DARKNET_VERSION_MINOR 7)
set(ARROWS_DARKNET_VERSION_PATCH 0)
set(ARROWS_DARKNET_VERSION "${ARROWS_DARKNET_VERSION_MAJOR}.${ARROWS_DARKNET_VERSION_MINOR}.${ARROWS_DARKNET_VERSION_PATCH}")

# Organize target into folders for IDEs that support it
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_FOLDER "Darknet")

# Default library dir
set(ARROWS_DARKNET_DEFAULT_LIBRARY_DIR "lib" CACHE STRING
    "Default library directory for arrows")
mark_as_advanced(ARROWS_DARKNET_DEFAULT_LIBRARY_DIR)

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
# Look for and use ARROWS OCV
#
set( arrows_OCV_DIR "" CACHE PATH "Path to build directory of ARROWS OCV" )
if ( IS_DIRECTORY ${arrows_OCV_DIR} )
  find_package( arrows_ocv NO_MODULE )
  if ( arrows_OCV_FOUND )
    # Search for libraries in the arrows ocv install root
    list(APPEND CMAKE_PREFIX_PATH "${KWIVER_DIR}")
    list(APPEND CMAKE_PREFIX_PATH "${fletch_DIR}")
  endif()
endif()

set(ARROWS_DARKNET_CMAKE_DIR "${ARROWS_DARKNET_SOURCE_DIR}/CMake")
list(INSERT CMAKE_MODULE_PATH 0 ${KWIVER_CMAKE_DIR})

OPTION(ARROWS_DARKNET_BUILD_SHARED     "Build ARROWS-DARKNET components shared or not" TRUE )
MARK_AS_ADVANCED( ARROWS_DARKNET_BUILD_SHARED )
set(BUILD_SHARED_LIBS ${ARROWS_DARKNET_BUILT_SHARED})

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

set( headers
  darknet_custom_resize.h
  darknet_detector.h
  darknet_trainer.h
)

set( sources
  darknet_custom_resize.cxx
  darknet_detector.cxx
  darknet_trainer.cxx
  )

set( Darknet_linked_libs
  ${Darknet_LIBRARIES}
  kwiversys
  ${OpenCV_LIBS}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  kwiver_algo_ocv )

# if darknet was built with the GPU support
if (Darknet_USING_GPU)
  find_package(CUDA QUIET REQUIRED)
  include_directories( ${CUDA_TOOLKIT_ROOT_DIR}/include )
  add_definitions( -DDARKNET_USE_GPU )  # needed by darknet detector
endif()

if (Darknet_USING_CUDNN)
  include_directories( ${CUDNN_TOOLKIT_ROOT_DIR}/include )
  add_definitions( -DDARKNET_USE_CUDNN )  # needed by darknet detector
endif()


# Visual Studio links to a static lib of darknet due to the pthread dependency
# So we need to add all libraries to this arrow, if darknet was a dll, we would
# not have this issue, but we need pthreads and it likes static for some reason
if (WIN32)
  add_definitions( -D_TIMESPEC_DEFINED )
  set( Darknet_linked_libs ${Darknet_linked_libs} pthreadVC2 )

  if (Darknet_USING_GPU)
    set( Darknet_linked_libs
         ${Darknet_linked_libs}
         ${CUDA_CUDA_LIBRARY}
         ${CUDA_LIBRARIES}
         ${CUDA_CUBLAS_LIBRARIES}
         ${CUDA_curand_LIBRARY} )
  endif()
  if (Darknet_USING_CUDNN)
    message(STATUS "Darkent Arrow using CUDNN, looking for it here : ${CUDNN_TOOLKIT_ROOT_DIR}")
    find_library( CUDNN_LIBRARIES
    NAMES cudnn libcudnn
    HINTS ${CUDNN_TOOLKIT_ROOT_DIR}
    PATH_SUFFIXES lib lib64 lib/x64 lib/x86 targets/aarch64-linux
    )
  if(NOT CUDNN_LIBRARIES)
    message(FATAL_ERROR "Unable to find cudnn libraries")
  endif()
  include_directories( ${CUDNN_TOOLKIT_ROOT_DIR}/include )
    set( Darknet_linked_libs
         ${Darknet_linked_libs}
         ${CUDNN_LIBRARIES} )
  endif()
endif()

kwiver_install_headers(
  SUBDIR     arrows/darknet
  ${headers}
  )

kwiver_install_headers(
  ${CMAKE_CURRENT_BINARY_DIR}/kwiver_algo_darknet_export.h
  NOPATH   SUBDIR     arrows/darknet
  )


kwiver_add_library( kwiver_algo_darknet
  ${headers}
  ${sources}
  )

target_link_libraries( kwiver_algo_darknet
  PUBLIC               kwiver::vital_algo
  PRIVATE              ${Darknet_linked_libs}
  )

target_include_directories( kwiver_algo_darknet SYSTEM
  PUBLIC                    ${KWIVER_INCLUDE_DIRS}
  )

algorithms_create_plugin( kwiver_algo_darknet
  register_algorithms.cxx
  )

if( KWIVER_ENABLE_PYTHON )
  kwiver_create_python_init( kwiver/arrows/darknet )

  kwiver_add_python_module( ${CMAKE_CURRENT_SOURCE_DIR}/generate_headers.py
    kwiver/arrows/darknet
    generate_headers )
endif()

# Prepare space-separated list of library names for config
get_property(arrows_darknet_libs GLOBAL PROPERTY kwiver_libraries)
string(REPLACE ";" " " arrows_darknet_libs "${arrows_darknet_libs}")

set(ARROWS_DARKNET_CONFIG_FILE          "${ARROWS_DARKNET_BINARY_DIR}/arrows-darknet-config.cmake")
configure_file(
  "${ARROWS_DARKNET_CMAKE_DIR}/arrows-darknet-config-build.cmake.in"
  "${ARROWS_CONFIG_FILE}"
  @ONLY
  )

kwiver_export_targets("${ARROWS_DARKNET_BINARY_DIR}/arrows-darknet-config-targets.cmake")

#TODO: Define package dependencies
set(ARROWS_DARKNET_DEPS "")

if(EXISTS /etc/redhat-release)
  file(READ /etc/redhat-release RHEL_VERSION)
  string(REGEX REPLACE ".*release ([^\\. ]*).*" "\\1" RHEL_VERSION "${RHEL_VERSION}")
  set(CPACK_SYSTEM_NAME "el${RHEL_VERSION}.${CMAKE_SYSTEM_PROCESSOR}")
  set(CPACK_RPM_PACKAGE_AUTOREQPROV " no")
  set(CPACK_RPM_PACKAGE_REQUIRES "${ARROWS_DARKNET_DEPS}")
else()
  set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(CPACK_PACKAGE_NAME              "ARROWS")
set(CPACK_PACKAGE_VENDOR            "Kitware, Inc.")
set(CPACK_PACKAGE_CONTACT           "kitware@kitware.com")
set(CPACK_MONOLITHIC_INSTALL        true)
set(CPACK_PACKAGE_VERSION_MAJOR     "${ARROWS_DARKNET_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR     "${ARROWS_DARKNET_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH     "${ARROWS_DARKNET_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION           "${ARROWS_DARKNET_VERSION}")
#set(CPACK_RESOURCE_FILE_LICENSE     "${KWIVER_DIR}/LICENSE")
set(CPACK_PACKAGING_INSTALL_PREFIX  "${CMAKE_INSTALL_PREFIX}")
set(CPACK_PACKAGE_FILE_NAME         "${CPACK_PACKAGE_NAME}-"
                                    "${CPACK_PACKAGE_VERSION}-"
                                    "${CPACK_SYSTEM_NAME}")

include (CPack)