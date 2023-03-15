# Build / Install plugin containing OpenCV algorithm implementations + support
# structures

cmake_minimum_required(VERSION 3.15)
project(ARROWS_OCV)

###
# ARROWS-OCV version
set(ARROWS_OCV_VERSION_MAJOR 1)
set(ARROWS_OCV_VERSION_MINOR 7)
set(ARROWS_OCV_VERSION_PATCH 0)
set(ARROWS_OCV_VERSION "${ARROWS_OCV_VERSION_MAJOR}.${ARROWS_OCV_VERSION_MINOR}.${ARROWS_OCV_VERSION_PATCH}")

# Organize target into folders for IDEs that support it
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_FOLDER "OCV")
set(ARROWS_OCV_SOURCE_DIR           "${CMAKE_SOURCE_DIR}")
set(ARROWS_OCV_BINARY_DIR           "${CMAKE_BINARY_DIR}")

# Default library dir
set(ARROWS_OCV_DEFAULT_LIBRARY_DIR "lib" CACHE STRING
    "Default library directory for arrows")
mark_as_advanced(ARROWS_OCV_DEFAULT_LIBRARY_DIR)

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

set(ARROWS_OCV_CMAKE_DIR "${ARROWS_OCV_SOURCE_DIR}/CMake")
list(INSERT CMAKE_MODULE_PATH 0 ${KWIVER_CMAKE_DIR})

OPTION(ARROWS_OCV_ENABLE_TESTS                "Enable testing" OFF )
OPTION(ARROWS_OCV_BUILD_SHARED     "Build ARROWS-OCV components shared or not" TRUE )
MARK_AS_ADVANCED( ARROWS_OCV_BUILD_SHARED )
set(BUILD_SHARED_LIBS ${ARROWS_OCV_BUILT_SHARED})

# Install rules for CMake utilities
#include( kwiver-install-utils )
include( kwiver-utils )
include( kwiver-depends )

# project includes
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories(${KWIVER_INCLUDE_DIRS})
include_directories(${ARROWS_INCLUDE_DIRS})
include_directories(${ARROWS_MVG_INCLUDE_DIRS})

include_directories( SYSTEM ${CMAKE_CURRENT_SOURCE_DIR} )

set( ocv_headers_public
  analyze_tracks.h
  camera_intrinsics.h
  descriptor_set.h
  detect_features.h
  detect_features_AGAST.h
  detect_features_FAST.h
  detect_features_GFTT.h
  detect_features_MSD.h
  detect_features_MSER.h
  detect_features_simple_blob.h
  detect_features_STAR.h
  detect_motion_3frame_differencing.h
  detect_motion_mog2.h
  draw_detected_object_set.h
  draw_tracks.h
  estimate_fundamental_matrix.h
  estimate_homography.h
  estimate_pnp.h
  extract_descriptors.h
  extract_descriptors_BRIEF.h
  extract_descriptors_DAISY.h
  extract_descriptors_FREAK.h
  extract_descriptors_LATCH.h
  extract_descriptors_LUCID.h
  feature_set.h
  feature_detect_extract_BRISK.h
  feature_detect_extract_ORB.h
  feature_detect_extract_SIFT.h
  feature_detect_extract_SURF.h
  detect_heat_map.h
  hough_circle_detector.h
  image_container.h
  image_io.h
  inpaint.h
  mat_image_memory.h
  match_features.h
  match_features_bruteforce.h
  match_features_flannbased.h
  match_set.h
  merge_images.h
  refine_detections_write_to_disk.h
  resection_camera.h
  split_image.h
  track_features_klt.h
  )
kwiver_install_headers(
  SUBDIR     ocv
  ${ocv_headers_public}
  )
kwiver_install_headers(
  ${CMAKE_CURRENT_BINARY_DIR}/kwiver_algo_ocv_export.h
  NOPATH   SUBDIR     ocv
  )

set( ocv_sources
  analyze_tracks.cxx
  camera_intrinsics.cxx
  descriptor_set.cxx
  detect_features.cxx
  detect_features_AGAST.cxx
  detect_features_FAST.cxx
  detect_features_GFTT.cxx
  detect_features_MSD.cxx
  detect_features_MSER.cxx
  detect_features_simple_blob.cxx
  detect_features_STAR.cxx
  detect_motion_3frame_differencing.cxx
  detect_motion_mog2.cxx
  draw_detected_object_set.cxx
  draw_tracks.cxx
  estimate_fundamental_matrix.cxx
  estimate_homography.cxx
  estimate_pnp.cxx
  extract_descriptors.cxx
  extract_descriptors_BRIEF.cxx
  extract_descriptors_DAISY.cxx
  extract_descriptors_FREAK.cxx
  extract_descriptors_LATCH.cxx
  extract_descriptors_LUCID.cxx
  detect_heat_map.cxx
  hough_circle_detector.cxx
  feature_set.cxx
  feature_detect_extract_BRISK.cxx
  feature_detect_extract_ORB.cxx
  feature_detect_extract_SIFT.cxx
  feature_detect_extract_SURF.cxx
  image_container.cxx
  image_io.cxx
  inpaint.cxx
  mat_image_memory.cxx
  match_features.cxx
  match_features_bruteforce.cxx
  match_features_flannbased.cxx
  match_set.cxx
  merge_images.cxx
  refine_detections_write_to_disk.cxx
  resection_camera.cxx
  split_image.cxx
  track_features_klt.cxx
  )

kwiver_add_library( kwiver_algo_ocv
  ${ocv_headers_public}
  ${ocv_sources}
  )
target_compile_definitions( kwiver_algo_ocv
  PUBLIC KWIVER_OPENCV_VERSION_MAJOR=${KWIVER_OPENCV_VERSION_MAJOR}
  )
target_link_libraries( kwiver_algo_ocv
  PUBLIC               vital_algo
                       ${OpenCV_LIBS}
  PRIVATE              kwiversys
                       kwiver_algo_core
                       kwiver_algo_mvg
  )

algorithms_create_plugin( kwiver_algo_ocv
  register_algorithms.cxx
  )

if (ARROWS_OCV_ENABLE_TESTS)
  enable_testing()
  include(CTest)
  set(BUILD_TESTING    ON)  # needed for compatibility with kwiversys
  mark_as_advanced(BUILD_TESTING)
  include_directories(${KWIVER_TEST_INCLUDE_DIR})
  add_subdirectory( tests )
endif()

# Prepare space-separated list of library names for config
get_property(arrows_ocv_libs GLOBAL PROPERTY kwiver_libraries)
string(REPLACE ";" " " arrows_ocv_libs "${arrows_ocv_libs}")

set(ARROWS_OCV_CONFIG_FILE          "${ARROWS_OCV_BINARY_DIR}/arrows-ocv-config.cmake")
configure_file(
  "${ARROWS_OCV_CMAKE_DIR}/arrows-ocv-config-build.cmake.in"
  "${ARROWS_CONFIG_FILE}"
  @ONLY
  )

#TODO: Define package dependencies
set(ARROWS_OCV_DEPS "")

if(EXISTS /etc/redhat-release)
  file(READ /etc/redhat-release RHEL_VERSION)
  string(REGEX REPLACE ".*release ([^\\. ]*).*" "\\1" RHEL_VERSION "${RHEL_VERSION}")
  set(CPACK_SYSTEM_NAME "el${RHEL_VERSION}.${CMAKE_SYSTEM_PROCESSOR}")
  set(CPACK_RPM_PACKAGE_AUTOREQPROV " no")
  set(CPACK_RPM_PACKAGE_REQUIRES "${ARROWS_OCV_DEPS}")
else()
  set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(CPACK_PACKAGE_NAME              "ARROWS")
set(CPACK_PACKAGE_VENDOR            "Kitware, Inc.")
set(CPACK_PACKAGE_CONTACT           "kitware@kitware.com")
set(CPACK_MONOLITHIC_INSTALL        true)
set(CPACK_PACKAGE_VERSION_MAJOR     "${ARROWS_OCV_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR     "${ARROWS_OCV_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH     "${ARROWS_OCV_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION           "${ARROWS_OCV_VERSION}")
#set(CPACK_RESOURCE_FILE_LICENSE     "${KWIVER_DIR}/LICENSE")
set(CPACK_PACKAGING_INSTALL_PREFIX  "${CMAKE_INSTALL_PREFIX}")
set(CPACK_PACKAGE_FILE_NAME         "${CPACK_PACKAGE_NAME}-"
                                    "${CPACK_PACKAGE_VERSION}-"
                                    "${CPACK_SYSTEM_NAME}")

include (CPack)
