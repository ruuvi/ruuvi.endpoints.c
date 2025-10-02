#include "unity.h"

#include <string.h>
#include <stdint.h>
#include "ruuvi_endpoint_e1.h"

#define RE_E1_BLE_PACKET_HEADER 0x2B, 0xFF, 0x99, 0x04

void
setUp (void)
{
}

void
tearDown (void)
{
}

/**
 * @brief Typical encode operation
 *
 * @retval RD_SUCCESS on success test.
 */
void
test_ruuvi_endpoint_e1_get_ok (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = 29.5f,
        .humidity_rh       = 55.3f,
        .pressure_pa       = 101102.0f,
        .pm1p0_ppm         = 10.1f,
        .pm2p5_ppm         = 11.2f,
        .pm4p0_ppm         = 121.3f,
        .pm10p0_ppm        = 455.4f,
        .co2               = 201,
        .voc               = 10,
        .nox               = 2,
        .luminosity        = 13027,
        .sound_inst_dba    = 42.4f,
        .sound_avg_dba     = 47.6f,
        .sound_peak_spl_db = 80.4f,
        .seq_cnt           = 0xDECDEE,
        .flags             = flags,
        .address           = 0xCBB8334C884F,
    };
    static const uint8_t valid_data[] =
    {
        0xE1,                              // Data type
        0x17, 0x0C,                        // Temperature
        0x56, 0x68,                        // Humidity
        0xC7, 0x9E,                        // Pressure
        0x00, 0x65,                        // PM1.0
        0x00, 0x70,                        // PM2.5
        0x04, 0xBD,                        // PM4.0
        0x11, 0xCA,                        // PM10.0
        0x00, 0xC9,                        // CO2
        0x05,                              // VOX
        0x01,                              // NOX
        0x13, 0xE0, 0xAC,                  // Luminosity
        0x3D,                              // Sound inst
        0x4A,                              // Sound avg
        0x9C,                              // Sound peak
        0xDE, 0xCD, 0xEE,                  // Seq cnt
        0x00,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F // MAC address
    };
    _Static_assert (sizeof (valid_data) == RE_E1_DATA_LENGTH);
    uint8_t        raw_buf[RE_E1_RAW_BUF_SIZE] = { RE_E1_BLE_PACKET_HEADER };
    uint8_t * const p_payload                   = &raw_buf[RE_E1_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_e1_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_inst_dba * 10.0f),
                       lrintf (decoded_data.sound_inst_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt, decoded_data.seq_cnt);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.address, decoded_data.address);
}

void
test_ruuvi_endpoint_e1_get_zeroes (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = 0.0f,
        .humidity_rh       = 0.0f,
        .pressure_pa       = 50000.0f,
        .pm1p0_ppm         = 0.0f,
        .pm2p5_ppm         = 0.0f,
        .pm4p0_ppm         = 0.0f,
        .pm10p0_ppm        = 0.0f,
        .co2               = 0,
        .voc               = 0,
        .nox               = 0,
        .luminosity        = 0,
        .sound_inst_dba    = 18.0f,
        .sound_avg_dba     = 18.0f,
        .sound_peak_spl_db = 18.0f,
        .seq_cnt           = 0,
        .flags             = flags,
        .address           = 0xCBB8334C884F,
    };
    static const uint8_t valid_data[] =
    {
        0xE1,                              // Data type
        0x00, 0x00,                        // Temperature
        0x00, 0x00,                        // Humidity
        0x00, 0x00,                        // Pressure
        0x00, 0x00,                        // PM1.0
        0x00, 0x00,                        // PM2.5
        0x00, 0x00,                        // PM4.0
        0x00, 0x00,                        // PM10.0
        0x00, 0x00,                        // CO2
        0x00,                              // VOX
        0x00,                              // NOX
        0x00, 0x00, 0x00,                  // Luminosity
        0x00,                              // Sound inst
        0x00,                              // Sound avg
        0x00,                              // Sound peak
        0x00, 0x00, 0x00,                  // Seq cnt
        0x00,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F // MAC address
    };
    _Static_assert (sizeof (valid_data) == RE_E1_DATA_LENGTH);
    uint8_t        raw_buf[RE_E1_RAW_BUF_SIZE] = { RE_E1_BLE_PACKET_HEADER };
    uint8_t * const p_payload                   = &raw_buf[RE_E1_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_e1_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_inst_dba * 10.0f),
                       lrintf (decoded_data.sound_inst_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt, decoded_data.seq_cnt);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.address, decoded_data.address);
}

void
test_ruuvi_endpoint_e1_get_temperature (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = 25.0f,
        .humidity_rh       = 0.0f,
        .pressure_pa       = 50000.0f,
        .pm1p0_ppm         = 0.0f,
        .pm2p5_ppm         = 0.0f,
        .pm4p0_ppm         = 0.0f,
        .pm10p0_ppm        = 0.0f,
        .co2               = 0,
        .voc               = 0,
        .nox               = 0,
        .luminosity        = 0,
        .sound_inst_dba    = 18.0f,
        .sound_avg_dba     = 18.0f,
        .sound_peak_spl_db = 18.0f,
        .seq_cnt           = 0,
        .flags             = flags,
        .address           = 0xCBB8334C884F,
    };
    static const uint8_t valid_data[] =
    {
        0xE1,                              // Data type
        0x13, 0x88,                        // Temperature
        0x00, 0x00,                        // Humidity
        0x00, 0x00,                        // Pressure
        0x00, 0x00,                        // PM1.0
        0x00, 0x00,                        // PM2.5
        0x00, 0x00,                        // PM4.0
        0x00, 0x00,                        // PM10.0
        0x00, 0x00,                        // CO2
        0x00,                              // VOX
        0x00,                              // NOX
        0x00, 0x00, 0x00,                  // Luminosity
        0x00,                              // Sound inst
        0x00,                              // Sound avg
        0x00,                              // Sound peak
        0x00, 0x00, 0x00,                  // Seq cnt
        0x00,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F // MAC address
    };
    _Static_assert (sizeof (valid_data) == RE_E1_DATA_LENGTH);
    uint8_t        raw_buf[RE_E1_RAW_BUF_SIZE] = { RE_E1_BLE_PACKET_HEADER };
    uint8_t * const p_payload                   = &raw_buf[RE_E1_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_e1_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_inst_dba * 10.0f),
                       lrintf (decoded_data.sound_inst_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt, decoded_data.seq_cnt);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.address, decoded_data.address);
}

void
test_ruuvi_endpoint_e1_get_humidity (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = 0.0f,
        .humidity_rh       = 70.0f,
        .pressure_pa       = 50000.0f,
        .pm1p0_ppm         = 0.0f,
        .pm2p5_ppm         = 0.0f,
        .pm4p0_ppm         = 0.0f,
        .pm10p0_ppm        = 0.0f,
        .co2               = 0,
        .voc               = 0,
        .nox               = 0,
        .luminosity        = 0,
        .sound_inst_dba    = 18.0f,
        .sound_avg_dba     = 18.0f,
        .sound_peak_spl_db = 18.0f,
        .seq_cnt           = 0,
        .flags             = flags,
        .address           = 0xCBB8334C884F,
    };
    static const uint8_t valid_data[] =
    {
        0xE1,                              // Data type
        0x00, 0x00,                        // Temperature
        0x6D, 0x60,                        // Humidity
        0x00, 0x00,                        // Pressure
        0x00, 0x00,                        // PM1.0
        0x00, 0x00,                        // PM2.5
        0x00, 0x00,                        // PM4.0
        0x00, 0x00,                        // PM10.0
        0x00, 0x00,                        // CO2
        0x00,                              // VOX
        0x00,                              // NOX
        0x00, 0x00, 0x00,                  // Luminosity
        0x00,                              // Sound inst
        0x00,                              // Sound avg
        0x00,                              // Sound peak
        0x00, 0x00, 0x00,                  // Seq cnt
        0x00,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F // MAC address
    };
    _Static_assert (sizeof (valid_data) == RE_E1_DATA_LENGTH);
    uint8_t        raw_buf[RE_E1_RAW_BUF_SIZE] = { RE_E1_BLE_PACKET_HEADER };
    uint8_t * const p_payload                   = &raw_buf[RE_E1_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_e1_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_inst_dba * 10.0f),
                       lrintf (decoded_data.sound_inst_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt, decoded_data.seq_cnt);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.address, decoded_data.address);
}

void
test_ruuvi_endpoint_e1_get_pressure (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = 0.0f,
        .humidity_rh       = 0.0f,
        .pressure_pa       = 100000.0f,
        .pm1p0_ppm         = 0.0f,
        .pm2p5_ppm         = 0.0f,
        .pm4p0_ppm         = 0.0f,
        .pm10p0_ppm        = 0.0f,
        .co2               = 0,
        .voc               = 0,
        .nox               = 0,
        .luminosity        = 0,
        .sound_inst_dba    = 18.0f,
        .sound_avg_dba     = 18.0f,
        .sound_peak_spl_db = 18.0f,
        .seq_cnt           = 0,
        .flags             = flags,
        .address           = 0xCBB8334C884F,
    };
    static const uint8_t valid_data[] =
    {
        0xE1,                              // Data type
        0x00, 0x00,                        // Temperature
        0x00, 0x00,                        // Humidity
        0xC3, 0x50,                        // Pressure
        0x00, 0x00,                        // PM1.0
        0x00, 0x00,                        // PM2.5
        0x00, 0x00,                        // PM4.0
        0x00, 0x00,                        // PM10.0
        0x00, 0x00,                        // CO2
        0x00,                              // VOX
        0x00,                              // NOX
        0x00, 0x00, 0x00,                  // Luminosity
        0x00,                              // Sound inst
        0x00,                              // Sound avg
        0x00,                              // Sound peak
        0x00, 0x00, 0x00,                  // Seq cnt
        0x00,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F // MAC address
    };
    _Static_assert (sizeof (valid_data) == RE_E1_DATA_LENGTH);
    uint8_t        raw_buf[RE_E1_RAW_BUF_SIZE] = { RE_E1_BLE_PACKET_HEADER };
    uint8_t * const p_payload                   = &raw_buf[RE_E1_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_e1_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_inst_dba * 10.0f),
                       lrintf (decoded_data.sound_inst_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt, decoded_data.seq_cnt);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.address, decoded_data.address);
}

void
test_ruuvi_endpoint_e1_get_pm1m0 (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = 0.0f,
        .humidity_rh       = 0.0f,
        .pressure_pa       = 50000.0f,
        .pm1p0_ppm         = 700.0f,
        .pm2p5_ppm         = 0.0f,
        .pm4p0_ppm         = 0.0f,
        .pm10p0_ppm        = 0.0f,
        .co2               = 0,
        .voc               = 0,
        .nox               = 0,
        .luminosity        = 0,
        .sound_inst_dba    = 18.0f,
        .sound_avg_dba     = 18.0f,
        .sound_peak_spl_db = 18.0f,
        .seq_cnt           = 0,
        .flags             = flags,
        .address           = 0xCBB8334C884F,
    };
    static const uint8_t valid_data[] =
    {
        0xE1,                              // Data type
        0x00, 0x00,                        // Temperature
        0x00, 0x00,                        // Humidity
        0x00, 0x00,                        // Pressure
        0x1B, 0x58,                        // PM1.0
        0x00, 0x00,                        // PM2.5
        0x00, 0x00,                        // PM4.0
        0x00, 0x00,                        // PM10.0
        0x00, 0x00,                        // CO2
        0x00,                              // VOX
        0x00,                              // NOX
        0x00, 0x00, 0x00,                  // Luminosity
        0x00,                              // Sound inst
        0x00,                              // Sound avg
        0x00,                              // Sound peak
        0x00, 0x00, 0x00,                  // Seq cnt
        0x00,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F // MAC address
    };
    _Static_assert (sizeof (valid_data) == RE_E1_DATA_LENGTH);
    uint8_t        raw_buf[RE_E1_RAW_BUF_SIZE] = { RE_E1_BLE_PACKET_HEADER };
    uint8_t * const p_payload                   = &raw_buf[RE_E1_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_e1_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_inst_dba * 10.0f),
                       lrintf (decoded_data.sound_inst_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt, decoded_data.seq_cnt);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.address, decoded_data.address);
}

void
test_ruuvi_endpoint_e1_get_pm2m5 (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = 0.0f,
        .humidity_rh       = 0.0f,
        .pressure_pa       = 50000.0f,
        .pm1p0_ppm         = 0.0f,
        .pm2p5_ppm         = 700.0f,
        .pm4p0_ppm         = 0.0f,
        .pm10p0_ppm        = 0.0f,
        .co2               = 0,
        .voc               = 0,
        .nox               = 0,
        .luminosity        = 0,
        .sound_inst_dba    = 18.0f,
        .sound_avg_dba     = 18.0f,
        .sound_peak_spl_db = 18.0f,
        .seq_cnt           = 0,
        .flags             = flags,
        .address           = 0xCBB8334C884F,
    };
    static const uint8_t valid_data[] =
    {
        0xE1,                              // Data type
        0x00, 0x00,                        // Temperature
        0x00, 0x00,                        // Humidity
        0x00, 0x00,                        // Pressure
        0x00, 0x00,                        // PM1.0
        0x1B, 0x58,                        // PM2.5
        0x00, 0x00,                        // PM4.0
        0x00, 0x00,                        // PM10.0
        0x00, 0x00,                        // CO2
        0x00,                              // VOX
        0x00,                              // NOX
        0x00, 0x00, 0x00,                  // Luminosity
        0x00,                              // Sound inst
        0x00,                              // Sound avg
        0x00,                              // Sound peak
        0x00, 0x00, 0x00,                  // Seq cnt
        0x00,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F // MAC address
    };
    _Static_assert (sizeof (valid_data) == RE_E1_DATA_LENGTH);
    uint8_t        raw_buf[RE_E1_RAW_BUF_SIZE] = { RE_E1_BLE_PACKET_HEADER };
    uint8_t * const p_payload                   = &raw_buf[RE_E1_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_e1_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_inst_dba * 10.0f),
                       lrintf (decoded_data.sound_inst_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt, decoded_data.seq_cnt);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.address, decoded_data.address);
}

void
test_ruuvi_endpoint_e1_get_pm4m0 (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = 0.0f,
        .humidity_rh       = 0.0f,
        .pressure_pa       = 50000.0f,
        .pm1p0_ppm         = 0.0f,
        .pm2p5_ppm         = 0.0f,
        .pm4p0_ppm         = 700.0f,
        .pm10p0_ppm        = 0.0f,
        .co2               = 0,
        .voc               = 0,
        .nox               = 0,
        .luminosity        = 0,
        .sound_inst_dba    = 18.0f,
        .sound_avg_dba     = 18.0f,
        .sound_peak_spl_db = 18.0f,
        .seq_cnt           = 0,
        .flags             = flags,
        .address           = 0xCBB8334C884F,
    };
    static const uint8_t valid_data[] =
    {
        0xE1,                              // Data type
        0x00, 0x00,                        // Temperature
        0x00, 0x00,                        // Humidity
        0x00, 0x00,                        // Pressure
        0x00, 0x00,                        // PM1.0
        0x00, 0x00,                        // PM2.5
        0x1B, 0x58,                        // PM4.0
        0x00, 0x00,                        // PM10.0
        0x00, 0x00,                        // CO2
        0x00,                              // VOX
        0x00,                              // NOX
        0x00, 0x00, 0x00,                  // Luminosity
        0x00,                              // Sound inst
        0x00,                              // Sound avg
        0x00,                              // Sound peak
        0x00, 0x00, 0x00,                  // Seq cnt
        0x00,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F // MAC address
    };
    _Static_assert (sizeof (valid_data) == RE_E1_DATA_LENGTH);
    uint8_t        raw_buf[RE_E1_RAW_BUF_SIZE] = { RE_E1_BLE_PACKET_HEADER };
    uint8_t * const p_payload                   = &raw_buf[RE_E1_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_e1_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_inst_dba * 10.0f),
                       lrintf (decoded_data.sound_inst_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt, decoded_data.seq_cnt);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.address, decoded_data.address);
}

void
test_ruuvi_endpoint_e1_get_pm10m0 (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = 0.0f,
        .humidity_rh       = 0.0f,
        .pressure_pa       = 50000.0f,
        .pm1p0_ppm         = 0.0f,
        .pm2p5_ppm         = 0.0f,
        .pm4p0_ppm         = 0.0f,
        .pm10p0_ppm        = 700.0f,
        .co2               = 0,
        .voc               = 0,
        .nox               = 0,
        .luminosity        = 0,
        .sound_inst_dba    = 18.0f,
        .sound_avg_dba     = 18.0f,
        .sound_peak_spl_db = 18.0f,
        .seq_cnt           = 0,
        .flags             = flags,
        .address           = 0xCBB8334C884F,
    };
    static const uint8_t valid_data[] =
    {
        0xE1,                              // Data type
        0x00, 0x00,                        // Temperature
        0x00, 0x00,                        // Humidity
        0x00, 0x00,                        // Pressure
        0x00, 0x00,                        // PM1.0
        0x00, 0x00,                        // PM2.5
        0x00, 0x00,                        // PM4.0
        0x1B, 0x58,                        // PM10.0
        0x00, 0x00,                        // CO2
        0x00,                              // VOX
        0x00,                              // NOX
        0x00, 0x00, 0x00,                  // Luminosity
        0x00,                              // Sound inst
        0x00,                              // Sound avg
        0x00,                              // Sound peak
        0x00, 0x00, 0x00,                  // Seq cnt
        0x00,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F // MAC address
    };
    _Static_assert (sizeof (valid_data) == RE_E1_DATA_LENGTH);
    uint8_t        raw_buf[RE_E1_RAW_BUF_SIZE] = { RE_E1_BLE_PACKET_HEADER };
    uint8_t * const p_payload                   = &raw_buf[RE_E1_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_e1_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_inst_dba * 10.0f),
                       lrintf (decoded_data.sound_inst_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt, decoded_data.seq_cnt);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.address, decoded_data.address);
}

void
test_ruuvi_endpoint_e1_get_co2 (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = 0.0f,
        .humidity_rh       = 0.0f,
        .pressure_pa       = 50000.0f,
        .pm1p0_ppm         = 0.0f,
        .pm2p5_ppm         = 0.0f,
        .pm4p0_ppm         = 0.0f,
        .pm10p0_ppm        = 0.0f,
        .co2               = 35000,
        .voc               = 0,
        .nox               = 0,
        .luminosity        = 0,
        .sound_inst_dba    = 18.0f,
        .sound_avg_dba     = 18.0f,
        .sound_peak_spl_db = 18.0f,
        .seq_cnt           = 0,
        .flags             = flags,
        .address           = 0xCBB8334C884F,
    };
    static const uint8_t valid_data[] =
    {
        0xE1,                              // Data type
        0x00, 0x00,                        // Temperature
        0x00, 0x00,                        // Humidity
        0x00, 0x00,                        // Pressure
        0x00, 0x00,                        // PM1.0
        0x00, 0x00,                        // PM2.5
        0x00, 0x00,                        // PM4.0
        0x00, 0x00,                        // PM10.0
        0x88, 0xB8,                        // CO2
        0x00,                              // VOX
        0x00,                              // NOX
        0x00, 0x00, 0x00,                  // Luminosity
        0x00,                              // Sound inst
        0x00,                              // Sound avg
        0x00,                              // Sound peak
        0x00, 0x00, 0x00,                  // Seq cnt
        0x00,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F // MAC address
    };
    _Static_assert (sizeof (valid_data) == RE_E1_DATA_LENGTH);
    uint8_t        raw_buf[RE_E1_RAW_BUF_SIZE] = { RE_E1_BLE_PACKET_HEADER };
    uint8_t * const p_payload                   = &raw_buf[RE_E1_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_e1_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_inst_dba * 10.0f),
                       lrintf (decoded_data.sound_inst_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt, decoded_data.seq_cnt);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.address, decoded_data.address);
}

void
test_ruuvi_endpoint_e1_get_voc (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = 0.0f,
        .humidity_rh       = 0.0f,
        .pressure_pa       = 50000.0f,
        .pm1p0_ppm         = 0.0f,
        .pm2p5_ppm         = 0.0f,
        .pm4p0_ppm         = 0.0f,
        .pm10p0_ppm        = 0.0f,
        .co2               = 0,
        .voc               = 499,
        .nox               = 0,
        .luminosity        = 0,
        .sound_inst_dba    = 18.0f,
        .sound_avg_dba     = 18.0f,
        .sound_peak_spl_db = 18.0f,
        .seq_cnt           = 0,
        .flags             = flags,
        .address           = 0xCBB8334C884F,
    };
    static const uint8_t valid_data[] =
    {
        0xE1,                              // Data type
        0x00, 0x00,                        // Temperature
        0x00, 0x00,                        // Humidity
        0x00, 0x00,                        // Pressure
        0x00, 0x00,                        // PM1.0
        0x00, 0x00,                        // PM2.5
        0x00, 0x00,                        // PM4.0
        0x00, 0x00,                        // PM10.0
        0x00, 0x00,                        // CO2
        0xF9,                              // VOX
        0x00,                              // NOX
        0x00, 0x00, 0x00,                  // Luminosity
        0x00,                              // Sound inst
        0x00,                              // Sound avg
        0x00,                              // Sound peak
        0x00, 0x00, 0x00,                  // Seq cnt
        0x40,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F // MAC address
    };
    _Static_assert (sizeof (valid_data) == RE_E1_DATA_LENGTH);
    uint8_t        raw_buf[RE_E1_RAW_BUF_SIZE] = { RE_E1_BLE_PACKET_HEADER };
    uint8_t * const p_payload                   = &raw_buf[RE_E1_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_e1_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_inst_dba * 10.0f),
                       lrintf (decoded_data.sound_inst_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt, decoded_data.seq_cnt);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.address, decoded_data.address);
}

void
test_ruuvi_endpoint_e1_get_nox (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = 0.0f,
        .humidity_rh       = 0.0f,
        .pressure_pa       = 50000.0f,
        .pm1p0_ppm         = 0.0f,
        .pm2p5_ppm         = 0.0f,
        .pm4p0_ppm         = 0.0f,
        .pm10p0_ppm        = 0.0f,
        .co2               = 0,
        .voc               = 0,
        .nox               = 497,
        .luminosity        = 0,
        .sound_inst_dba    = 18.0f,
        .sound_avg_dba     = 18.0f,
        .sound_peak_spl_db = 18.0f,
        .seq_cnt           = 0,
        .flags             = flags,
        .address           = 0xCBB8334C884F,
    };
    static const uint8_t valid_data[] =
    {
        0xE1,                              // Data type
        0x00, 0x00,                        // Temperature
        0x00, 0x00,                        // Humidity
        0x00, 0x00,                        // Pressure
        0x00, 0x00,                        // PM1.0
        0x00, 0x00,                        // PM2.5
        0x00, 0x00,                        // PM4.0
        0x00, 0x00,                        // PM10.0
        0x00, 0x00,                        // CO2
        0x00,                              // VOX
        0xF8,                              // NOX
        0x00, 0x00, 0x00,                  // Luminosity
        0x00,                              // Sound inst
        0x00,                              // Sound avg
        0x00,                              // Sound peak
        0x00, 0x00, 0x00,                  // Seq cnt
        0x80,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F // MAC address
    };
    _Static_assert (sizeof (valid_data) == RE_E1_DATA_LENGTH);
    uint8_t        raw_buf[RE_E1_RAW_BUF_SIZE] = { RE_E1_BLE_PACKET_HEADER };
    uint8_t * const p_payload                   = &raw_buf[RE_E1_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_e1_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_inst_dba * 10.0f),
                       lrintf (decoded_data.sound_inst_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt, decoded_data.seq_cnt);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.address, decoded_data.address);
}

void
test_ruuvi_endpoint_e1_get_luminosity (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = 0.0f,
        .humidity_rh       = 0.0f,
        .pressure_pa       = 50000.0f,
        .pm1p0_ppm         = 0.0f,
        .pm2p5_ppm         = 0.0f,
        .pm4p0_ppm         = 0.0f,
        .pm10p0_ppm        = 0.0f,
        .co2               = 0,
        .voc               = 0,
        .nox               = 0,
        .luminosity        = 144000.02f,
        .sound_inst_dba    = 18.0f,
        .sound_avg_dba     = 18.0f,
        .sound_peak_spl_db = 18.0f,
        .seq_cnt           = 0,
        .flags             = flags,
        .address           = 0xCBB8334C884F,
    };
    static const uint8_t valid_data[] =
    {
        0xE1,                              // Data type
        0x00, 0x00,                        // Temperature
        0x00, 0x00,                        // Humidity
        0x00, 0x00,                        // Pressure
        0x00, 0x00,                        // PM1.0
        0x00, 0x00,                        // PM2.5
        0x00, 0x00,                        // PM4.0
        0x00, 0x00,                        // PM10.0
        0x00, 0x00,                        // CO2
        0x00,                              // VOX
        0x00,                              // NOX
        0xDB, 0xBA, 0x02,                  // Luminosity
        0x00,                              // Sound inst
        0x00,                              // Sound avg
        0x00,                              // Sound peak
        0x00, 0x00, 0x00,                  // Seq cnt
        0x00,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F // MAC address
    };
    _Static_assert (sizeof (valid_data) == RE_E1_DATA_LENGTH);
    uint8_t        raw_buf[RE_E1_RAW_BUF_SIZE] = { RE_E1_BLE_PACKET_HEADER };
    uint8_t * const p_payload                   = &raw_buf[RE_E1_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_e1_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_inst_dba * 10.0f),
                       lrintf (decoded_data.sound_inst_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt, decoded_data.seq_cnt);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.address, decoded_data.address);
}

void
test_ruuvi_endpoint_e1_get_sound_inst_dba (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = 0.0f,
        .humidity_rh       = 0.0f,
        .pressure_pa       = 50000.0f,
        .pm1p0_ppm         = 0.0f,
        .pm2p5_ppm         = 0.0f,
        .pm4p0_ppm         = 0.0f,
        .pm10p0_ppm        = 0.0f,
        .co2               = 0,
        .voc               = 0,
        .nox               = 0,
        .luminosity        = 0,
        .sound_inst_dba    = 101.0f,
        .sound_avg_dba     = 18.0f,
        .sound_peak_spl_db = 18.0f,
        .seq_cnt           = 0,
        .flags             = flags,
        .address           = 0xCBB8334C884F,
    };
    static const uint8_t valid_data[] =
    {
        0xE1,                              // Data type
        0x00, 0x00,                        // Temperature
        0x00, 0x00,                        // Humidity
        0x00, 0x00,                        // Pressure
        0x00, 0x00,                        // PM1.0
        0x00, 0x00,                        // PM2.5
        0x00, 0x00,                        // PM4.0
        0x00, 0x00,                        // PM10.0
        0x00, 0x00,                        // CO2
        0x00,                              // VOX
        0x00,                              // NOX
        0x00, 0x00, 0x00,                  // Luminosity
        0xCF,                              // Sound inst
        0x00,                              // Sound avg
        0x00,                              // Sound peak
        0x00, 0x00, 0x00,                  // Seq cnt
        0x08,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F // MAC address
    };
    _Static_assert (sizeof (valid_data) == RE_E1_DATA_LENGTH);
    uint8_t        raw_buf[RE_E1_RAW_BUF_SIZE] = { RE_E1_BLE_PACKET_HEADER };
    uint8_t * const p_payload                   = &raw_buf[RE_E1_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_e1_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_inst_dba * 10.0f),
                       lrintf (decoded_data.sound_inst_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt, decoded_data.seq_cnt);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.address, decoded_data.address);
}

void
test_ruuvi_endpoint_e1_get_sound_avg_dba (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = 0.0f,
        .humidity_rh       = 0.0f,
        .pressure_pa       = 50000.0f,
        .pm1p0_ppm         = 0.0f,
        .pm2p5_ppm         = 0.0f,
        .pm4p0_ppm         = 0.0f,
        .pm10p0_ppm        = 0.0f,
        .co2               = 0,
        .voc               = 0,
        .nox               = 0,
        .luminosity        = 0,
        .sound_inst_dba    = 18.0f,
        .sound_avg_dba     = 101.0f,
        .sound_peak_spl_db = 18.0f,
        .seq_cnt           = 0,
        .flags             = flags,
        .address           = 0xCBB8334C884F,
    };
    static const uint8_t valid_data[] =
    {
        0xE1,                              // Data type
        0x00, 0x00,                        // Temperature
        0x00, 0x00,                        // Humidity
        0x00, 0x00,                        // Pressure
        0x00, 0x00,                        // PM1.0
        0x00, 0x00,                        // PM2.5
        0x00, 0x00,                        // PM4.0
        0x00, 0x00,                        // PM10.0
        0x00, 0x00,                        // CO2
        0x00,                              // VOX
        0x00,                              // NOX
        0x00, 0x00, 0x00,                  // Luminosity
        0x00,                              // Sound inst
        0xCF,                              // Sound avg
        0x00,                              // Sound peak
        0x00, 0x00, 0x00,                  // Seq cnt
        0x10,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F // MAC address
    };
    _Static_assert (sizeof (valid_data) == RE_E1_DATA_LENGTH);
    uint8_t        raw_buf[RE_E1_RAW_BUF_SIZE] = { RE_E1_BLE_PACKET_HEADER };
    uint8_t * const p_payload                   = &raw_buf[RE_E1_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_e1_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_inst_dba * 10.0f),
                       lrintf (decoded_data.sound_inst_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt, decoded_data.seq_cnt);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.address, decoded_data.address);
}

void
test_ruuvi_endpoint_e1_get_sound_peak_spl_db (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = 0.0f,
        .humidity_rh       = 0.0f,
        .pressure_pa       = 50000.0f,
        .pm1p0_ppm         = 0.0f,
        .pm2p5_ppm         = 0.0f,
        .pm4p0_ppm         = 0.0f,
        .pm10p0_ppm        = 0.0f,
        .co2               = 0,
        .voc               = 0,
        .nox               = 0,
        .luminosity        = 0,
        .sound_inst_dba    = 18.0f,
        .sound_avg_dba     = 18.0f,
        .sound_peak_spl_db = 101.0f,
        .seq_cnt           = 0,
        .flags             = flags,
        .address           = 0xCBB8334C884F,
    };
    static const uint8_t valid_data[] =
    {
        0xE1,                              // Data type
        0x00, 0x00,                        // Temperature
        0x00, 0x00,                        // Humidity
        0x00, 0x00,                        // Pressure
        0x00, 0x00,                        // PM1.0
        0x00, 0x00,                        // PM2.5
        0x00, 0x00,                        // PM4.0
        0x00, 0x00,                        // PM10.0
        0x00, 0x00,                        // CO2
        0x00,                              // VOX
        0x00,                              // NOX
        0x00, 0x00, 0x00,                  // Luminosity
        0x00,                              // Sound inst
        0x00,                              // Sound avg
        0xCF,                              // Sound peak
        0x00, 0x00, 0x00,                  // Seq cnt
        0x20,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F // MAC address
    };
    _Static_assert (sizeof (valid_data) == RE_E1_DATA_LENGTH);
    uint8_t        raw_buf[RE_E1_RAW_BUF_SIZE] = { RE_E1_BLE_PACKET_HEADER };
    uint8_t * const p_payload                   = &raw_buf[RE_E1_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_e1_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_inst_dba * 10.0f),
                       lrintf (decoded_data.sound_inst_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt, decoded_data.seq_cnt);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.address, decoded_data.address);
}

void
test_ruuvi_endpoint_e1_get_seq_cnt (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = 0.0f,
        .humidity_rh       = 0.0f,
        .pressure_pa       = 50000.0f,
        .pm1p0_ppm         = 0.0f,
        .pm2p5_ppm         = 0.0f,
        .pm4p0_ppm         = 0.0f,
        .pm10p0_ppm        = 0.0f,
        .co2               = 0,
        .voc               = 0,
        .nox               = 0,
        .luminosity        = 0,
        .sound_inst_dba    = 18.0f,
        .sound_avg_dba     = 18.0f,
        .sound_peak_spl_db = 18.0f,
        .seq_cnt           = 0xABCDEF,
        .flags             = flags,
        .address           = 0xCBB8334C884F,
    };
    static const uint8_t valid_data[] =
    {
        0xE1,                              // Data type
        0x00, 0x00,                        // Temperature
        0x00, 0x00,                        // Humidity
        0x00, 0x00,                        // Pressure
        0x00, 0x00,                        // PM1.0
        0x00, 0x00,                        // PM2.5
        0x00, 0x00,                        // PM4.0
        0x00, 0x00,                        // PM10.0
        0x00, 0x00,                        // CO2
        0x00,                              // VOX
        0x00,                              // NOX
        0x00, 0x00, 0x00,                  // Luminosity
        0x00,                              // Sound inst
        0x00,                              // Sound avg
        0x00,                              // Sound peak
        0xAB, 0xCD, 0xEF,                  // Seq cnt
        0x00,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F // MAC address
    };
    _Static_assert (sizeof (valid_data) == RE_E1_DATA_LENGTH);
    uint8_t        raw_buf[RE_E1_RAW_BUF_SIZE] = { RE_E1_BLE_PACKET_HEADER };
    uint8_t * const p_payload                   = &raw_buf[RE_E1_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_e1_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_inst_dba * 10.0f),
                       lrintf (decoded_data.sound_inst_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt, decoded_data.seq_cnt);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.address, decoded_data.address);
}

void
test_ruuvi_endpoint_e1_get_flag_calibration_in_progress (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = true,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = 0.0f,
        .humidity_rh       = 0.0f,
        .pressure_pa       = 50000.0f,
        .pm1p0_ppm         = 0.0f,
        .pm2p5_ppm         = 0.0f,
        .pm4p0_ppm         = 0.0f,
        .pm10p0_ppm        = 0.0f,
        .co2               = 0,
        .voc               = 0,
        .nox               = 0,
        .luminosity        = 0,
        .sound_inst_dba    = 18.0f,
        .sound_avg_dba     = 18.0f,
        .sound_peak_spl_db = 18.0f,
        .seq_cnt           = 0,
        .flags             = flags,
        .address           = 0xCBB8334C884F,
    };
    static const uint8_t valid_data[] =
    {
        0xE1,                              // Data type
        0x00, 0x00,                        // Temperature
        0x00, 0x00,                        // Humidity
        0x00, 0x00,                        // Pressure
        0x00, 0x00,                        // PM1.0
        0x00, 0x00,                        // PM2.5
        0x00, 0x00,                        // PM4.0
        0x00, 0x00,                        // PM10.0
        0x00, 0x00,                        // CO2
        0x00,                              // VOX
        0x00,                              // NOX
        0x00, 0x00, 0x00,                  // Luminosity
        0x00,                              // Sound inst
        0x00,                              // Sound avg
        0x00,                              // Sound peak
        0x00, 0x00, 0x00,                  // Seq cnt
        0x01,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F // MAC address
    };
    _Static_assert (sizeof (valid_data) == RE_E1_DATA_LENGTH);
    uint8_t        raw_buf[RE_E1_RAW_BUF_SIZE] = { RE_E1_BLE_PACKET_HEADER };
    uint8_t * const p_payload                   = &raw_buf[RE_E1_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_e1_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_inst_dba * 10.0f),
                       lrintf (decoded_data.sound_inst_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt, decoded_data.seq_cnt);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.address, decoded_data.address);
}

void
test_ruuvi_endpoint_e1_get_flag_button_pressed (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = true,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = 0.0f,
        .humidity_rh       = 0.0f,
        .pressure_pa       = 50000.0f,
        .pm1p0_ppm         = 0.0f,
        .pm2p5_ppm         = 0.0f,
        .pm4p0_ppm         = 0.0f,
        .pm10p0_ppm        = 0.0f,
        .co2               = 0,
        .voc               = 0,
        .nox               = 0,
        .luminosity        = 0,
        .sound_inst_dba    = 18.0f,
        .sound_avg_dba     = 18.0f,
        .sound_peak_spl_db = 18.0f,
        .seq_cnt           = 0,
        .flags             = flags,
        .address           = 0xCBB8334C884F,
    };
    static const uint8_t valid_data[] =
    {
        0xE1,                              // Data type
        0x00, 0x00,                        // Temperature
        0x00, 0x00,                        // Humidity
        0x00, 0x00,                        // Pressure
        0x00, 0x00,                        // PM1.0
        0x00, 0x00,                        // PM2.5
        0x00, 0x00,                        // PM4.0
        0x00, 0x00,                        // PM10.0
        0x00, 0x00,                        // CO2
        0x00,                              // VOX
        0x00,                              // NOX
        0x00, 0x00, 0x00,                  // Luminosity
        0x00,                              // Sound inst
        0x00,                              // Sound avg
        0x00,                              // Sound peak
        0x00, 0x00, 0x00,                  // Seq cnt
        0x02,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F // MAC address
    };
    _Static_assert (sizeof (valid_data) == RE_E1_DATA_LENGTH);
    uint8_t        raw_buf[RE_E1_RAW_BUF_SIZE] = { RE_E1_BLE_PACKET_HEADER };
    uint8_t * const p_payload                   = &raw_buf[RE_E1_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_e1_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_inst_dba * 10.0f),
                       lrintf (decoded_data.sound_inst_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt, decoded_data.seq_cnt);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.address, decoded_data.address);
}

void
test_ruuvi_endpoint_e1_get_flag_rtc_running_on_boot (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = true,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = 0.0f,
        .humidity_rh       = 0.0f,
        .pressure_pa       = 50000.0f,
        .pm1p0_ppm         = 0.0f,
        .pm2p5_ppm         = 0.0f,
        .pm4p0_ppm         = 0.0f,
        .pm10p0_ppm        = 0.0f,
        .co2               = 0,
        .voc               = 0,
        .nox               = 0,
        .luminosity        = 0,
        .sound_inst_dba    = 18.0f,
        .sound_avg_dba     = 18.0f,
        .sound_peak_spl_db = 18.0f,
        .seq_cnt           = 0,
        .flags             = flags,
        .address           = 0xCBB8334C884F,
    };
    static const uint8_t valid_data[] =
    {
        0xE1,                              // Data type
        0x00, 0x00,                        // Temperature
        0x00, 0x00,                        // Humidity
        0x00, 0x00,                        // Pressure
        0x00, 0x00,                        // PM1.0
        0x00, 0x00,                        // PM2.5
        0x00, 0x00,                        // PM4.0
        0x00, 0x00,                        // PM10.0
        0x00, 0x00,                        // CO2
        0x00,                              // VOX
        0x00,                              // NOX
        0x00, 0x00, 0x00,                  // Luminosity
        0x00,                              // Sound inst
        0x00,                              // Sound avg
        0x00,                              // Sound peak
        0x00, 0x00, 0x00,                  // Seq cnt
        0x04,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F // MAC address
    };
    _Static_assert (sizeof (valid_data) == RE_E1_DATA_LENGTH);
    uint8_t        raw_buf[RE_E1_RAW_BUF_SIZE] = { RE_E1_BLE_PACKET_HEADER };
    uint8_t * const p_payload                   = &raw_buf[RE_E1_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_e1_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_inst_dba * 10.0f),
                       lrintf (decoded_data.sound_inst_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt, decoded_data.seq_cnt);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.address, decoded_data.address);
}

void
test_ruuvi_endpoint_e1_get_ok_max (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = true,
        .flag_button_pressed          = true,
        .flag_rtc_running_on_boot     = true,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = 163.835f,
        .humidity_rh       = 100.0f,
        .pressure_pa       = 115534.0f,
        .pm1p0_ppm         = 1000.0f,
        .pm2p5_ppm         = 1000.0f,
        .pm4p0_ppm         = 1000.0f,
        .pm10p0_ppm        = 1000.0f,
        .co2               = 40000,
        .voc               = 500,
        .nox               = 500,
        .luminosity        = 144284.00f,
        .sound_inst_dba    = 120,
        .sound_avg_dba     = 120,
        .sound_peak_spl_db = 120,
        .seq_cnt           = 0xFFFFFE,
        .flags             = flags,
        .address           = 0xFFFFFFFFFFFF,
    };
    static const uint8_t max_data[] =
    {
        0xE1,                              // Data type
        0x7F, 0xFF,                        // Temperature
        0x9C, 0x40,                        // Humidity
        0xFF, 0xFE,                        // Pressure
        0x27, 0x10,                        // PM1.0
        0x27, 0x10,                        // PM2.5
        0x27, 0x10,                        // PM4.0
        0x27, 0x10,                        // PM10.0
        0x9C, 0x40,                        // CO2
        0xFA,                              // VOC
        0xFA,                              // NOX
        0xDC, 0x28, 0xF0,                  // Luminosity
        0xFF,                              // Sound inst
        0xFF,                              // Sound avg
        0xFF,                              // Sound peak
        0xFF, 0xFF, 0xFE,                  // Seq cnt
        0x07,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF // MAC address
    };
    _Static_assert (sizeof (max_data) == RE_E1_DATA_LENGTH);
    uint8_t        raw_buf[RE_E1_RAW_BUF_SIZE] = { RE_E1_BLE_PACKET_HEADER };
    uint8_t * const p_payload                   = &raw_buf[RE_E1_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (max_data, p_payload, sizeof (max_data));
    re_e1_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_inst_dba * 10.0f),
                       lrintf (decoded_data.sound_inst_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt, decoded_data.seq_cnt);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.address, decoded_data.address);
}

void
test_ruuvi_endpoint_e1_get_ok_min (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = -163.835f,
        .humidity_rh       = 0.0f,
        .pressure_pa       = 50000.0f,
        .pm1p0_ppm         = 0.0f,
        .pm2p5_ppm         = 0.0f,
        .pm4p0_ppm         = 0.0f,
        .pm10p0_ppm        = 0.0f,
        .co2               = 0,
        .voc               = 0,
        .nox               = 0,
        .luminosity        = 0,
        .sound_inst_dba    = 18,
        .sound_avg_dba     = 18,
        .sound_peak_spl_db = 18,
        .seq_cnt           = 0,
        .flags             = flags,
        .address           = 0xCBB8334C884F,
    };
    static const uint8_t min_data[] =
    {
        0xE1,                              // Data type
        0x80, 0x01,                        // Temperature
        0x00, 0x00,                        // Humidity
        0x00, 0x00,                        // Pressure
        0x00, 0x00,                        // PM1.0
        0x00, 0x00,                        // PM2.5
        0x00, 0x00,                        // PM4.0
        0x00, 0x00,                        // PM10.0
        0x00, 0x00,                        // CO2
        0x00,                              // VOC
        0x00,                              // NOX
        0x00, 0x00, 0x00,                  // Luminosity
        0U,                                // Sound inst
        0U,                                // Sound avg
        0U,                                // Sound peak
        0x00, 0x00, 0x00,                  // Seq cnt
        0x00,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F // MAC address
    };
    _Static_assert (sizeof (min_data) == RE_E1_DATA_LENGTH);
    uint8_t        raw_buf[RE_E1_RAW_BUF_SIZE] = { RE_E1_BLE_PACKET_HEADER };
    uint8_t * const p_payload                   = &raw_buf[RE_E1_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (min_data, p_payload, sizeof (min_data));
    re_e1_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_inst_dba * 10.0f),
                       lrintf (decoded_data.sound_inst_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt, decoded_data.seq_cnt);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.address, decoded_data.address);
}

/**
 * @brief Null buffer
 *
 * @retval RE_ERROR_NULL on success test.
 */
void
test_ruuvi_endpoint_e1_get_error_null_buffer (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = 29.5f,
        .humidity_rh       = 55.3f,
        .pressure_pa       = 101102.0f,
        .pm1p0_ppm         = 10.1f,
        .pm2p5_ppm         = 11.2f,
        .pm4p0_ppm         = 121.3f,
        .pm10p0_ppm        = 455.4f,
        .co2               = 201,
        .voc               = 10,
        .nox               = 2,
        .luminosity        = 13027,
        .sound_inst_dba    = 42.4f,
        .sound_avg_dba     = 47.6f,
        .sound_peak_spl_db = 80.4f,
        .seq_cnt           = 0xDECDEE,
        .flags             = flags,
        .address           = 0xCBB8334C884F,
    };
    uint8_t * const p_test_buffer = NULL;
    TEST_ASSERT_EQUAL (RE_ERROR_NULL, re_e1_encode (p_test_buffer, &data));
}

/**
 * @brief Null data
 *
 * @retval RE_ERROR_NULL on success test.
 */
void
test_ruuvi_endpoint_e1_get_error_null_data (void)
{
    uint8_t             test_buffer[RE_E1_DATA_LENGTH] = { 0 };
    const re_e1_data_t * p_re_e1_data                   = NULL;
    TEST_ASSERT_EQUAL (RE_ERROR_NULL, re_e1_encode ( (uint8_t * const) &test_buffer,
                       p_re_e1_data));
}

/**
 * @brief True to test encode operation with invalid data
 *
 * @retval RD_SUCCESS on success test.
 */
void
test_ruuvi_endpoint_e1_get_invalid_data (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = NAN,
        .humidity_rh       = NAN,
        .pressure_pa       = NAN,
        .pm1p0_ppm         = NAN,
        .pm2p5_ppm         = NAN,
        .pm4p0_ppm         = NAN,
        .pm10p0_ppm        = NAN,
        .co2               = NAN,
        .voc               = NAN,
        .nox               = NAN,
        .luminosity        = NAN,
        .sound_inst_dba    = NAN,
        .sound_avg_dba     = NAN,
        .sound_peak_spl_db = NAN,
        .seq_cnt           = RE_E1_INVALID_SEQUENCE,
        .flags             = flags,
        .address           = 0xFFFFFFFFFFFF,
    };
    static const uint8_t invalid_data[] =
    {
        0xE1,                              // Data type
        0x80, 0x00,                        // Temperature
        0xFF, 0xFF,                        // Humidity
        0xFF, 0xFF,                        // Pressure
        0xFF, 0xFF,                        // PM1.0
        0xFF, 0xFF,                        // PM2.5
        0xFF, 0xFF,                        // PM4.0
        0xFF, 0xFF,                        // PM10.0
        0xFF, 0xFF,                        // CO2
        0xFF,                              // VOX
        0xFF,                              // NOX
        0xFF, 0xFF, 0xFF,                  // Luminosity
        0xFF,                              // Sound inst
        0xFF,                              // Sound avg
        0xFF,                              // Sound peak
        0xFF, 0xFF, 0xFF,                  // Seq cnt
        0xF8,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF // MAC address
    };
    _Static_assert (sizeof (invalid_data) == RE_E1_DATA_LENGTH);
    re_status_t err_code        = RE_SUCCESS;
    uint8_t     test_buffer[40] = { 0 };
    err_code                    = re_e1_encode ( (uint8_t * const) &test_buffer, &data);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT_EQUAL_HEX8_ARRAY (invalid_data, test_buffer, sizeof (invalid_data));
    const uint8_t raw_buf_prefix[] = { 0x2B, 0xFF, 0x99, 0x04 };
    _Static_assert (sizeof (raw_buf_prefix) == RE_E1_OFFSET_PAYLOAD,
                    "Invalid raw_buf_prefix size");
    uint8_t raw_buf[48];
    memcpy (&raw_buf[0], raw_buf_prefix, sizeof (raw_buf_prefix));
    memcpy (&raw_buf[RE_E1_OFFSET_PAYLOAD], test_buffer, sizeof (test_buffer));
    re_e1_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_decode (raw_buf, &decoded_data));
    TEST_ASSERT (isnan (decoded_data.temperature_c));
    TEST_ASSERT (isnan (decoded_data.humidity_rh));
    TEST_ASSERT (isnan (decoded_data.pressure_pa));
    TEST_ASSERT (isnan (decoded_data.pm1p0_ppm));
    TEST_ASSERT (isnan (decoded_data.pm1p0_ppm));
    TEST_ASSERT (isnan (decoded_data.pm1p0_ppm));
    TEST_ASSERT (isnan (decoded_data.pm1p0_ppm));
    TEST_ASSERT (isnan (decoded_data.co2));
    TEST_ASSERT (isnan (decoded_data.voc));
    TEST_ASSERT (isnan (decoded_data.nox));
    TEST_ASSERT (isnan (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_inst_dba * 10.0f),
                       lrintf (decoded_data.sound_inst_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt, decoded_data.seq_cnt);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.address, decoded_data.address);
}

void
test_ruuvi_endpoint_e1_underflow (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_e1_data_t m_re_fe_data_underflow =
    {
        .temperature_c     = -164,
        .humidity_rh       = -1,
        .pressure_pa       = 49999,
        .pm1p0_ppm         = -1,
        .pm2p5_ppm         = -1,
        .pm4p0_ppm         = -1,
        .pm10p0_ppm        = -1,
        .co2               = -1,
        .voc               = -1,
        .nox               = -1,
        .luminosity        = -1,
        .sound_inst_dba    = -1,
        .sound_avg_dba     = -1,
        .sound_peak_spl_db = -1,
        .seq_cnt           = 0,
        .flags             = flags,
        .address           = 0x000000000000,
    };
    static const uint8_t min_data[] =
    {
        0xE1,                              // Data type
        0x80, 0x01,                        // Temperature
        0x00, 0x00,                        // Humidity
        0x00, 0x00,                        // Pressure
        0x00, 0x00,                        // PM1.0
        0x00, 0x00,                        // PM2.5
        0x00, 0x00,                        // PM4.0
        0x00, 0x00,                        // PM10.0
        0x00, 0x00,                        // CO2
        0x00,                              // VOC
        0x00,                              // NOX
        0x00, 0x00, 0x00,                  // Luminosity
        0U,                                // Sound inst
        0U,                                // Sound avg
        0U,                                // Sound peak
        0x00, 0x00, 0x00,                  // Seq cnt
        0x00,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // MAC address
    };
    _Static_assert (sizeof (min_data) == RE_E1_DATA_LENGTH);
    uint8_t test_buffer[RE_E1_DATA_LENGTH] = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_encode (test_buffer, &m_re_fe_data_underflow));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (min_data, test_buffer, sizeof (min_data));
}

void
test_ruuvi_endpoint_e1_overflow (void)
{
    static const re_e1_flags_t flags =
    {
        .flag_calibration_in_progress = true,
        .flag_button_pressed          = true,
        .flag_rtc_running_on_boot     = true,
    };
    static const re_e1_data_t data =
    {
        .temperature_c     = 163.9f,
        .humidity_rh       = 100.1f,
        .pressure_pa       = 115535.0f,
        .pm1p0_ppm         = 1000.1,
        .pm2p5_ppm         = 1000.1,
        .pm4p0_ppm         = 1000.1,
        .pm10p0_ppm        = 1000.1,
        .co2               = 40001,
        .voc               = 501,
        .nox               = 501,
        .luminosity        = 144285.00f,
        .sound_inst_dba    = 130,
        .sound_avg_dba     = 130,
        .sound_peak_spl_db = 130,
        .seq_cnt           = 0xFFFFFF,
        .flags             = flags,
        .address           = 0xFFFFFFFFFFFF,
    };
    static const uint8_t max_data[] =
    {
        0xE1,                              // Data type
        0x7F, 0xFF,                        // Temperature
        0x9C, 0x40,                        // Humidity
        0xFF, 0xFE,                        // Pressure
        0x27, 0x10,                        // PM1.0
        0x27, 0x10,                        // PM2.5
        0x27, 0x10,                        // PM4.0
        0x27, 0x10,                        // PM10.0
        0x9C, 0x40,                        // CO2
        0xFA,                              // VOC
        0xFA,                              // NOX
        0xDC, 0x28, 0xF0,                  // Luminosity
        0xFF,                              // Sound inst
        0xFF,                              // Sound avg
        0xFF,                              // Sound peak
        0xFF, 0xFF, 0xFF,                  // Seq cnt
        0x07,                              // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,      // Reserved
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF // MAC address
    };
    _Static_assert (sizeof (max_data) == RE_E1_DATA_LENGTH);
    uint8_t test_buffer[RE_E1_DATA_LENGTH] = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e1_encode (test_buffer, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (max_data, test_buffer, sizeof (max_data));
}

void
test_ruuvi_endpoint_e1_check_format_ok (void)
{
    uint8_t raw_buf[RE_E1_RAW_BUF_SIZE] = { RE_E1_BLE_PACKET_HEADER, 0xE1 };
    TEST_ASSERT_TRUE (re_e1_check_format (raw_buf));
}

void
test_ruuvi_endpoint_e1_check_format_fail (void)
{
    const uint8_t raw_buf_payload_format_E1[48] = { 0x02, 0x01, 0x04, 0x1B, 0xFF, 0x99, 0x04, 0xE1 };
    TEST_ASSERT_FALSE (re_e1_check_format (raw_buf_payload_format_E1));
    const uint8_t raw_buf_manufacturer_id_1[48] = { 0x02, 0x01, 0x04, 0x1B, 0xFF, 0x99, 0x05, 0xE0 };
    TEST_ASSERT_FALSE (re_e1_check_format (raw_buf_manufacturer_id_1));
    const uint8_t raw_buf_manufacturer_id_2[48] = { 0x02, 0x01, 0x04, 0x1B, 0xFF, 0x9a, 0x04, 0xE0 };
    TEST_ASSERT_FALSE (re_e1_check_format (raw_buf_manufacturer_id_2));
    const uint8_t raw_buf_type[48] = { 0x02, 0x01, 0x04, 0x1B, 0xFE, 0x99, 0x04, 0xE0 };
    TEST_ASSERT_FALSE (re_e1_check_format (raw_buf_type));
    const uint8_t raw_buf_len[48] = { 0x02, 0x01, 0x04, 0x1A, 0xFF, 0x99, 0x04, 0xE0 };
    TEST_ASSERT_FALSE (re_e1_check_format (raw_buf_len));
    const uint8_t raw_buf_byte1[48] = { 0x02, 0x02, 0x04, 0x1B, 0xFF, 0x99, 0x04, 0xE0 };
    TEST_ASSERT_FALSE (re_e1_check_format (raw_buf_byte1));
    const uint8_t raw_buf_byte0[48] = { 0x03, 0x01, 0x04, 0x1B, 0xFF, 0x99, 0x04, 0xE0 };
    TEST_ASSERT_FALSE (re_e1_check_format (raw_buf_byte0));
}

void
test_ruuvi_endpoint_e1_data_invalid (void)
{
    const re_e1_seq_cnt_t  measurement_cnt = 0x123456;
    const re_e1_mac_addr_t radio_mac       = 0xCBB8334C884FULL;
    const re_e1_data_t     data            = re_e1_data_invalid (measurement_cnt, radio_mac);
    TEST_ASSERT (isnan (data.temperature_c));
    TEST_ASSERT (isnan (data.humidity_rh));
    TEST_ASSERT (isnan (data.pressure_pa));
    TEST_ASSERT (isnan (data.pm1p0_ppm));
    TEST_ASSERT (isnan (data.pm1p0_ppm));
    TEST_ASSERT (isnan (data.pm1p0_ppm));
    TEST_ASSERT (isnan (data.pm1p0_ppm));
    TEST_ASSERT (isnan (data.co2));
    TEST_ASSERT (isnan (data.voc));
    TEST_ASSERT (isnan (data.nox));
    TEST_ASSERT (isnan (data.luminosity));
    TEST_ASSERT (isnan (data.sound_inst_dba));
    TEST_ASSERT (isnan (data.sound_avg_dba));
    TEST_ASSERT (isnan (data.sound_peak_spl_db));
    TEST_ASSERT_EQUAL (data.seq_cnt, measurement_cnt);
    TEST_ASSERT_EQUAL (false, data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (false, data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (false, data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.address, radio_mac);
}
