# Build / Install VTK arrow

cmake_minimum_required(VERSION 3.15)
project(ARROWS_VTK)

###
# ARROWS-VTK version
set(ARROWS_VTK_VERSION_MAJOR 1)
set(ARROWS_VTK_VERSION_MINOR 7)
set(ARROWS_VTK_VERSION_PATCH 0)
set(ARROWS_VTK_VERSION "${ARROWS_VTK_VERSION_MAJOR}.${ARROWS_VTK_VERSION_MINOR}.${ARROWS_VTK_VERSION_PATCH}")

# Organize target into folders for IDEs that support it
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_FOLDER "VTK")

# Default library dir
set(ARROWS_VTK_DEFAULT_LIBRARY_DIR "lib" CACHE STRING
    "Default library directory for arrows")
mark_as_advanced(ARROWS_VTK_DEFAULT_LIBRARY_DIR)

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

set(ARROWS_VTK_CMAKE_DIR "${ARROWS_VTK_SOURCE_DIR}/CMake")
list(INSERT CMAKE_MODULE_PATH 0 ${KWIVER_CMAKE_DIR})

OPTION(ARROWS_VTK_ENABLE_TESTS                "Enable testing" OFF )
OPTION(ARROWS_VTK_BUILD_SHARED     "Build ARROWS-VTK components shared or not" TRUE )
MARK_AS_ADVANCED( ARROWS_VTK_BUILD_SHARED )
set(BUILD_SHARED_LIBS ${ARROWS_VTK_BUILD_SHARED})

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

set( plugin_vtk_headers
  depth_utils.h
  vtkKwiverCamera.h
  )

kwiver_install_headers(
  SUBDIR     arrows/vtk
  ${plugin_vtk_headers}
  )

kwiver_install_headers(
  ${CMAKE_CURRENT_BINARY_DIR}/kwiver_algo_vtk_export.h
  NOPATH   SUBDIR     arrows/vtk
  )

set( plugin_vtk_sources
  depth_utils.cxx
  vtkKwiverCamera.cxx
  )

if (VTK_VERSION VERSION_GREATER_EQUAL 9.1)
  list(APPEND plugin_vtk_headers mesh_coloration.h)
  list(APPEND plugin_vtk_sources mesh_coloration.cxx)
  add_definitions( -DVTK_ENABLE_COLOR_MESH )
endif()

kwiver_add_library( kwiver_algo_vtk
  ${plugin_vtk_headers}
  ${plugin_vtk_sources}
  )

if(VTK_VERSION VERSION_LESS 9.0)
  set(VTK_public_targets
      vtkRenderingCore
      vtkRenderingOpenGL2)
  set(VTK_private_targets
      vtkCommonCore
      vtkCommonDataModel
      vtkIOImage
      vtkIOXML)
else()
  set(VTK_public_targets
      VTK::RenderingCore
      VTK::RenderingOpenGL2)
  set(VTK_private_targets
      VTK::CommonCore
      VTK::CommonDataModel
      VTK::IOImage
      VTK::IOXML)
  vtk_module_autoinit(
    TARGETS kwiver_algo_vtk
    MODULES ${VTK_public_targets} ${VTK_private_targets})
endif()

target_link_libraries( kwiver_algo_vtk
  PUBLIC               kwiver::vital
                       ${VTK_public_targets}
  PRIVATE              kwiver::vital_algo
                       ${VTK_private_targets}
  )

target_include_directories( kwiver_algo_vtk SYSTEM
  PUBLIC                    ${KWIVER_INCLUDE_DIRS}
  )

#algorithms_create_plugin( kwiver_algo_vtk
#  register_algorithms.cxx
#  )

if (ARROWS_VTK_ENABLE_TESTS)
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
get_property(arrows_vtk_libs GLOBAL PROPERTY kwiver_libraries)
string(REPLACE ";" " " arrows_vtk_libs "${arrows_vtk_libs}")

set(ARROWS_VTK_CONFIG_FILE          "${ARROWS_VTK_BINARY_DIR}/arrows-vtk-config.cmake")
configure_file(
  "${ARROWS_VTK_CMAKE_DIR}/arrows-vtk-config-build.cmake.in"
  "${ARROWS_CONFIG_FILE}"
  @ONLY
  )

kwiver_export_targets("${ARROWS_VTK_BINARY_DIR}/arrows-vtk-config-targets.cmake")

#TODO: Define package dependencies
set(ARROWS_VTK_DEPS "")

if(EXISTS /etc/redhat-release)
  file(READ /etc/redhat-release RHEL_VERSION)
  string(REGEX REPLACE ".*release ([^\\. ]*).*" "\\1" RHEL_VERSION "${RHEL_VERSION}")
  set(CPACK_SYSTEM_NAME "el${RHEL_VERSION}.${CMAKE_SYSTEM_PROCESSOR}")
  set(CPACK_RPM_PACKAGE_AUTOREQPROV " no")
  set(CPACK_RPM_PACKAGE_REQUIRES "${ARROWS_VTK_DEPS}")
else()
  set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(CPACK_PACKAGE_NAME              "ARROWS")
set(CPACK_PACKAGE_VENDOR            "Kitware, Inc.")
set(CPACK_PACKAGE_CONTACT           "kitware@kitware.com")
set(CPACK_MONOLITHIC_INSTALL        true)
set(CPACK_PACKAGE_VERSION_MAJOR     "${ARROWS_VTK_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR     "${ARROWS_VTK_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH     "${ARROWS_VTK_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION           "${ARROWS_VTK_VERSION}")
#set(CPACK_RESOURCE_FILE_LICENSE     "${KWIVER_DIR}/LICENSE")
set(CPACK_PACKAGING_INSTALL_PREFIX  "${CMAKE_INSTALL_PREFIX}")
set(CPACK_PACKAGE_FILE_NAME         "${CPACK_PACKAGE_NAME}-"
                                    "${CPACK_PACKAGE_VERSION}-"
                                    "${CPACK_SYSTEM_NAME}")

include (CPack)