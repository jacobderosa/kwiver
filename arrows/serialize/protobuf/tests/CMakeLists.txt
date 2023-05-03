project(kwiver_serialize_protbuf_tests)

include(kwiver-test-setup)

set( test_libraries
  kwiver::vital    kwiver::vital_vpm kwiver::vital_algo    kwiver::vital_util
  kwiver_serialize_protobuf    kwiver::vital_types_protobuf )

##############################
# PROTOBUF tests
##############################

kwiver_discover_gtests(serialize-protobuf serialize          LIBRARIES ${test_libraries})
kwiver_discover_gtests(serialize-protobuf serialize_metadata LIBRARIES ${test_libraries})
kwiver_discover_gtests(serialize-protobuf convert_protobuf   LIBRARIES ${test_libraries})
