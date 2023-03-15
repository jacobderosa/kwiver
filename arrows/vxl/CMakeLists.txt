# Build / Install plugin containing VXL algorithm implementations + support
# structures

cmake_minimum_required(VERSION 3.15)

project(ARROWS_VXL)

###
# ARROWS_VXL version
set(ARROWS_VXL_VERSION_MAJOR 1)
set(ARROWS_VXL_VERSION_MINOR 7)
set(ARROWS_VXL_VERSION_PATCH 0)
set(ARROWS_VXL_VERSION "${ARROWS_VXL_VERSION_MAJOR}.${ARROWS_VXL_VERSION_MINOR}.${ARROWS_VXL_VERSION_PATCH}")

# Organize target into folders for IDEs that support it
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_FOLDER "VXL")
set(ARROWS_VXL_SOURCE_DIR           "${CMAKE_SOURCE_DIR}")
set(ARROWS_VXL_BINARY_DIR           "${CMAKE_BINARY_DIR}")

# Default library dir
set(ARROWS_VXL_DEFAULT_LIBRARY_DIR "lib" CACHE STRING
    "Default library directory for arrows")
mark_as_advanced(ARROWS_VXL_DEFAULT_LIBRARY_DIR)

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

###
# Look for and use ARROWS MVG
#
set( arrows_MVG_DIR "" CACHE PATH "Path to build directory of ARROWS MVG" )
if ( IS_DIRECTORY ${arrows_MVG_DIR} )
  find_package( arrows_mvg NO_MODULE )
  if ( arrows_MVG_FOUND )
    # Search for libraries in the arrows mvg install root
    list(APPEND CMAKE_PREFIX_PATH "${ARROWS_MVG_DIR}")
  endif()
endif()

set(ARROWS_VXL_CMAKE_DIR "${ARROWS_VXL_SOURCE_DIR}/CMake")
list(INSERT CMAKE_MODULE_PATH 0 ${KWIVER_CMAKE_DIR})

OPTION(ARROWS_VXL_ENABLE_TESTS                "Enable testing" OFF )
OPTION(ARROWS_VXL_BUILD_SHARED     "Build ARROWS-VXL components shared or not" TRUE )
MARK_AS_ADVANCED( ARROWS_VXL_BUILD_SHARED )
set(BUILD_SHARED_LIBS ${ARROWS_VXL_BUILT_SHARED})

set( kwiver_plugin_algorithm_subdir               ${KWIVER_PLUGIN_ALGORITHM_DIR} )

# Install rules for CMake utilities
#include( kwiver-install-utils )
include( kwiver-utils )
include( kwiver-depends )

# project includes
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories(${KWIVER_INCLUDE_DIRS})
include_directories(${ARROWS_KLV_INCLUDE_DIRS})
include_directories(${ARROWS_MVG_INCLUDE_DIRS})

include_directories( SYSTEM ${CMAKE_CURRENT_SOURCE_DIR} )

set(vxl_headers_public
  aligned_edge_detection.h
  average_frames.h
  bounding_box.h
  bundle_adjust.h
  camera.h
  camera_map.h
  close_loops_homography_guided.h
  color_commonality_filter.h
  compute_homography_overlap.h
  convert_image.h
  estimate_canonical_transform.h
  estimate_essential_matrix.h
  estimate_fundamental_matrix.h
  estimate_homography.h
  estimate_similarity_transform.h
  hashed_image_classifier_filter.h
  high_pass_filter.h
  image_container.h
  image_io.h
  kd_tree.h
  match_features_constrained.h
  morphology.h
  optimize_cameras.h
  pixel_feature_extractor.h
  polygon.h
  split_image.h
  threshold.h
  triangulate_landmarks.h
  vil_image_memory.h
  )

set(vxl_sources
  aligned_edge_detection.cxx
  average_frames.cxx
  bundle_adjust.cxx
  camera.cxx
  camera_map.cxx
  close_loops_homography_guided.cxx
  color_commonality_filter.cxx
  compute_homography_overlap.cxx
  convert_image.cxx
  estimate_canonical_transform.cxx
  estimate_essential_matrix.cxx
  estimate_fundamental_matrix.cxx
  estimate_homography.cxx
  estimate_similarity_transform.cxx
  hashed_image_classifier.cxx
  hashed_image_classifier_filter.cxx
  high_pass_filter.cxx
  image_container.cxx
  image_io.cxx
  kd_tree.cxx
  match_features_constrained.cxx
  morphology.cxx
  optimize_cameras.cxx
  pixel_feature_extractor.cxx
  polygon.cxx
  split_image.cxx
  threshold.cxx
  triangulate_landmarks.cxx
  vil_image_memory.cxx
  )

if( fletch_ENABLE_FFmpeg AND _FFmpeg_version VERSION_LESS 4 )
  list(APPEND vxl_headers_public
    vidl_ffmpeg_video_input.h
  )
  list(APPEND vxl_sources
    vidl_ffmpeg_video_input.cxx
  )
  add_definitions( -DVXL_ENABLE_FFMPEG )
endif()

kwiver_install_headers(
  SUBDIR     arrows/vxl
  ${vxl_headers_public}
  )

kwiver_install_headers(
  ${CMAKE_CURRENT_BINARY_DIR}/kwiver_algo_vxl_export.h
  NOPATH   SUBDIR     arrows/vxl
  )

kwiver_add_library( kwiver_algo_vxl
  ${vxl_headers_public}
  ${vxl_sources}
  )
target_link_libraries( kwiver_algo_vxl
  PUBLIC               vital_algo
                       vil vpgl vgl
  PRIVATE              kwiver_algo_klv
                       vital_config
                       kwiver_algo_vxl
                       rrel rsdl
                       vnl
                       vidl
                       vil_algo
                       vgl_algo
                       vpgl_algo
                       kwiversys
  )

algorithms_create_plugin( kwiver_algo_vxl
  register_algorithms.cxx
  )

# Add tests if enabled
if (ARROWS_VXL_ENABLE_TESTS)
  enable_testing()
  include(CTest)
  set(BUILD_TESTING    ON)  # needed for compatibility with kwiversys
  mark_as_advanced(BUILD_TESTING)
  include_directories(${KWIVER_TEST_INCLUDE_DIR})
  add_subdirectory( tests )
endif()

# Prepare space-separated list of library names for config
get_property(arrows_vxl_libs GLOBAL PROPERTY kwiver_libraries)
string(REPLACE ";" " " arrows_vxl_libs "${arrows_vxl_libs}")

set(ARROWS_VXL_CONFIG_FILE          "${ARROWS_VXL_BINARY_DIR}/arrows-vxl-config.cmake")
configure_file(
  "${ARROWS_VXL_CMAKE_DIR}/arrows-vxl-config-build.cmake.in"
  "${ARROWS_CONFIG_FILE}"
  @ONLY
  )

#TODO: Define package dependencies
set(ARROWS_VXL_DEPS "")

if(EXISTS /etc/redhat-release)
  file(READ /etc/redhat-release RHEL_VERSION)
  string(REGEX REPLACE ".*release ([^\\. ]*).*" "\\1" RHEL_VERSION "${RHEL_VERSION}")
  set(CPACK_SYSTEM_NAME "el${RHEL_VERSION}.${CMAKE_SYSTEM_PROCESSOR}")
  set(CPACK_RPM_PACKAGE_AUTOREQPROV " no")
  set(CPACK_RPM_PACKAGE_REQUIRES "${ARROWS_VXL_DEPS}")
else()
  set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(CPACK_PACKAGE_NAME              "ARROWS")
set(CPACK_PACKAGE_VENDOR            "Kitware, Inc.")
set(CPACK_PACKAGE_CONTACT           "kitware@kitware.com")
set(CPACK_MONOLITHIC_INSTALL        true)
set(CPACK_PACKAGE_VERSION_MAJOR     "${ARROWS_VXL_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR     "${ARROWS_VXL_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH     "${ARROWS_VXL_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION           "${ARROWS_VXL_VERSION}")
#set(CPACK_RESOURCE_FILE_LICENSE     "${KWIVER_DIR}/LICENSE")
set(CPACK_PACKAGING_INSTALL_PREFIX  "${CMAKE_INSTALL_PREFIX}")
set(CPACK_PACKAGE_FILE_NAME         "${CPACK_PACKAGE_NAME}-"
                                    "${CPACK_PACKAGE_VERSION}-"
                                    "${CPACK_SYSTEM_NAME}")

include (CPack)
