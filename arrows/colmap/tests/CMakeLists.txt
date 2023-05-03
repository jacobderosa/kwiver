project(arrows_test_colmap)

set(CMAKE_FOLDER "Arrows/colmap/Tests")

include(kwiver-test-setup)

set(test_libraries      colmap kwiver::vital kwiver::vital_vpm kwiver_algo_colmap kwiver::kwiver_algo_ocv)

##############################
# Algorithms Colmap tests
##############################
kwiver_discover_gtests(colmap image                LIBRARIES ${test_libraries} ARGUMENTS "${kwiver_test_data_directory}")
