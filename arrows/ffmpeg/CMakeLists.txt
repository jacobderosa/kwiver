# Build / Install plugin containing ffmpeg algorithm implementations + support
# structures

cmake_minimum_required(VERSION 3.15)

project(ARROWS_FFMPEG)

###
# ARROWS_FFMPEG version
set(ARROWS_FFMPEG_VERSION_MAJOR 1)
set(ARROWS_FFMPEG_VERSION_MINOR 7)
set(ARROWS_FFMPEG_VERSION_PATCH 0)
set(ARROWS_FFMPEG_VERSION "${ARROWS_FFMPEG_VERSION_MAJOR}.${ARROWS_FFMPEG_VERSION_MINOR}.${ARROWS_FFMPEG_VERSION_PATCH}")

# Organize target into folders for IDEs that support it
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_FOLDER "FFmpeg")

# Default library dir
set(ARROWS_FFMPEG_DEFAULT_LIBRARY_DIR "lib" CACHE STRING
    "Default library directory for arrows")
mark_as_advanced(ARROWS_FFMPEG_DEFAULT_LIBRARY_DIR)

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
# Look for and use ARROWS KLV
#
set( arrows_KLV_DIR "" CACHE PATH "Path to build directory of ARROWS KLV" )
if ( IS_DIRECTORY ${arrows_KLV_DIR} )
  find_package( arrows_klv NO_MODULE )
  if ( arrows_KLV_FOUND )
    # Search for libraries in the arrows klv install root
    list(APPEND CMAKE_PREFIX_PATH "${ARROWS_KLV_DIR}")
  endif()
endif()

set(ARROWS_FFMPEG_CMAKE_DIR "${ARROWS_FFMPEG_SOURCE_DIR}/CMake")
list(INSERT CMAKE_MODULE_PATH 0 ${KWIVER_CMAKE_DIR})

OPTION(ARROWS_FFMPEG_ENABLE_TESTS                "Enable testing" OFF )
OPTION(ARROWS_FFMPEG_BUILD_SHARED     "Build ARROWS-FFMPEG components shared or not" TRUE )
MARK_AS_ADVANCED( ARROWS_FFMPEG_BUILD_SHARED )
set(BUILD_SHARED_LIBS ${ARROWS_FFMPEG_BUILT_SHARED})

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

#include_directories(${FFMPEG_INCLUDE_DIR})

if (NOT FFMPEG_FOUND_SEVERAL)
  message( FATAL_ERROR "FFmpeg headers all in the same directories, "
"this is not supported by this Arrow."
"\nThis likely indicates that you are building against an old FFmpeg ! "
)
endif()

set(ffmpeg_headers_public
  ffmpeg_init.h
  ffmpeg_video_input.h
  ffmpeg_video_output.h
  ffmpeg_video_raw_image.cxx
  ffmpeg_video_raw_metadata.cxx
  ffmpeg_video_settings.h
  )

kwiver_install_headers(
  SUBDIR     arrows/ffmpeg
  ${ffmpeg_headers_public}
  )

kwiver_install_headers(
  ${CMAKE_CURRENT_BINARY_DIR}/kwiver_algo_ffmpeg_export.h
  NOPATH   SUBDIR     arrows/ffmpeg
  )

set(ffmpeg_sources
  ffmpeg_cuda.cxx
  ffmpeg_init.cxx
  ffmpeg_util.cxx
  ffmpeg_video_input.cxx
  ffmpeg_video_output.cxx
  ffmpeg_video_raw_image.cxx
  ffmpeg_video_raw_metadata.cxx
  ffmpeg_video_settings.cxx
  )

kwiver_add_library( kwiver_algo_ffmpeg
  ${ffmpeg_headers_public}
  ${ffmpeg_sources}
)

if(ARROWS_FFMPEG_ENABLE_CUDA)
  set(ARROWS_FFMPEG_ENABLE_FFMPEG_CUDA ON CACHE BOOL "Enable use of CUVID/NVENC codecs")
  target_include_directories( kwiver_algo_ffmpeg SYSTEM
    PRIVATE                   ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES}
  )
  if(ARROWS_FFMPEG_ENABLE_FFMPEG_CUDA)
    add_compile_definitions(ARROWS_FFMPEG_ENABLE_FFMPEG_CUDA)
    set(FFMPEG_CUDA_LIBRARIES cuda)
  endif()
endif()

if(fletch_ENABLED_ZLib)
  if(BUILD_SHARED_LIBS)
    find_package(ZLIB MODULE REQUIRED)
  else()
    # Zlib produces both static and dynamic libraries
    # This allows static libraries to be picked instead of shared libraries
    # Thereby not creating a zlib dependency in he library
    find_library(ZLIB_LIBRARIES NAMES libz.a libz libzlib
                 PATHS ${fletch_ROOT}
                 PATH_SUFFIXES lib
                 )
  endif()
endif()

target_link_libraries( kwiver_algo_ffmpeg
  PUBLIC               kwiver::vital_algo
  PRIVATE              kwiver::kwiver_algo_klv kwiver::kwiversys
                       ${FFMPEG_CUDA_LIBRARIES}
                       ${FFMPEG_LIBRARIES}
                       ${ZLIB_LIBRARIES}
                       ${CUDA_LIBRARY}
  )

target_include_directories( kwiver_algo_ffmpeg SYSTEM
    PRIVATE                   ${FFMPEG_INCLUDE_DIR}
    PUBLIC                    ${KWIVER_INCLUDE_DIRS}
  )

algorithms_create_plugin( kwiver_algo_ffmpeg
  register_algorithms.cxx
  )

# Add tests if enabled
if (ARROWS_FFMPEG_ENABLE_TESTS)
  enable_testing()
  include(CTest)
  set(BUILD_TESTING    ON)  # needed for compatibility with kwiversys
  mark_as_advanced(BUILD_TESTING)
  include_directories(${KWIVER_TEST_INCLUDE_DIR})
  add_subdirectory( tests )
endif()

# Prepare space-separated list of library names for config
get_property(arrows_ffmpeg_libs GLOBAL PROPERTY kwiver_libraries)
string(REPLACE ";" " " arrows_ffmpeg_libs "${arrows_ffmpeg_libs}")

set(ARROWS_FFMPEG_CONFIG_FILE          "${ARROWS_FFMPEG_BINARY_DIR}/arrows-ffmpeg-config.cmake")
configure_file(
  "${ARROWS_FFMPEG_CMAKE_DIR}/arrows-ffmpeg-config-build.cmake.in"
  "${ARROWS_CONFIG_FILE}"
  @ONLY
  )

kwiver_export_targets("${ARROWS_FFMPEG_BINARY_DIR}/arrows-ffmpeg-config-targets.cmake")

#TODO: Define package dependencies
set(ARROWS_FFMPEG_DEPS "")

if(EXISTS /etc/redhat-release)
  file(READ /etc/redhat-release RHEL_VERSION)
  string(REGEX REPLACE ".*release ([^\\. ]*).*" "\\1" RHEL_VERSION "${RHEL_VERSION}")
  set(CPACK_SYSTEM_NAME "el${RHEL_VERSION}.${CMAKE_SYSTEM_PROCESSOR}")
  set(CPACK_RPM_PACKAGE_AUTOREQPROV " no")
  set(CPACK_RPM_PACKAGE_REQUIRES "${ARROWS_FFMPEG_DEPS}")
else()
  set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(CPACK_PACKAGE_NAME              "ARROWS")
set(CPACK_PACKAGE_VENDOR            "Kitware, Inc.")
set(CPACK_PACKAGE_CONTACT           "kitware@kitware.com")
set(CPACK_MONOLITHIC_INSTALL        true)
set(CPACK_PACKAGE_VERSION_MAJOR     "${ARROWS_FFMPEG_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR     "${ARROWS_FFMPEG_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH     "${ARROWS_FFMPEG_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION           "${ARROWS_FFMPEG_VERSION}")
#set(CPACK_RESOURCE_FILE_LICENSE     "${KWIVER_DIR}/LICENSE")
set(CPACK_PACKAGING_INSTALL_PREFIX  "${CMAKE_INSTALL_PREFIX}")
set(CPACK_PACKAGE_FILE_NAME         "${CPACK_PACKAGE_NAME}-"
                                    "${CPACK_PACKAGE_VERSION}-"
                                    "${CPACK_SYSTEM_NAME}")

include (CPack)