// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// \brief Test KLV 0601 read / write.

#include "data_format.h"

#include <arrows/klv/klv_0601.h>
#include <arrows/klv/klv_1204.h>

// ----------------------------------------------------------------------------
int
main( int argc, char** argv )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

// ----------------------------------------------------------------------------
void
test_read_write( klv_value const& expected_result,
                 klv_bytes_t const& input_bytes )
{
  using format_t = klv_0601_local_set_format;
  test_read_write_format< format_t >( expected_result, input_bytes );
}

using kld = klv_lengthy< double >;
auto const expected_result = klv_local_set{
  { KLV_0601_PRECISION_TIMESTAMP,
    uint64_t{ 0x000459F4A6AA4AA8 } },
  { KLV_0601_MISSION_ID,                       std::string{ "MISSION01" } },
  { KLV_0601_PLATFORM_TAIL_NUMBER,             std::string{ "AF-101" } },
  { KLV_0601_PLATFORM_HEADING_ANGLE,           kld{  159.97436484321355 } },
  { KLV_0601_PLATFORM_PITCH_ANGLE,             kld{ -0.4315317239906003 } },
  { KLV_0601_PLATFORM_ROLL_ANGLE,              kld{  3.4058656575212867 } },
  { KLV_0601_PLATFORM_TRUE_AIRSPEED,           kld{  147.0 } },
  { KLV_0601_PLATFORM_INDICATED_AIRSPEED,      kld{  159.0 } },
  { KLV_0601_PLATFORM_DESIGNATION,             std::string{ "MQ1-B" } },
  { KLV_0601_IMAGE_SOURCE_SENSOR,              std::string{ "EO" } },
  { KLV_0601_IMAGE_COORDINATE_SYSTEM,
    std::string{ "Geodetic WGS84" } },
  { KLV_0601_SENSOR_LATITUDE,                  kld{  60.176822966978335 } },
  { KLV_0601_SENSOR_LONGITUDE,                 kld{  128.42675904204452 } },
  { KLV_0601_SENSOR_TRUE_ALTITUDE,             kld{  14190.719462882427 } },
  { KLV_0601_SENSOR_HORIZONTAL_FOV,            kld{  144.57129777981231 } },
  { KLV_0601_SENSOR_VERTICAL_FOV,              kld{  152.64362554360267 } },
  { KLV_0601_SENSOR_RELATIVE_AZIMUTH_ANGLE,    kld{  160.71921143697557 } },
  { KLV_0601_SENSOR_RELATIVE_ELEVATION_ANGLE,  kld{ -168.79232483394085 } },
  { KLV_0601_SENSOR_RELATIVE_ROLL_ANGLE,       kld{  176.86543764939194 } },
  { KLV_0601_SLANT_RANGE,                      kld{  68590.983298744770 } },
  { KLV_0601_TARGET_WIDTH,                     kld{  722.81986724650960 } },
  { KLV_0601_FRAME_CENTER_LATITUDE,            kld{ -10.542388633146132 } },
  { KLV_0601_FRAME_CENTER_LONGITUDE,           kld{  29.157890122923018 } },
  { KLV_0601_FRAME_CENTER_ELEVATION,           kld{  3216.0372320134275 } },
  { KLV_0601_OFFSET_CORNER_LATITUDE_POINT_1,   kld{  0.013660084841456343 } },
  { KLV_0601_OFFSET_CORNER_LONGITUDE_POINT_1,  kld{  0.0036599322489089632 } },
  { KLV_0601_OFFSET_CORNER_LATITUDE_POINT_2,   kld{ -0.0036599322489089632 } },
  { KLV_0601_OFFSET_CORNER_LONGITUDE_POINT_2,  kld{  0.013660084841456343 } },
  { KLV_0601_OFFSET_CORNER_LATITUDE_POINT_3,   kld{ -0.011062196722312082 } },
  { KLV_0601_OFFSET_CORNER_LONGITUDE_POINT_3,  kld{ -0.005159154026917325 } },
  { KLV_0601_OFFSET_CORNER_LATITUDE_POINT_4,   kld{  0.0010620441297647023 } },
  { KLV_0601_OFFSET_CORNER_LONGITUDE_POINT_4,  kld{ -0.01216086306344798 } },
  { KLV_0601_ICING_DETECTED,
    KLV_0601_ICING_DETECTED_TRUE },
  { KLV_0601_WIND_DIRECTION,                   kld{  235.92401007095447 } },
  { KLV_0601_WIND_SPEED,                       kld{  69.803921568627445 } },
  { KLV_0601_STATIC_PRESSURE,                  kld{  3725.1850156404976 } },
  { KLV_0601_DENSITY_ALTITUDE,                 kld{  14818.677042801555 } },
  { KLV_0601_OUTSIDE_AIR_TEMPERATURE,          kld{  84.0 } },
  { KLV_0601_TARGET_LOCATION_LATITUDE,         kld{ -79.163850051892850 } },
  { KLV_0601_TARGET_LOCATION_LONGITUDE,        kld{  166.40081296041646 } },
  { KLV_0601_TARGET_LOCATION_ELEVATION,        kld{  18389.047074082551 } },
  { KLV_0601_TARGET_TRACK_GATE_WIDTH,          kld{  6.0 } },
  { KLV_0601_TARGET_TRACK_GATE_HEIGHT,         kld{  30.0 } },
  { KLV_0601_TARGET_ERROR_ESTIMATE_CE90,       kld{  425.21515220874340 } },
  { KLV_0601_TARGET_ERROR_ESTIMATE_LE90,       kld{  608.92309452964071 } },
  { KLV_0601_GENERIC_FLAG_DATA,
    uint64_t{ 1 << KLV_0601_GENERIC_FLAG_DATA_BIT_LASER_RANGE |
              1 << KLV_0601_GENERIC_FLAG_DATA_BIT_SLANT_RANGE |
              1 << KLV_0601_GENERIC_FLAG_DATA_BIT_IMAGE_INVALID } },
  { KLV_0601_SECURITY_LOCAL_SET,               {} },
  { KLV_0601_DIFFERENTIAL_PRESSURE,            kld{ 1191.9584954604411 } },
  { KLV_0601_PLATFORM_ANGLE_OF_ATTACK,         kld{-8.6703085421308028 } },
  { KLV_0601_PLATFORM_VERTICAL_SPEED,          kld{-61.887874996185190 } },
  { KLV_0601_PLATFORM_SIDESLIP_ANGLE,          kld{-5.0825525681325727 } },
  { KLV_0601_AIRFIELD_BAROMETRIC_PRESSURE,     kld{ 2088.9600976577403 } },
  { KLV_0601_AIRFIELD_ELEVATION,               kld{ 8306.8055237659264 } },
  { KLV_0601_RELATIVE_HUMIDITY,                kld{ 50.588235294117645 } },
  { KLV_0601_PLATFORM_GROUND_SPEED,            kld{ 140.0 } },
  { KLV_0601_GROUND_RANGE,                     kld{ 3506979.0316063400 } },
  { KLV_0601_PLATFORM_FUEL_REMAINING,          kld{ 6420.5386434729535 } },
  { KLV_0601_PLATFORM_CALL_SIGN,               std::string{ "TOP GUN" } },
  { KLV_0601_WEAPON_LOAD,                      uint64_t{ 45016 } },
  { KLV_0601_WEAPON_FIRED,                     uint64_t{ 186 } },
  { KLV_0601_LASER_PRF_CODE,                   uint64_t{ 1743 } },
  { KLV_0601_SENSOR_FOV_NAME,
    KLV_0601_SENSOR_FOV_NAME_MEDIUM },
  { KLV_0601_PLATFORM_MAGNETIC_HEADING,        kld{ 311.86816205081254 } },
  { KLV_0601_VERSION_NUMBER,                   uint64_t{ 13 } },
  { KLV_0601_ALTERNATE_PLATFORM_LATITUDE,      kld{ -86.041207348947040 } },
  { KLV_0601_ALTERNATE_PLATFORM_LONGITUDE,     kld{  0.15552755452484243 } },
  { KLV_0601_ALTERNATE_PLATFORM_ALTITUDE,      kld{  9.4453345540549662 } },
  { KLV_0601_ALTERNATE_PLATFORM_NAME,          std::string{ "APACHE" } },
  { KLV_0601_ALTERNATE_PLATFORM_HEADING,       kld{  32.60242618448158 } },
  { KLV_0601_EVENT_START_TIME,                 uint64_t{ 798039894000000 } },
  { KLV_0601_RVT_LOCAL_SET,                    {} },
  { KLV_0601_VMTI_LOCAL_SET,                   {} },
  { KLV_0601_SENSOR_ELLIPSOID_HEIGHT,          kld{  14190.719462882427 } },
  { KLV_0601_ALTERNATE_PLATFORM_ELLIPSOID_HEIGHT,
    kld{ 9.4453345540549662 } },
  { KLV_0601_OPERATIONAL_MODE,
    KLV_0601_OPERATIONAL_MODE_OPERATIONAL },
  { KLV_0601_FRAME_CENTER_HEIGHT_ABOVE_ELLIPSOID,
    kld{ 9.4453345540549662 } },
  { KLV_0601_SENSOR_NORTH_VELOCITY,            kld{  25.497756889553518 } },
  { KLV_0601_SENSOR_EAST_VELOCITY,             kld{  12.095217749565112 } },
  { KLV_0601_IMAGE_HORIZON_PIXEL_PACK,
    klv_blob{ { 0x00, 0x24, 0x38, 0x00 } } },
  { KLV_0601_FULL_CORNER_LATITUDE_POINT_1,     kld{ -10.528728394084018 } },
  { KLV_0601_FULL_CORNER_LONGITUDE_POINT_1,    kld{  29.161550416220702 } },
  { KLV_0601_FULL_CORNER_LATITUDE_POINT_2,     kld{ -10.546048884534299 } },
  { KLV_0601_FULL_CORNER_LONGITUDE_POINT_2,    kld{  29.171550361985130 } },
  { KLV_0601_FULL_CORNER_LATITUDE_POINT_3,     kld{ -10.553450817499986 } },
  { KLV_0601_FULL_CORNER_LONGITUDE_POINT_3,    kld{  29.152729888052090 } },
  { KLV_0601_FULL_CORNER_LATITUDE_POINT_4,     kld{ -10.541326436466223 } },
  { KLV_0601_FULL_CORNER_LONGITUDE_POINT_4,    kld{  29.145729909253181 } },
  { KLV_0601_FULL_PLATFORM_PITCH_ANGLE,        kld{ -0.43152510208614409 } },
  { KLV_0601_FULL_PLATFORM_ROLL_ANGLE,         kld{  3.4058139815022304 } },
  { KLV_0601_FULL_PLATFORM_ANGLE_OF_ATTACK,    kld{ -8.6701769841230370 } },
  { KLV_0601_FULL_PLATFORM_SIDESLIP_ANGLE,     kld{ -47.683000037299003 } },
  { KLV_0601_MIIS_CORE_IDENTIFIER,
    klv_1204_miis_id{
      1, KLV_1204_DEVICE_ID_TYPE_PHYSICAL, KLV_1204_DEVICE_ID_TYPE_VIRTUAL,
      { { 0xF5, 0x92, 0xF0, 0x23, 0x73, 0x36, 0x4A, 0xF8, 0xAA, 0x91, 0x62,
        0xC0, 0x0F, 0x2E, 0xB2, 0xDA } },
      { { 0x16, 0xB7, 0x43, 0x41, 0x00, 0x08, 0x41, 0xA0, 0xBE, 0x36, 0x5B,
        0x5A, 0xB9, 0x6A, 0x36, 0x45 } } } },
  { KLV_0601_SAR_MOTION_IMAGERY_LOCAL_SET,     {} },
  { KLV_0601_TARGET_WIDTH_EXTENDED,            kld{ 13898.5000000000000 } },
  { KLV_0601_RANGE_IMAGE_LOCAL_SET,            {} },
  { KLV_0601_GEOREGISTRATION_LOCAL_SET,        {} },
  { KLV_0601_COMPOSITE_IMAGING_LOCAL_SET,      {} },
  { KLV_0601_SEGMENT_LOCAL_SET,                {} },
  { KLV_0601_AMEND_LOCAL_SET,                  {} },
  { KLV_0601_SDCC_FLP,                         {} },
  { KLV_0601_DENSITY_ALTITUDE_EXTENDED,        kld{ 23456.234375000000 } },
  { KLV_0601_SENSOR_ELLIPSOID_HEIGHT_EXTENDED, kld{ 23456.234375000000 } },
  { KLV_0601_ALTERNATE_PLATFORM_ELLIPSOID_HEIGHT_EXTENDED,
    kld{ 23456.234375000000 } },
  { KLV_0601_STREAM_DESIGNATOR,                std::string{ "BLUE" } },
  { KLV_0601_OPERATIONAL_BASE,                 std::string{ "BASE01" } },
  { KLV_0601_BROADCAST_SOURCE,                 std::string{ "HOME" } },
  { KLV_0601_RANGE_TO_RECOVERY_LOCATION,       kld{ 1.6250000000000000 } },
  { KLV_0601_TIME_AIRBORNE,                    uint64_t{ 19887 } },
  { KLV_0601_PROPULSION_UNIT_SPEED,            uint64_t{ 3000 } },
  { KLV_0601_PLATFORM_COURSE_ANGLE,            kld{ 125.00000000000000 } },
  { KLV_0601_ALTITUDE_ABOVE_GROUND_LEVEL,      kld{ 2150.0000000000000 } },
  { KLV_0601_RADAR_ALTIMETER,                  kld{ 2154.5000000000000 } },
  { KLV_0601_CONTROL_COMMAND,
    klv_0601_control_command{ 5, "Fly to Waypoint 1", 0 } },
  { KLV_0601_CONTROL_COMMAND_VERIFICATION_LIST,
    std::vector< uint64_t >{ 3, 7 } },
  { KLV_0601_SENSOR_AZIMUTH_RATE,              kld{  1.0 } },
  { KLV_0601_SENSOR_ELEVATION_RATE,            kld{  0.004150390625000000 } },
  { KLV_0601_SENSOR_ROLL_RATE,                 kld{ -50.000000000000000 } },
  { KLV_0601_ONBOARD_MI_STORAGE_PERCENT_FULL,  kld{  72.000000000000000 } },
  { KLV_0601_ACTIVE_WAVELENGTH_LIST,           klv_blob{ { 0x01, 0x03 } } },
  { KLV_0601_COUNTRY_CODES,
    klv_0601_country_codes{
      KLV_0102_COUNTRY_CODING_METHOD_GENC_THREE_LETTER,
      std::string{ "CAN" }, kv::nullopt, std::string{ "FRA" } } },
  { KLV_0601_NUMBER_OF_NAVSATS_IN_VIEW,        uint64_t{ 7 } },
  { KLV_0601_POSITIONING_METHOD_SOURCE,
    uint64_t{ 1 << KLV_0601_POSITIONING_METHOD_SOURCE_BIT_ON_BOARD_INS |
              1 << KLV_0601_POSITIONING_METHOD_SOURCE_BIT_GPS } },
  { KLV_0601_PLATFORM_STATUS,
    KLV_0601_PLATFORM_STATUS_EGRESS },
  { KLV_0601_SENSOR_CONTROL_MODE,
    KLV_0601_SENSOR_CONTROL_MODE_AUTO_HOLDING_POSITION },
  { KLV_0601_SENSOR_FRAME_RATE_PACK,
    klv_0601_frame_rate{ 60000, 1001 } },
  { KLV_0601_WAVELENGTHS_LIST,
    klv_blob{ { 0x0D, 0x15, 0x00, 0x00, 0x07, 0xD0, 0x00, 0x00, 0x0F, 0xA0,
      0x4E, 0x4E, 0x49, 0x52 } } },
  { KLV_0601_TARGET_ID,                        std::string{ "A123" } },
  { KLV_0601_AIRBASE_LOCATIONS,
    klv_0601_airbase_locations{
      klv_0601_location_dlp{
        38.8418589830398559, -77.0367841720581054, 3.0 },
      klv_0601_location_dlp{
        38.9393529891967773, -77.4598112106323242, 95.0 } } },
  { KLV_0601_TAKEOFF_TIME,
    uint64_t{ 1529588637122999 } },
  { KLV_0601_TRANSMISSION_FREQUENCY,           kld{ 2400.0000000000000 } },
  { KLV_0601_ONBOARD_MI_STORAGE_CAPACITY,      uint64_t{ 10000 } },
  { KLV_0601_ZOOM_PERCENTAGE,                  kld{ 55.000000000000000 } },
  { KLV_0601_COMMUNICATIONS_METHOD,
    std::string{ "Frequency Modulation" } },
  { KLV_0601_LEAP_SECONDS,                     int64_t{ 30 } },
  { KLV_0601_CORRECTION_OFFSET,                int64_t{ 5025678901 } },
  { KLV_0601_PAYLOAD_LIST,
    std::vector< klv_0601_payload_record >{
      { 0, KLV_0601_PAYLOAD_TYPE_ELECTRO_OPTICAL, "VIS Nose Camera" },
      { 1, KLV_0601_PAYLOAD_TYPE_ELECTRO_OPTICAL, "ACME VIS Model 123" },
      { 2, KLV_0601_PAYLOAD_TYPE_ELECTRO_OPTICAL, "ACME IR Model 456" }
    }
  },
  { KLV_0601_ACTIVE_PAYLOADS,                  klv_blob{ { 0x0B } } },
  { KLV_0601_WEAPONS_STORES,
    std::vector< klv_0601_weapons_store >{
      { 1, 1, 1, 3,
        KLV_0601_WEAPONS_GENERAL_STATUS_ALL_UP_ROUND,
        uint8_t{ 1 << KLV_0601_WEAPON_ENGAGEMENT_STATUS_BIT_FUSE_ENABLED },
        "Harpoon" },
      { 1, 1, 2, 2,
        KLV_0601_WEAPONS_GENERAL_STATUS_LAUNCH,
        uint8_t{
          1 << KLV_0601_WEAPON_ENGAGEMENT_STATUS_BIT_FUSE_ENABLED |
          1 << KLV_0601_WEAPON_ENGAGEMENT_STATUS_BIT_LASER_ENABLED |
          1 << KLV_0601_WEAPON_ENGAGEMENT_STATUS_BIT_TARGET_ENABLED |
          1 << KLV_0601_WEAPON_ENGAGEMENT_STATUS_BIT_WEAPON_ARMED },
        "Hellfire" },
      { 1, 2, 1, 1,
        KLV_0601_WEAPONS_GENERAL_STATUS_ALL_UP_ROUND,
        uint8_t{ 0 },
        "GBU-15" }
    }
  },
  { KLV_0601_WAYPOINT_LIST,
    klv_blob{ {
      0x0F, 0x00, 0x00, 0x01, 0x03, 0x40, 0x71, 0xD8, 0x94, 0x19, 0xBD, 0xBF,
      0xE7, 0x08, 0x98, 0x00, 0x0F, 0x01, 0x00, 0x02, 0x02, 0x40, 0x71, 0xD3,
      0x88, 0x19, 0xBC, 0xCE, 0x24, 0x08, 0xFC, 0x00, 0x0F, 0x02, 0x7F, 0xFF,
      0x01, 0x40, 0x71, 0xE3, 0x08, 0x19, 0xBF, 0x2C, 0x1B, 0x07, 0xD0, 0x00,
      0x0F, 0x03, 0xFF, 0xFE, 0x00, 0x40, 0x71, 0xE5, 0xAF, 0x19, 0xBF, 0x5A,
      0xA7, 0x09, 0x60, 0x00 } } },
  { KLV_0601_VIEW_DOMAIN,
    klv_0601_view_domain{
      kld{ 210.00000000000000 }, kld{ 300.00000000000000 },
      kld{ -75.000000000000000 }, kld{ 50.000000000000000 } } } };

auto const input_bytes = klv_bytes_t{
  0x02, 0x08, // KLV_0601_PRECISION_TIMESTAMP
  0x00, 0x04, 0x59, 0xF4, 0xA6, 0xAA, 0x4A, 0xA8,
  0x03, 0x09, // KLV_0601_MISSION_ID
  0x4D, 0x49, 0x53, 0x53, 0x49, 0x4F, 0x4E, 0x30, 0x31,
  0x04, 0x06, // KLV_0601_PLATFORM_TAIL_NUMBER
  0x41, 0x46, 0x2D, 0x31, 0x30, 0x31,
  0x05, 0x02, // KLV_0601_PLATFORM_HEADING_ANGLE
  0x71, 0xC2,
  0x06, 0x02, // KLV_0601_PLATFORM_PITCH_ANGLE
  0xFD, 0x3D,
  0x07, 0x02, // KLV_0601_PLATFORM_ROLL_ANGLE
  0x08, 0xB8,
  0x08, 0x01, // KLV_0601_PLATFORM_TRUE_AIRSPEED
  0x93,
  0x09, 0x01, // KLV_0601_PLATFORM_INDICATED_AIRSPEED
  0x9F,
  0x0A, 0x05, // KLV_0601_PLATFORM_DESIGNATION
  0x4D, 0x51, 0x31, 0x2D, 0x42,
  0x0B, 0x02, // KLV_0601_IMAGE_SOURCE_SENSOR
  0x45, 0x4F,
  0x0C, 0x0E, // KLV_0601_IMAGE_COORDINATE_SYSTEM
  0x47, 0x65, 0x6F, 0x64, 0x65, 0x74, 0x69, 0x63, 0x20, 0x57, 0x47, 0x53,
  0x38, 0x34,
  0x0D, 0x04, // KLV_0601_SENSOR_LATITUDE
  0x55, 0x95, 0xB6, 0x6D,
  0x0E, 0x04, // KLV_0601_SENSOR_LONGITUDE
  0x5B, 0x53, 0x60, 0xC4,
  0x0F, 0x02, // KLV_0601_SENSOR_TRUE_ALTITUDE
  0xC2, 0x21,
  0x10, 0x02, // KLV_0601_SENSOR_HORIZONTAL_FOV
  0xCD, 0x9C,
  0x11, 0x02, // KLV_0601_SENSOR_VERTICAL_FOV
  0xD9, 0x17,
  0x12, 0x04, // KLV_0601_SENSOR_RELATIVE_AZIMUTH_ANGLE
  0x72, 0x4A, 0x0A, 0x20,
  0x13, 0x04, // KLV_0601_SENSOR_RELATIVE_ELEVATION_ANGLE
  0x87, 0xF8, 0x4B, 0x86,
  0x14, 0x04, // KLV_0601_SENSOR_RELATIVE_ROLL_ANGLE
  0x7D, 0xC5, 0x5E, 0xCE,
  0x15, 0x04, // KLV_0601_SLANT_RANGE
  0x03, 0x83, 0x09, 0x26,
  0x16, 0x02, // KLV_0601_TARGET_WIDTH
  0x12, 0x81,
  0x17, 0x04, // KLV_0601_FRAME_CENTER_LATITUDE
  0xF1, 0x01, 0xA2, 0x29,
  0x18, 0x04, // KLV_0601_FRAME_CENTER_LONGITUDE
  0x14, 0xBC, 0x08, 0x2B,
  0x19, 0x02, // KLV_0601_FRAME_CENTER_ELEVATION
  0x34, 0xF3,
  0x1A, 0x02, // KLV_0601_OFFSET_CORNER_LATITUDE_POINT_1
  0x17, 0x50,
  0x1B, 0x02, // KLV_0601_OFFSET_CORNER_LONGITUDE_POINT_1
  0x06, 0x3F,
  0x1C, 0x02, // KLV_0601_OFFSET_CORNER_LATITUDE_POINT_2
  0xF9, 0xC1,
  0x1D, 0x02, // KLV_0601_OFFSET_CORNER_LONGITUDE_POINT_2
  0x17, 0x50,
  0x1E, 0x02, // KLV_0601_OFFSET_CORNER_LATITUDE_POINT_3
  0xED, 0x1F,
  0x1F, 0x02, // KLV_0601_OFFSET_CORNER_LONGITUDE_POINT_3
  0xF7, 0x32,
  0x20, 0x02, // KLV_0601_OFFSET_CORNER_LATITUDE_POINT_4
  0x01, 0xD0,
  0x21, 0x02, // KLV_0601_OFFSET_CORNER_LONGITUDE_POINT_4
  0xEB, 0x3F,
  0x22, 0x01, // KLV_0601_ICING_DETECTED
  0x02,
  0x23, 0x02, // KLV_0601_WIND_DIRECTION
  0xA7, 0xC4,
  0x24, 0x01, // KLV_0601_WIND_SPEED
  0xB2,
  0x25, 0x02, // KLV_0601_STATIC_PRESSURE
  0xBE, 0xBA,
  0x26, 0x02, // KLV_0601_DENSITY_ALTITUDE
  0xCA, 0x35,
  0x27, 0x01, // KLV_0601_OUTSIDE_AIR_TEMPERATURE
  0x54,
  0x28, 0x04, // KLV_0601_TARGET_LOCATION_LATITUDE
  0x8F, 0x69, 0x52, 0x62,
  0x29, 0x04, // KLV_0601_TARGET_LOCATION_LONGITUDE
  0x76, 0x54, 0x57, 0xF2,
  0x2A, 0x02, // KLV_0601_TARGET_LOCATION_ELEVATION
  0xF8, 0x23,
  0x2B, 0x01, // KLV_0601_TARGET_TRACK_GATE_WIDTH
  0x03,
  0x2C, 0x01, // KLV_0601_TARGET_TRACK_GATE_HEIGHT
  0x0F,
  0x2D, 0x02, // KLV_0601_TARGET_ERROR_ESTIMATE_CE90
  0x1A, 0x95,
  0x2E, 0x02, // KLV_0601_TARGET_ERROR_ESTIMATE_LE90
  0x26, 0x11,
  0x2F, 0x01, // KLV_0601_GENERIC_FLAG_DATA
  0x31,
  0x30, 0x00, // KLV_0601_SECURITY_LOCAL_SET
  0x31, 0x02, // KLV_0601_DIFFERENTIAL_PRESSURE
  0x3D, 0x07,
  0x32, 0x02, // KLV_0601_PLATFORM_ANGLE_OF_ATTACK
  0xC8, 0x83,
  0x33, 0x02, // KLV_0601_PLATFORM_VERTICAL_SPEED
  0xD3, 0xFE,
  0x34, 0x02, // KLV_0601_PLATFORM_SIDESLIP_ANGLE
  0xDF, 0x79,
  0x35, 0x02, // KLV_0601_AIRFIELD_BAROMETRIC_PRESSURE
  0x6A, 0xF4,
  0x36, 0x02, // KLV_0601_AIRFIELD_ELEVATION
  0x76, 0x70,
  0x37, 0x01, // KLV_0601_RELATIVE_HUMIDITY
  0x81,
  0x38, 0x01, // KLV_0601_PLATFORM_GROUND_SPEED
  0x8C,
  0x39, 0x04, // KLV_0601_GROUND_RANGE
  0xB3, 0x8E, 0xAC, 0xF1,
  0x3A, 0x02, // KLV_0601_PLATFORM_FUEL_REMAINING
  0xA4, 0x5D,
  0x3B, 0x07, // KLV_0601_PLATFORM_CALL_SIGN
  0x54, 0x4F, 0x50, 0x20, 0x47, 0x55, 0x4E,
  0x3C, 0x02, // KLV_0601_WEAPON_LOAD
  0xAF, 0xD8,
  0x3D, 0x01, // KLV_0601_WEAPON_FIRED
  0xBA,
  0x3E, 0x02, // KLV_0601_LASER_PRF_CODE
  0x06, 0xCF,
  0x3F, 0x01, // KLV_0601_SENSOR_FOV_NAME_MEDIUM
  0x02,
  0x40, 0x02, // KLV_0601_PLATFORM_MAGNETIC_HEADING
  0xDD, 0xC5,
  0x41, 0x01, // KLV_0601_VERSION_NUMBER
  0x0D,
  0x43, 0x04, // KLV_0601_ALTERNATE_PLATFORM_LATITUDE
  0x85, 0xA1, 0x5A, 0x39,
  0x44, 0x04, // KLV_0601_ALTERNATE_PLATFORM_LONGITUDE
  0x00, 0x1C, 0x50, 0x1C,
  0x45, 0x02, // KLV_0601_ALTERNATE_PLATFORM_ALTITUDE
  0x0B, 0xB3,
  0x46, 0x06, // KLV_0601_ALTERNATE_PLATFORM_NAME
  0x41, 0x50, 0x41, 0x43, 0x48, 0x45,
  0x47, 0x02, // KLV_0601_ALTERNATE_PLATFORM_HEADING
  0x17, 0x2F,
  0x48, 0x08, // KLV_0601_EVENT_START_TIME
  0x00, 0x02, 0xD5, 0xD0, 0x24, 0x66, 0x01, 0x80,
  0x49, 0x00, // KLV_0601_RVT_LOCAL_SET
  0x4A, 0x00, // KLV_0601_VMTI_LOCAL_SET
  0x4B, 0x02, // KLV_0601_SENSOR_ELLIPSOID_HEIGHT
  0xC2, 0x21,
  0x4C, 0x02, // KLV_0601_ALTERNATE_PLATFORM_ELLIPSOID_HEIGHT
  0x0B, 0xB3,
  0x4D, 0x01, // KLV_0601_OPERATIONAL_MODE
  0x01,
  0x4E, 0x02, // KLV_0601_FRAME_CENTER_HEIGHT_ABOVE_ELLIPSOID
  0x0B, 0xB3,
  0x4F, 0x02, // KLV_0601_SENSOR_NORTH_VELOCITY
  0x09, 0xFB,
  0x50, 0x02, // KLV_0601_SENSOR_EAST_VELOCITY
  0x04, 0xBC,
  0x51, 0x04, // KLV_0601_IMAGE_HORIZON_PIXEL_PACK
  0x00, 0x24, 0x38, 0x00,
  0x52, 0x04, // KLV_0601_FULL_CORNER_LATITUDE_POINT_1
  0xF1, 0x06, 0x9B, 0x63,
  0x53, 0x04, // KLV_0601_FULL_CORNER_LONGITUDE_POINT_1
  0x14, 0xBC, 0xB2, 0xC0,
  0x54, 0x04, // KLV_0601_FULL_CORNER_LATITUDE_POINT_2
  0xF1, 0x00, 0x4D, 0x00,
  0x55, 0x04, // KLV_0601_FULL_CORNER_LONGITUDE_POINT_2
  0x14, 0xBE, 0x84, 0xC8,
  0x56, 0x04, // KLV_0601_FULL_CORNER_LATITUDE_POINT_3
  0xF0, 0xFD, 0x9B, 0x17,
  0x57, 0x04, // KLV_0601_FULL_CORNER_LONGITUDE_POINT_3
  0x14, 0xBB, 0x17, 0xAF,
  0x58, 0x04, // KLV_0601_FULL_CORNER_LATITUDE_POINT_4
  0xF1, 0x02, 0x05, 0x2A,
  0x59, 0x04, // KLV_0601_FULL_CORNER_LONGITUDE_POINT_4
  0x14, 0xB9, 0xD1, 0x76,
  0x5A, 0x04, // KLV_0601_FULL_PLATFORM_PITCH_ANGLE
  0xFF, 0x62, 0xE2, 0xF2,
  0x5B, 0x04, // KLV_0601_FULL_PLATFORM_ROLL_ANGLE
  0x04, 0xD8, 0x04, 0xDF,
  0x5C, 0x04, // KLV_0601_FULL_PLATFORM_ANGLE_OF_ATTACK
  0xF3, 0xAB, 0x48, 0xEF,
  0x5D, 0x04, // KLV_0601_FULL_PLATFORM_SIDESLIP_ANGLE
  0xDE, 0x17, 0x93, 0x23,
  0x5E, 0x22, // KLV_0601_MIIS_CORE_IDENTIFIER
  0x01, 0x70, 0xF5, 0x92, 0xF0, 0x23, 0x73, 0x36, 0x4A, 0xF8, 0xAA, 0x91,
  0x62, 0xC0, 0x0F, 0x2E, 0xB2, 0xDA, 0x16, 0xB7, 0x43, 0x41, 0x00, 0x08,
  0x41, 0xA0, 0xBE, 0x36, 0x5B, 0x5A, 0xB9, 0x6A, 0x36, 0x45,
  0x5F, 0x00, // KLV_0601_SAR_MOTION_IMAGERY_LOCAL_SET
  0x60, 0x03, // KLV_0601_TARGET_WIDTH_EXTENDED
  0x00, 0xD9, 0x2A,
  0x61, 0x00, // KLV_0601_RANGE_IMAGE_LOCAL_SET
  0x62, 0x00, // KLV_0601_GEOREGISTRATION_LOCAL_SET
  0x63, 0x00, // KLV_0601_COMPOSITE_IMAGING_LOCAL_SET
  0x64, 0x00, // KLV_0601_SEGMENT_LOCAL_SET
  0x65, 0x00, // KLV_0601_AMEND_LOCAL_SET
  0x66, 0x00, // KLV_0601_SDCC_FLP
  0x67, 0x03, // KLV_0601_DENSITY_ALTITUDE_EXTENDED
  0x2F, 0x92, 0x1E,
  0x68, 0x03, // KLV_0601_SENSOR_ELLIPSOID_HEIGHT_EXTENDED
  0x2F, 0x92, 0x1E,
  0x69, 0x03, // KLV_0601_ALTERNATE_PLATFORM_ELLIPSOID_HEIGHT_EXTENDED
  0x2F, 0x92, 0x1E,
  0x6A, 0x04, // KLV_0601_STREAM_DESIGNATOR
  0x42, 0x4C, 0x55, 0x45,
  0x6B, 0x06, // KLV_0601_OPERATIONAL_BASE
  0x42, 0x41, 0x53, 0x45, 0x30, 0x31,
  0x6C, 0x04, // KLV_0601_BROADCAST_SOURCE
  0x48, 0x4F, 0x4D, 0x45,
  0x6D, 0x03, // KLV_0601_RANGE_TO_RECOVERY_LOCATION
  0x00, 0x01, 0xA0,
  0x6E, 0x02, // KLV_0601_TIME_AIRBORNE
  0x4D, 0xAF,
  0x6F, 0x02, // KLV_0601_PROPULSION_UNIT_SPEED
  0x0B, 0xB8,
  0x70, 0x02, // KLV_0601_PLATFORM_COURSE_ANGLE
  0x1F, 0x40,
  0x71, 0x03, // KLV_0601_ALTITUDE_ABOVE_GROUND_LEVEL
  0x05, 0xF5, 0x00,
  0x72, 0x03, // KLV_0601_RADAR_ALTIMETER
  0x05, 0xF7, 0x40,
  0x73, 0x13, // KLV_0601_CONTROL_COMMAND
  0x05, 0x11, 0x46, 0x6C, 0x79, 0x20, 0x74, 0x6F, 0x20, 0x57, 0x61, 0x79,
  0x70, 0x6F, 0x69, 0x6E, 0x74, 0x20, 0x31,
  0x74, 0x02, // KLV_0601_CONTROL_COMMAND_VERIFICATION_LIST
  0x03, 0x07,
  0x75, 0x02, // KLV_0601_SENSOR_AZIMUTH_RATE
  0x3E, 0x90,
  0x76, 0x03, // KLV_0601_SENSOR_ELEVATION_RATE
  0x3E, 0x80, 0x11,
  0x77, 0x02, // KLV_0601_SENSOR_ROLL_RATE
  0x3B, 0x60,
  0x78, 0x02, // KLV_0601_ONBOARD_MI_STORAGE_PERCENT_FULL
  0x48, 0x00,
  0x79, 0x02, // KLV_0601_ACTIVE_WAVELENGTH_LIST
  0x01, 0x03,
  0x7A, 0x0B, // KLV_0601_COUNTRY_CODES
  0x01, 0x0E, 0x03, 0x43, 0x41, 0x4E, 0x00, 0x03, 0x46, 0x52, 0x41,
  0x7B, 0x01, // KLV_0601_NUMBER_OF_NAVSATS_IN_VIEW
  0x07,
  0x7C, 0x01, // KLV_0601_POSITIONING_METHOD_SOURCE
  0x03,
  0x7D, 0x01, // KLV_0601_PLATFORM_STATUS
  0x09,
  0x7E, 0x01, // KLV_0601_SENSOR_CONTROL_MODE
  0x05,
  0x7F, 0x05, // KLV_0601_SENSOR_FRAME_RATE_PACK
  0x83, 0xD4, 0x60, 0x87, 0x69,
  0x81, 0x00, 0x0E, // KLV_0601_WAVELENGTHS_LIST
  0x0D, 0x15, 0x00, 0x00, 0x07, 0xD0, 0x00, 0x00, 0x0F, 0xA0, 0x4E, 0x4E,
  0x49, 0x52,
  0x81, 0x01, 0x04, // KLV_0601_TARGET_ID
  0x41, 0x31, 0x32, 0x33,
  0x81, 0x02, 0x18, // KLV_0601_AIRBASE_LOCATIONS
  0x0B, 0x40, 0x6B, 0xC2, 0x09, 0x19, 0xBD, 0xA5, 0x54, 0x07, 0x0E, 0x00,
  0x0B, 0x40, 0x78, 0x3C, 0xB8, 0x19, 0xA2, 0x92, 0x74, 0x07, 0xC6, 0x00,
  0x81, 0x03, 0x07, // KLV_0601_TAKEOFF_TIME
  0x05, 0x6F, 0x27, 0x1B, 0x5E, 0x41, 0xB7,
  0x81, 0x04, 0x03, // KLV_0601_TRANSMISSION_FREQUENCY
  0x02, 0x57, 0xC0,
  0x81, 0x05, 0x02, // KLV_0601_ONBOARD_MI_STORAGE_CAPACITY
  0x27, 0x10,
  0x81, 0x06, 0x02, // KLV_0601_ZOOM_PERCENTAGE
  0x37, 0x00,
  0x81, 0x07, 0x14, // KLV_0601_COMMUNICATIONS_METHOD
  0x46, 0x72, 0x65, 0x71, 0x75, 0x65, 0x6E, 0x63, 0x79, 0x20, 0x4D, 0x6F,
  0x64, 0x75, 0x6C, 0x61, 0x74, 0x69, 0x6F, 0x6E,
  0x81, 0x08, 0x01, // KLV_0601_LEAP_SECONDS
  0x1E,
  0x81, 0x09, 0x05, // KLV_0601_CORRECTION_OFFSET
  0x01, 0x2B, 0x8D, 0xC6, 0x35,
  0x81, 0x0A, 0x3F, // KLV_0601_PAYLOAD_LIST
  0x03, 0x12, 0x00, 0x00, 0x0F, 0x56, 0x49, 0x53, 0x20, 0x4E, 0x6F, 0x73,
  0x65, 0x20, 0x43, 0x61, 0x6D, 0x65, 0x72, 0x61, 0x15, 0x01, 0x00, 0x12,
  0x41, 0x43, 0x4D, 0x45, 0x20, 0x56, 0x49, 0x53, 0x20, 0x4D, 0x6F, 0x64,
  0x65, 0x6C, 0x20, 0x31, 0x32, 0x33, 0x14, 0x02, 0x00, 0x11, 0x41, 0x43,
  0x4D, 0x45, 0x20, 0x49, 0x52, 0x20, 0x4D, 0x6F, 0x64, 0x65, 0x6C, 0x20,
  0x34, 0x35, 0x36,
  0x81, 0x0B, 0x01, // KLV_0601_ACTIVE_PAYLOADS
  0x0B,
  0x81, 0x0C, 0x2C, // KLV_0601_WEAPON_STORES
  0x0E, 0x01, 0x01, 0x01, 0x03, 0x82, 0x03, 0x07, 0x48, 0x61, 0x72, 0x70,
  0x6F, 0x6F, 0x6E, 0x0F, 0x01, 0x01, 0x02, 0x02, 0x9E, 0x04, 0x08, 0x48,
  0x65, 0x6C, 0x6C, 0x66, 0x69, 0x72, 0x65, 0x0C, 0x01, 0x02, 0x01, 0x01,
  0x03, 0x06, 0x47, 0x42, 0x55, 0x2D, 0x31, 0x35,
  0x81, 0x0D, 0x40, // KLV_0601_WAYPOINT_LIST
  0x0F, 0x00, 0x00, 0x01, 0x03, 0x40, 0x71, 0xD8, 0x94, 0x19, 0xBD, 0xBF,
  0xE7, 0x08, 0x98, 0x00, 0x0F, 0x01, 0x00, 0x02, 0x02, 0x40, 0x71, 0xD3,
  0x88, 0x19, 0xBC, 0xCE, 0x24, 0x08, 0xFC, 0x00, 0x0F, 0x02, 0x7F, 0xFF,
  0x01, 0x40, 0x71, 0xE3, 0x08, 0x19, 0xBF, 0x2C, 0x1B, 0x07, 0xD0, 0x00,
  0x0F, 0x03, 0xFF, 0xFE, 0x00, 0x40, 0x71, 0xE5, 0xAF, 0x19, 0xBF, 0x5A,
  0xA7, 0x09, 0x60, 0x00,
  0x81, 0x0E, 0x0E, // KLV_0601_VIEW_DOMAIN
  0x06, 0x34, 0x80, 0x00, 0x4B, 0x00, 0x00, 0x06, 0x1A, 0x40, 0x00, 0x0C,
  0x80, 0x00, };

// ----------------------------------------------------------------------------
TEST ( klv, read_write_0601 )
{
  CALL_TEST( test_read_write, {}, {} );
  CALL_TEST( test_read_write, expected_result, input_bytes );
}

// ----------------------------------------------------------------------------
TEST ( klv, read_write_0601_packet )
{
  auto const packet_header = klv_bytes_t{
    0x06, 0x0E, 0x2B, 0x34, 0x02, 0x0B, 0x01, 0x01,
    0x0E, 0x01, 0x03, 0x01, 0x01, 0x00, 0x00, 0x00,
    0x82, 0x03, 0xBB };
  auto const packet_footer = klv_bytes_t{ KLV_0601_CHECKSUM, 2, 0x58, 0x02 };

  // Assemble the target packet's serialized form
  auto packet_bytes =
    klv_bytes_t( packet_header.size() +
                 input_bytes.size() +
                 packet_footer.size() );
  auto bytes_it = packet_bytes.begin();
  bytes_it = std::copy( packet_header.cbegin(), packet_header.cend(),
                        bytes_it );
  bytes_it = std::copy( input_bytes.cbegin(), input_bytes.cend(),
                        bytes_it );
  bytes_it = std::copy( packet_footer.cbegin(), packet_footer.cend(),
                        bytes_it );

  // Assemble the target packet's unserialized form
  auto const test_packet = klv_packet{ klv_0601_key(), expected_result };

  // Deserialize
  auto read_it = packet_bytes.cbegin();
  auto const read_packet = klv_read_packet( read_it, packet_bytes.size() );
  EXPECT_EQ( packet_bytes.cend(), read_it );
  EXPECT_EQ( test_packet, read_packet );

  // Reserialize
  klv_bytes_t written_bytes( klv_packet_length( read_packet ) );
  auto write_it = written_bytes.begin();
  klv_write_packet( read_packet, write_it, written_bytes.size() );
  EXPECT_EQ( written_bytes.end(), write_it );
  EXPECT_EQ( packet_bytes, written_bytes );
}
