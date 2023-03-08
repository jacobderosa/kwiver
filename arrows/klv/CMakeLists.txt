# Build / Install Key-Length-Value Metadata Arrow

project(ARROWS_KLV)

###
# ARROWS-KLV version
set(ARROWS_KLV_VERSION_MAJOR 1)
set(ARROWS_KLV_VERSION_MINOR 7)
set(ARROWS_KLV_VERSION_PATCH 0)
set(ARROWS_KLV_VERSION "${ARROWS_KLV_VERSION_MAJOR}.${ARROWS_KLV_VERSION_MINOR}.${ARROWS_KLV_VERSION_PATCH}")

# Organize target into folders for IDEs that support it
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_FOLDER "KLV")
set(ARROWS_KLV_SOURCE_DIR           "${CMAKE_SOURCE_DIR}")
set(ARROWS_KLV_BINARY_DIR           "${CMAKE_BINARY_DIR}")

# Default library dir
set(ARROWS_KLV_DEFAULT_LIBRARY_DIR "lib" CACHE STRING
    "Default library directory for arrows")
mark_as_advanced(ARROWS_KLV_DEFAULT_LIBRARY_DIR)

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

set(ARROWS_KLV_CMAKE_DIR "${ARROWS_KLV_SOURCE_DIR}/CMake")
list(INSERT CMAKE_MODULE_PATH 0 ${KWIVER_CMAKE_DIR})

set(BUILD_SHARED_LIBS ${KWIVER_BUILT_SHARED})

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

set( sources
  apply_child_klv.cxx
  klv_blob.cxx
  klv_checksum.cxx
  klv_convert_vital.cxx
  klv_data_format.cxx
  klv_demuxer.cxx
  klv_lengthy.cxx
  klv_length_constraints.cxx
  klv_key.cxx
  klv_metadata.cxx
  klv_muxer.cxx
  klv_packet.cxx
  klv_read_write.cxx
  klv_set.cxx
  klv_tag_traits.cxx
  klv_timeline.cxx
  klv_unimplemented.cxx
  klv_update_intervals.cxx
  klv_update_tracker.cxx
  klv_uuid.cxx
  klv_value.cxx
  klv_0102.cxx
  klv_0104.cxx
  klv_0601.cxx
  klv_0806.cxx
  klv_0806_aoi_set.cxx
  klv_0806_poi_set.cxx
  klv_0806_user_defined_set.cxx
  klv_0903.cxx
  klv_0903_algorithm_set.cxx
  klv_0903_location_pack.cxx
  klv_0903_ontology_set.cxx
  klv_0903_vchip_set.cxx
  klv_0903_vfeature_set.cxx
  klv_0903_vmask_set.cxx
  klv_0903_vobject_set.cxx
  klv_0903_vtarget_pack.cxx
  klv_0903_vtrackitem_pack.cxx
  klv_0903_vtrack_set.cxx
  klv_0903_vtracker_set.cxx
  klv_1002.cxx
  klv_1010.cxx
  klv_1107.cxx
  klv_1108.cxx
  klv_1108_metric_set.cxx
  klv_1202.cxx
  klv_1204.cxx
  klv_1206.cxx
  klv_1303.cxx
  klv_1601.cxx
  klv_1602.cxx
  klv_1607.cxx
  misp_time.cxx
  update_klv.cxx
  )

set( public_headers
  apply_child_klv.h
  klv_all.h
  klv_blob.h
  klv_demuxer.h
  klv_lengthy.h
  klv_length_constraints.h
  klv_list.h
  klv_list.hpp
  klv_key.h
  klv_metadata.h
  klv_muxer.h
  klv_packet.h
  klv_series.h
  klv_set.h
  klv_timeline.h
  klv_types.h
  klv_unimplemented.h
  klv_update_intervals.h
  klv_value.h
  klv_0102.h
  klv_0104.h
  klv_0601.h
  klv_0806.h
  klv_0806_aoi_set.h
  klv_0806_poi_set.h
  klv_0806_user_defined_set.h
  klv_0903.h
  klv_0903_algorithm_set.h
  klv_0903_location_pack.h
  klv_0903_ontology_set.h
  klv_0903_vchip_set.h
  klv_0903_vfeature_set.h
  klv_0903_vmask_set.h
  klv_0903_vobject_set.h
  klv_0903_vtarget_pack.h
  klv_0903_vtrackitem_pack.h
  klv_0903_vtrack_set.h
  klv_0903_vtracker_set.h
  klv_1002.h
  klv_1010.h
  klv_1107.h
  klv_1108.h
  klv_1108_metric_set.h
  klv_1202.h
  klv_1204.h
  klv_1206.h
  klv_1303.h
  klv_1303.hpp
  klv_1601.h
  klv_1602.h
  klv_1607.h
  misp_time.h
  update_klv.h
  )

set( private_headers
  klv_data_format.h
  klv_length_value.h
  klv_key_traits.h
  klv_read_write.h
  klv_update_tracker.h
  klv_util.h
)

kwiver_install_headers(
  ${public_headers}
  SUBDIR   klv
  )

kwiver_install_headers(
  ${CMAKE_CURRENT_BINARY_DIR}/kwiver_algo_klv_export.h
  NOPATH
  SUBDIR      klv
  )

kwiver_add_library( kwiver_algo_klv
  ${sources}
  ${public_headers}
  ${private_headers}
  )

target_link_libraries( kwiver_algo_klv
  PUBLIC        vital_algo
  PRIVATE       vital
                vital_logger
  )


algorithms_create_plugin( kwiver_algo_klv
  register_algorithms.cxx
  )


if (KLV_ENABLE_TESTS)
  include_directories(${KWIVER_TEST_INCLUDE_DIR})
  add_subdirectory( tests )
endif()

# Prepare space-separated list of library names for config
get_property(arrows_klv_libs GLOBAL PROPERTY kwiver_libraries)
string(REPLACE ";" " " arrows_klv_libs "${arrows_klv_libs}")

set(ARROWS_KLV_CONFIG_FILE          "${ARROWS_KLV_BINARY_DIR}/arrows-klv-config.cmake")
configure_file(
  "${ARROWS_KLV_CMAKE_DIR}/arrows-klv-config-build.cmake.in"
  "${ARROWS_CONFIG_FILE}"
  @ONLY
  )

#TODO: Define package dependencies
set(ARROWS_KLV_DEPS "")

if(EXISTS /etc/redhat-release)
  file(READ /etc/redhat-release RHEL_VERSION)
  string(REGEX REPLACE ".*release ([^\\. ]*).*" "\\1" RHEL_VERSION "${RHEL_VERSION}")
  set(CPACK_SYSTEM_NAME "el${RHEL_VERSION}.${CMAKE_SYSTEM_PROCESSOR}")
  set(CPACK_RPM_PACKAGE_AUTOREQPROV " no")
  set(CPACK_RPM_PACKAGE_REQUIRES "${ARROWS_KLV_DEPS}")
else()
  set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(CPACK_PACKAGE_NAME              "ARROWS")
set(CPACK_PACKAGE_VENDOR            "Kitware, Inc.")
set(CPACK_PACKAGE_CONTACT           "kitware@kitware.com")
set(CPACK_MONOLITHIC_INSTALL        true)
set(CPACK_PACKAGE_VERSION_MAJOR     "${ARROWS_KLV_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR     "${ARROWS_KLV_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH     "${ARROWS_KLV_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION           "${ARROWS_KLV_VERSION}")
#set(CPACK_RESOURCE_FILE_LICENSE     "${KWIVER_DIR}/LICENSE")
set(CPACK_PACKAGING_INSTALL_PREFIX  "${CMAKE_INSTALL_PREFIX}")
set(CPACK_PACKAGE_FILE_NAME         "${CPACK_PACKAGE_NAME}-"
                                    "${CPACK_PACKAGE_VERSION}-"
                                    "${CPACK_SYSTEM_NAME}")

include (CPack)