project(kwiver_serialize_json_tests)

set(CMAKE_FOLDER "Arrows/Serialize/Tests")

include(kwiver-test-setup)

set( test_libraries kwiver::vital kwiver::vital_vpm kwiver::vital_algo kwiver_serialize_json )

##############################
# JSON tests
##############################

kwiver_discover_gtests(serialize-json serialize         LIBRARIES ${test_libraries})
kwiver_discover_gtests(serialize-json load_save         LIBRARIES ${test_libraries})
