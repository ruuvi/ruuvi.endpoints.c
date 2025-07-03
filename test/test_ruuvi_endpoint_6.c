#include "unity.h"

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "ruuvi_endpoint_6.h"

#define RE_6_BLE_PACKET_HEADER 0x02, 0x01, 0x06, 0x03, 0x03, 0x98, 0xFC, 0x17, 0xFF, 0x99, 0x04

#define RE_6_DATA_MAC_ADDR_INIT(mac_byte3, mac_byte4, mac_byte5) \
    { \
        .byte3 = mac_byte3, \
        .byte4 = mac_byte4, \
        .byte5 = mac_byte5, \
    }

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
test_ruuvi_endpoint_6_get_ok (void)
{
    static const re_6_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_6_data_t data =
    {
        .temperature_c = 29.5f,
        .humidity_rh   = 55.3f,
        .pressure_pa   = 101102.0f,
        .pm2p5_ppm     = 11.2f,
        .co2           = 201,
        .voc           = 10,
        .nox           = 2,
        .luminosity    = 13027,
        .sound_avg_dba = 47.6f,
        .seq_cnt2      = 205,
        .flags         = flags,
        .mac_addr_24   = RE_6_DATA_MAC_ADDR_INIT (0x4C, 0x88, 0x4F),
    };
    static const uint8_t valid_data[] =
    {
        0x06,       // Data type
        0x17, 0x0C, // Temperature
        0x56, 0x68, // Humidity
        0xC7, 0x9E, // Pressure
        0x00, 0x70, // PM2.5
        0x00, 0xC9, // CO2
        0x05,       // VOC
        0x01,       // NOX
        0xD9,       // Luminosity
        0x4A,       // Sound dBA avg
        0xCD,       // Seq cnt2
        0x00,       // Flags
        0x4C,       // MAC address byte 3
        0x88,       // MAC address byte 4
        0x4F        // MAC address byte 5
    };
    _Static_assert (sizeof (valid_data) == RE_6_DATA_LENGTH);
    uint8_t        raw_buf[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER };
    uint8_t * const p_payload                  = &raw_buf[RE_6_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_6_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa), lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt2, decoded_data.seq_cnt2);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte3, decoded_data.mac_addr_24.byte3);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte4, decoded_data.mac_addr_24.byte4);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte5, decoded_data.mac_addr_24.byte5);
}

void
test_ruuvi_endpoint_6_get_zeroes (void)
{
    static const re_6_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_6_data_t data =
    {
        .temperature_c = 0.0f,
        .humidity_rh   = 0.0f,
        .pressure_pa   = 50000.0f,
        .pm2p5_ppm     = 0.0f,
        .co2           = 0,
        .voc           = 0,
        .nox           = 0,
        .luminosity    = 0,
        .sound_avg_dba = 18.0f,
        .seq_cnt2      = 0,
        .flags         = flags,
        .mac_addr_24   = RE_6_DATA_MAC_ADDR_INIT (0x4C, 0x88, 0x4F),
    };
    static const uint8_t valid_data[] =
    {
        0x06,       // Data type
        0x00, 0x00, // Temperature
        0x00, 0x00, // Humidity
        0x00, 0x00, // Pressure
        0x00, 0x00, // PM2.5
        0x00, 0x00, // CO2
        0x00,       // VOC
        0x00,       // NOX
        0x00,       // Luminosity
        0x00,       // Sound dBA avg
        0x00,       // Seq cnt2
        0x00,       // Flags
        0x4C,       // MAC address byte 3
        0x88,       // MAC address byte 4
        0x4F        // MAC address byte 5
    };
    _Static_assert (sizeof (valid_data) == RE_6_DATA_LENGTH);
    uint8_t        raw_buf[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER };
    uint8_t * const p_payload                  = &raw_buf[RE_6_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_6_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa), lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt2, decoded_data.seq_cnt2);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte3, decoded_data.mac_addr_24.byte3);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte4, decoded_data.mac_addr_24.byte4);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte5, decoded_data.mac_addr_24.byte5);
}

void
test_ruuvi_endpoint_6_get_temperature (void)
{
    static const re_6_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_6_data_t data =
    {
        .temperature_c = 25.0f,
        .humidity_rh   = 0.0f,
        .pressure_pa   = 50000.0f,
        .pm2p5_ppm     = 0.0f,
        .co2           = 0,
        .voc           = 0,
        .nox           = 0,
        .luminosity    = 0,
        .sound_avg_dba = 18.0f,
        .seq_cnt2      = 0,
        .flags         = flags,
        .mac_addr_24   = RE_6_DATA_MAC_ADDR_INIT (0x4C, 0x88, 0x4F),
    };
    static const uint8_t valid_data[] =
    {
        0x06,       // Data type
        0x13, 0x88, // Temperature
        0x00, 0x00, // Humidity
        0x00, 0x00, // Pressure
        0x00, 0x00, // PM2.5
        0x00, 0x00, // CO2
        0x00,       // VOC
        0x00,       // NOX
        0x00,       // Luminosity
        0x00,       // Sound dBA avg
        0x00,       // Seq cnt2
        0x00,       // Flags
        0x4C,       // MAC address byte 3
        0x88,       // MAC address byte 4
        0x4F        // MAC address byte 5
    };
    _Static_assert (sizeof (valid_data) == RE_6_DATA_LENGTH);
    uint8_t        raw_buf[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER };
    uint8_t * const p_payload                  = &raw_buf[RE_6_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_6_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa), lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt2, decoded_data.seq_cnt2);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte3, decoded_data.mac_addr_24.byte3);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte4, decoded_data.mac_addr_24.byte4);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte5, decoded_data.mac_addr_24.byte5);
}

void
test_ruuvi_endpoint_6_get_humidity (void)
{
    static const re_6_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_6_data_t data =
    {
        .temperature_c = 0.0f,
        .humidity_rh   = 70.0f,
        .pressure_pa   = 50000.0f,
        .pm2p5_ppm     = 0.0f,
        .co2           = 0,
        .voc           = 0,
        .nox           = 0,
        .luminosity    = 0,
        .sound_avg_dba = 18.0f,
        .seq_cnt2      = 0,
        .flags         = flags,
        .mac_addr_24   = RE_6_DATA_MAC_ADDR_INIT (0x4C, 0x88, 0x4F),
    };
    static const uint8_t valid_data[] =
    {
        0x06,       // Data type
        0x00, 0x00, // Temperature
        0x6D, 0x60, // Humidity
        0x00, 0x00, // Pressure
        0x00, 0x00, // PM2.5
        0x00, 0x00, // CO2
        0x00,       // VOC
        0x00,       // NOX
        0x00,       // Luminosity
        0x00,       // Sound dBA avg
        0x00,       // Seq cnt2
        0x00,       // Flags
        0x4C,       // MAC address byte 3
        0x88,       // MAC address byte 4
        0x4F        // MAC address byte 5
    };
    _Static_assert (sizeof (valid_data) == RE_6_DATA_LENGTH);
    uint8_t        raw_buf[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER };
    uint8_t * const p_payload                  = &raw_buf[RE_6_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_6_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa), lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt2, decoded_data.seq_cnt2);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte3, decoded_data.mac_addr_24.byte3);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte4, decoded_data.mac_addr_24.byte4);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte5, decoded_data.mac_addr_24.byte5);
}

void
test_ruuvi_endpoint_6_get_pressure (void)
{
    static const re_6_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_6_data_t data =
    {
        .temperature_c = 0.0f,
        .humidity_rh   = 0.0f,
        .pressure_pa   = 100000.0f,
        .pm2p5_ppm     = 0.0f,
        .co2           = 0,
        .voc           = 0,
        .nox           = 0,
        .luminosity    = 0,
        .sound_avg_dba = 18.0f,
        .seq_cnt2      = 0,
        .flags         = flags,
        .mac_addr_24   = RE_6_DATA_MAC_ADDR_INIT (0x4C, 0x88, 0x4F),
    };
    static const uint8_t valid_data[] =
    {
        0x06,       // Data type
        0x00, 0x00, // Temperature
        0x00, 0x00, // Humidity
        0xC3, 0x50, // Pressure
        0x00, 0x00, // PM2.5
        0x00, 0x00, // CO2
        0x00,       // VOC
        0x00,       // NOX
        0x00,       // Luminosity
        0x00,       // Sound dBA avg
        0x00,       // Seq cnt2
        0x00,       // Flags
        0x4C,       // MAC address byte 3
        0x88,       // MAC address byte 4
        0x4F        // MAC address byte 5
    };
    _Static_assert (sizeof (valid_data) == RE_6_DATA_LENGTH);
    uint8_t        raw_buf[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER };
    uint8_t * const p_payload                  = &raw_buf[RE_6_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_6_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa), lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt2, decoded_data.seq_cnt2);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte3, decoded_data.mac_addr_24.byte3);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte4, decoded_data.mac_addr_24.byte4);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte5, decoded_data.mac_addr_24.byte5);
}

void
test_ruuvi_endpoint_6_get_pm2p5 (void)
{
    static const re_6_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_6_data_t data =
    {
        .temperature_c = 0.0f,
        .humidity_rh   = 0.0f,
        .pressure_pa   = 50000.0f,
        .pm2p5_ppm     = 700.0f,
        .co2           = 0,
        .voc           = 0,
        .nox           = 0,
        .luminosity    = 0,
        .sound_avg_dba = 18.0f,
        .seq_cnt2      = 0,
        .flags         = flags,
        .mac_addr_24   = RE_6_DATA_MAC_ADDR_INIT (0x4C, 0x88, 0x4F),
    };
    static const uint8_t valid_data[] =
    {
        0x06,       // Data type
        0x00, 0x00, // Temperature
        0x00, 0x00, // Humidity
        0x00, 0x00, // Pressure
        0x1B, 0x58, // PM2.5
        0x00, 0x00, // CO2
        0x00,       // VOC
        0x00,       // NOX
        0x00,       // Luminosity
        0x00,       // Sound dBA avg
        0x00,       // Seq cnt2
        0x00,       // Flags
        0x4C,       // MAC address byte 3
        0x88,       // MAC address byte 4
        0x4F        // MAC address byte 5
    };
    _Static_assert (sizeof (valid_data) == RE_6_DATA_LENGTH);
    uint8_t        raw_buf[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER };
    uint8_t * const p_payload                  = &raw_buf[RE_6_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_6_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa), lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt2, decoded_data.seq_cnt2);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte3, decoded_data.mac_addr_24.byte3);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte4, decoded_data.mac_addr_24.byte4);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte5, decoded_data.mac_addr_24.byte5);
}

void
test_ruuvi_endpoint_6_get_co2 (void)
{
    static const re_6_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_6_data_t data =
    {
        .temperature_c = 0.0f,
        .humidity_rh   = 0.0f,
        .pressure_pa   = 50000.0f,
        .pm2p5_ppm     = 0.0f,
        .co2           = 35000,
        .voc           = 0,
        .nox           = 0,
        .luminosity    = 0,
        .sound_avg_dba = 18.0f,
        .seq_cnt2      = 0,
        .flags         = flags,
        .mac_addr_24   = RE_6_DATA_MAC_ADDR_INIT (0x4C, 0x88, 0x4F),
    };
    static const uint8_t valid_data[] =
    {
        0x06,       // Data type
        0x00, 0x00, // Temperature
        0x00, 0x00, // Humidity
        0x00, 0x00, // Pressure
        0x00, 0x00, // PM2.5
        0x88, 0xB8, // CO2
        0x00,       // VOC
        0x00,       // NOX
        0x00,       // Luminosity
        0x00,       // Sound dBA avg
        0x00,       // Seq cnt2
        0x00,       // Flags
        0x4C,       // MAC address byte 3
        0x88,       // MAC address byte 4
        0x4F        // MAC address byte 5
    };
    _Static_assert (sizeof (valid_data) == RE_6_DATA_LENGTH);
    uint8_t        raw_buf[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER };
    uint8_t * const p_payload                  = &raw_buf[RE_6_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_6_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa), lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt2, decoded_data.seq_cnt2);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte3, decoded_data.mac_addr_24.byte3);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte4, decoded_data.mac_addr_24.byte4);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte5, decoded_data.mac_addr_24.byte5);
}

void
test_ruuvi_endpoint_6_get_voc (void)
{
    static const re_6_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_6_data_t data =
    {
        .temperature_c = 0.0f,
        .humidity_rh   = 0.0f,
        .pressure_pa   = 50000.0f,
        .pm2p5_ppm     = 0.0f,
        .co2           = 0,
        .voc           = 499,
        .nox           = 0,
        .luminosity    = 0,
        .sound_avg_dba = 18.0f,
        .seq_cnt2      = 0,
        .flags         = flags,
        .mac_addr_24   = RE_6_DATA_MAC_ADDR_INIT (0x4C, 0x88, 0x4F),
    };
    static const uint8_t valid_data[] =
    {
        0x06,       // Data type
        0x00, 0x00, // Temperature
        0x00, 0x00, // Humidity
        0x00, 0x00, // Pressure
        0x00, 0x00, // PM2.5
        0x00, 0x00, // CO2
        0xF9,       // VOC
        0x00,       // NOX
        0x00,       // Luminosity
        0x00,       // Sound dBA avg
        0x00,       // Seq cnt2
        0x40,       // Flags
        0x4C,       // MAC address byte 3
        0x88,       // MAC address byte 4
        0x4F        // MAC address byte 5
    };
    _Static_assert (sizeof (valid_data) == RE_6_DATA_LENGTH);
    uint8_t        raw_buf[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER };
    uint8_t * const p_payload                  = &raw_buf[RE_6_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_6_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa), lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt2, decoded_data.seq_cnt2);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte3, decoded_data.mac_addr_24.byte3);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte4, decoded_data.mac_addr_24.byte4);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte5, decoded_data.mac_addr_24.byte5);
}

void
test_ruuvi_endpoint_6_get_nox (void)
{
    static const re_6_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_6_data_t data =
    {
        .temperature_c = 0.0f,
        .humidity_rh   = 0.0f,
        .pressure_pa   = 50000.0f,
        .pm2p5_ppm     = 0.0f,
        .co2           = 0,
        .voc           = 0,
        .nox           = 497,
        .luminosity    = 0,
        .sound_avg_dba = 18.0f,
        .seq_cnt2      = 0,
        .flags         = flags,
        .mac_addr_24   = RE_6_DATA_MAC_ADDR_INIT (0x4C, 0x88, 0x4F),
    };
    static const uint8_t valid_data[] =
    {
        0x06,       // Data type
        0x00, 0x00, // Temperature
        0x00, 0x00, // Humidity
        0x00, 0x00, // Pressure
        0x00, 0x00, // PM2.5
        0x00, 0x00, // CO2
        0x00,       // VOC
        0xF8,       // NOX
        0x00,       // Luminosity
        0x00,       // Sound dBA avg
        0x00,       // Seq cnt2
        0x80,       // Flags
        0x4C,       // MAC address byte 3
        0x88,       // MAC address byte 4
        0x4F        // MAC address byte 5
    };
    _Static_assert (sizeof (valid_data) == RE_6_DATA_LENGTH);
    uint8_t        raw_buf[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER };
    uint8_t * const p_payload                  = &raw_buf[RE_6_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_6_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa), lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt2, decoded_data.seq_cnt2);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte3, decoded_data.mac_addr_24.byte3);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte4, decoded_data.mac_addr_24.byte4);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte5, decoded_data.mac_addr_24.byte5);
}

void
test_ruuvi_endpoint_6_get_luminosity (void)
{
    static const re_6_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_6_data_t data =
    {
        .temperature_c = 0.0f,
        .humidity_rh   = 0.0f,
        .pressure_pa   = 50000.0f,
        .pm2p5_ppm     = 0.0f,
        .co2           = 0,
        .voc           = 0,
        .nox           = 0,
        .luminosity    = 62735.0f,
        .sound_avg_dba = 18.0f,
        .seq_cnt2      = 0,
        .flags         = flags,
        .mac_addr_24   = RE_6_DATA_MAC_ADDR_INIT (0x4C, 0x88, 0x4F),
    };
    static const uint8_t valid_data[] =
    {
        0x06,       // Data type
        0x00, 0x00, // Temperature
        0x00, 0x00, // Humidity
        0x00, 0x00, // Pressure
        0x00, 0x00, // PM2.5
        0x00, 0x00, // CO2
        0x00,       // VOC
        0x00,       // NOX
        0xFD,       // Luminosity
        0x00,       // Sound dBA avg
        0x00,       // Seq cnt2
        0x00,       // Flags
        0x4C,       // MAC address byte 3
        0x88,       // MAC address byte 4
        0x4F        // MAC address byte 5
    };
    _Static_assert (sizeof (valid_data) == RE_6_DATA_LENGTH);
    uint8_t        raw_buf[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER };
    uint8_t * const p_payload                  = &raw_buf[RE_6_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_6_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa), lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt2, decoded_data.seq_cnt2);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte3, decoded_data.mac_addr_24.byte3);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte4, decoded_data.mac_addr_24.byte4);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte5, decoded_data.mac_addr_24.byte5);
}

void
test_ruuvi_endpoint_6_get_sound_avg_dba (void)
{
    static const re_6_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_6_data_t data =
    {
        .temperature_c = 0.0f,
        .humidity_rh   = 0.0f,
        .pressure_pa   = 50000.0f,
        .pm2p5_ppm     = 0.0f,
        .co2           = 0,
        .voc           = 0,
        .nox           = 0,
        .luminosity    = 0,
        .sound_avg_dba = 101.0f,
        .seq_cnt2      = 0,
        .flags         = flags,
        .mac_addr_24   = RE_6_DATA_MAC_ADDR_INIT (0x4C, 0x88, 0x4F),
    };
    static const uint8_t valid_data[] =
    {
        0x06,       // Data type
        0x00, 0x00, // Temperature
        0x00, 0x00, // Humidity
        0x00, 0x00, // Pressure
        0x00, 0x00, // PM2.5
        0x00, 0x00, // CO2
        0x00,       // VOC
        0x00,       // NOX
        0x00,       // Luminosity
        0xCF,       // Sound dBA avg
        0x00,       // Seq cnt2
        0x10,       // Flags
        0x4C,       // MAC address byte 3
        0x88,       // MAC address byte 4
        0x4F        // MAC address byte 5
    };
    _Static_assert (sizeof (valid_data) == RE_6_DATA_LENGTH);
    uint8_t        raw_buf[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER };
    uint8_t * const p_payload                  = &raw_buf[RE_6_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_6_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa), lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt2, decoded_data.seq_cnt2);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte3, decoded_data.mac_addr_24.byte3);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte4, decoded_data.mac_addr_24.byte4);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte5, decoded_data.mac_addr_24.byte5);
}

void
test_ruuvi_endpoint_6_get_seq_cnt (void)
{
    static const re_6_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_6_data_t data =
    {
        .temperature_c = 0.0f,
        .humidity_rh   = 0.0f,
        .pressure_pa   = 50000.0f,
        .pm2p5_ppm     = 0.0f,
        .co2           = 0,
        .voc           = 0,
        .nox           = 0,
        .luminosity    = 0,
        .sound_avg_dba = 18.0f,
        .seq_cnt2      = 250,
        .flags         = flags,
        .mac_addr_24   = RE_6_DATA_MAC_ADDR_INIT (0x4C, 0x88, 0x4F),
    };
    static const uint8_t valid_data[] =
    {
        0x06,       // Data type
        0x00, 0x00, // Temperature
        0x00, 0x00, // Humidity
        0x00, 0x00, // Pressure
        0x00, 0x00, // PM2.5
        0x00, 0x00, // CO2
        0x00,       // VOC
        0x00,       // NOX
        0x00,       // Luminosity
        0x00,       // Sound dBA avg
        0xFA,       // Seq cnt2
        0x00,       // Flags
        0x4C,       // MAC address byte 3
        0x88,       // MAC address byte 4
        0x4F        // MAC address byte 5
    };
    _Static_assert (sizeof (valid_data) == RE_6_DATA_LENGTH);
    uint8_t        raw_buf[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER };
    uint8_t * const p_payload                  = &raw_buf[RE_6_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_6_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa), lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt2, decoded_data.seq_cnt2);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte3, decoded_data.mac_addr_24.byte3);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte4, decoded_data.mac_addr_24.byte4);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte5, decoded_data.mac_addr_24.byte5);
}

void
test_ruuvi_endpoint_6_get_flag_calibration_in_progress (void)
{
    static const re_6_flags_t flags =
    {
        .flag_calibration_in_progress = true,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_6_data_t data =
    {
        .temperature_c = 0.0f,
        .humidity_rh   = 0.0f,
        .pressure_pa   = 50000.0f,
        .pm2p5_ppm     = 0.0f,
        .co2           = 0,
        .voc           = 0,
        .nox           = 0,
        .luminosity    = 0,
        .sound_avg_dba = 18.0f,
        .seq_cnt2      = 0,
        .flags         = flags,
        .mac_addr_24   = RE_6_DATA_MAC_ADDR_INIT (0x4C, 0x88, 0x4F),
    };
    static const uint8_t valid_data[] =
    {
        0x06,       // Data type
        0x00, 0x00, // Temperature
        0x00, 0x00, // Humidity
        0x00, 0x00, // Pressure
        0x00, 0x00, // PM2.5
        0x00, 0x00, // CO2
        0x00,       // VOC
        0x00,       // NOX
        0x00,       // Luminosity
        0x00,       // Sound dBA avg
        0x00,       // Seq cnt2
        0x01,       // Flags
        0x4C,       // MAC address byte 3
        0x88,       // MAC address byte 4
        0x4F        // MAC address byte 5
    };
    _Static_assert (sizeof (valid_data) == RE_6_DATA_LENGTH);
    uint8_t        raw_buf[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER };
    uint8_t * const p_payload                  = &raw_buf[RE_6_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_6_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa), lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt2, decoded_data.seq_cnt2);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte3, decoded_data.mac_addr_24.byte3);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte4, decoded_data.mac_addr_24.byte4);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte5, decoded_data.mac_addr_24.byte5);
}

void
test_ruuvi_endpoint_6_get_flag_button_pressed (void)
{
    static const re_6_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = true,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_6_data_t data =
    {
        .temperature_c = 0.0f,
        .humidity_rh   = 0.0f,
        .pressure_pa   = 50000.0f,
        .pm2p5_ppm     = 0.0f,
        .co2           = 0,
        .voc           = 0,
        .nox           = 0,
        .luminosity    = 0,
        .sound_avg_dba = 18.0f,
        .seq_cnt2      = 0,
        .flags         = flags,
        .mac_addr_24   = RE_6_DATA_MAC_ADDR_INIT (0x4C, 0x88, 0x4F),
    };
    static const uint8_t valid_data[] =
    {
        0x06,       // Data type
        0x00, 0x00, // Temperature
        0x00, 0x00, // Humidity
        0x00, 0x00, // Pressure
        0x00, 0x00, // PM2.5
        0x00, 0x00, // CO2
        0x00,       // VOC
        0x00,       // NOX
        0x00,       // Luminosity
        0x00,       // Sound dBA avg
        0x00,       // Seq cnt2
        0x02,       // Flags
        0x4C,       // MAC address byte 3
        0x88,       // MAC address byte 4
        0x4F        // MAC address byte 5
    };
    _Static_assert (sizeof (valid_data) == RE_6_DATA_LENGTH);
    uint8_t        raw_buf[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER };
    uint8_t * const p_payload                  = &raw_buf[RE_6_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_6_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa), lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt2, decoded_data.seq_cnt2);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte3, decoded_data.mac_addr_24.byte3);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte4, decoded_data.mac_addr_24.byte4);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte5, decoded_data.mac_addr_24.byte5);
}

void
test_ruuvi_endpoint_6_get_flag_rtc_running_on_boot (void)
{
    static const re_6_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = true,
    };
    static const re_6_data_t data =
    {
        .temperature_c = 0.0f,
        .humidity_rh   = 0.0f,
        .pressure_pa   = 50000.0f,
        .pm2p5_ppm     = 0.0f,
        .co2           = 0,
        .voc           = 0,
        .nox           = 0,
        .luminosity    = 0,
        .sound_avg_dba = 18.0f,
        .seq_cnt2      = 0,
        .flags         = flags,
        .mac_addr_24   = RE_6_DATA_MAC_ADDR_INIT (0x4C, 0x88, 0x4F),
    };
    static const uint8_t valid_data[] =
    {
        0x06,       // Data type
        0x00, 0x00, // Temperature
        0x00, 0x00, // Humidity
        0x00, 0x00, // Pressure
        0x00, 0x00, // PM2.5
        0x00, 0x00, // CO2
        0x00,       // VOC
        0x00,       // NOX
        0x00,       // Luminosity
        0x00,       // Sound dBA avg
        0x00,       // Seq cnt2
        0x04,       // Flags
        0x4C,       // MAC address byte 3
        0x88,       // MAC address byte 4
        0x4F        // MAC address byte 5
    };
    _Static_assert (sizeof (valid_data) == RE_6_DATA_LENGTH);
    uint8_t        raw_buf[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER };
    uint8_t * const p_payload                  = &raw_buf[RE_6_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, p_payload, sizeof (valid_data));
    re_6_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa), lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt2, decoded_data.seq_cnt2);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte3, decoded_data.mac_addr_24.byte3);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte4, decoded_data.mac_addr_24.byte4);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte5, decoded_data.mac_addr_24.byte5);
}

void
test_ruuvi_endpoint_6_get_ok_max (void)
{
    static const re_6_flags_t flags =
    {
        .flag_calibration_in_progress = true,
        .flag_button_pressed          = true,
        .flag_rtc_running_on_boot     = true,
    };
    static const re_6_data_t data =
    {
        .temperature_c = 163.835f,
        .humidity_rh   = 100.0f,
        .pressure_pa   = 115534.0f,
        .pm2p5_ppm     = 1000.0f,
        .co2           = 40000,
        .voc           = 500,
        .nox           = 500,
        .luminosity    = 65535,
        .sound_avg_dba = 120,
        .seq_cnt2      = 255,
        .flags         = flags,
        .mac_addr_24   = RE_6_DATA_MAC_ADDR_INIT (0xFF, 0xFF, 0xFF),
    };
    static const uint8_t max_data[] =
    {
        0x06,       // Data type
        0x7F, 0xFF, // Temperature
        0x9C, 0x40, // Humidity
        0xFF, 0xFE, // Pressure
        0x27, 0x10, // PM2.5
        0x9C, 0x40, // CO2
        0xFA,       // VOC
        0xFA,       // NOX
        0xFE,       // Luminosity
        0xFF,       // Sound dBA avg
        0xFF,       // Seq cnt2
        0x07,       // Flags
        0xFF,       // MAC address byte 3
        0xFF,       // MAC address byte 4
        0xFF        // MAC address byte 5
    };
    _Static_assert (sizeof (max_data) == RE_6_DATA_LENGTH);
    uint8_t        raw_buf[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER };
    uint8_t * const p_payload                  = &raw_buf[RE_6_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (max_data, p_payload, sizeof (max_data));
    re_6_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa), lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt2, decoded_data.seq_cnt2);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte3, decoded_data.mac_addr_24.byte3);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte4, decoded_data.mac_addr_24.byte4);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte5, decoded_data.mac_addr_24.byte5);
}

void
test_ruuvi_endpoint_6_get_ok_min (void)
{
    static const re_6_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_6_data_t data =
    {
        .temperature_c = -163.835f,
        .humidity_rh   = 0.0f,
        .pressure_pa   = 50000.0f,
        .pm2p5_ppm     = 0.0f,
        .co2           = 0,
        .voc           = 0,
        .nox           = 0,
        .luminosity    = 0,
        .sound_avg_dba = 18,
        .seq_cnt2      = 0,
        .flags         = flags,
        .mac_addr_24   = RE_6_DATA_MAC_ADDR_INIT (0x00, 0x00, 0x00),
    };
    static const uint8_t min_data[] =
    {
        0x06,       // Data type
        0x80, 0x01, // Temperature
        0x00, 0x00, // Humidity
        0x00, 0x00, // Pressure
        0x00, 0x00, // PM2.5
        0x00, 0x00, // CO2
        0x00,       // VOC
        0x00,       // NOX
        0x00,       // Luminosity
        0x00,       // Sound dBA avg
        0x00,       // Seq cnt2
        0x00,       // Flags
        0x00,       // MAC address byte 3
        0x00,       // MAC address byte 4
        0x00        // MAC address byte 5
    };
    _Static_assert (sizeof (min_data) == RE_6_DATA_LENGTH);
    uint8_t        raw_buf[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER };
    uint8_t * const p_payload                  = &raw_buf[RE_6_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (min_data, p_payload, sizeof (min_data));
    re_6_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa), lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt2, decoded_data.seq_cnt2);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte3, decoded_data.mac_addr_24.byte3);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte4, decoded_data.mac_addr_24.byte4);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte5, decoded_data.mac_addr_24.byte5);
}

/**
 * @brief Null buffer
 *
 * @retval RE_ERROR_NULL on success test.
 */
void
test_ruuvi_endpoint_6_get_error_null_buffer (void)
{
    static const re_6_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_6_data_t m_re_6_data_ok =
    {
        .temperature_c = 29.5f,
        .humidity_rh   = 55.3f,
        .pressure_pa   = 101102.0f,
        .pm2p5_ppm     = 11.2f,
        .co2           = 201,
        .voc           = 10,
        .nox           = 2,
        .luminosity    = 13027,
        .sound_avg_dba = 56.6f,
        .seq_cnt2      = 205,
        .flags         = flags,
        .mac_addr_24   = RE_6_DATA_MAC_ADDR_INIT (0x4C, 0x88, 0x4F),
    };
    uint8_t * const p_test_buffer = NULL;
    TEST_ASSERT_EQUAL (RE_ERROR_NULL, re_6_encode (p_test_buffer, &m_re_6_data_ok));
}

/**
 * @brief Null data
 *
 * @retval RE_ERROR_NULL on success test.
 */
void
test_ruuvi_endpoint_6_get_error_null_data (void)
{
    uint8_t            test_buffer[RE_6_DATA_LENGTH] = { 0 };
    const re_6_data_t * p_re_6_data                   = NULL;
    TEST_ASSERT_EQUAL (RE_ERROR_NULL, re_6_encode ( (uint8_t * const) &test_buffer,
                       p_re_6_data));
}

/**
 * @brief True to test encode operation with invalid data
 *
 * @retval RD_SUCCESS on success test.
 */
void
test_ruuvi_endpoint_6_get_invalid_data (void)
{
    static const re_6_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_6_data_t data =
    {
        .temperature_c = NAN,
        .humidity_rh   = NAN,
        .pressure_pa   = NAN,
        .pm2p5_ppm     = NAN,
        .co2           = NAN,
        .voc           = NAN,
        .nox           = NAN,
        .luminosity    = NAN,
        .sound_avg_dba = NAN,
        .seq_cnt2      = 255,
        .flags         = flags,
        .mac_addr_24   = RE_6_DATA_MAC_ADDR_INIT (0xFF, 0xFF, 0xFF),
    };
    static const uint8_t invalid_data[] =
    {
        0x06,       // Data type
        0x80, 0x00, // Temperature
        0xFF, 0xFF, // Humidity
        0xFF, 0xFF, // Pressure
        0xFF, 0xFF, // PM2.5
        0xFF, 0xFF, // CO2
        0xFF,       // VOC
        0xFF,       // NOX
        0xFF,       // Luminosity
        0xFF,       // Sound dBA avg
        0xFF,       // Seq cnt2
        0xD0,       // Flags
        0xFF,       // MAC address byte 3
        0xFF,       // MAC address byte 4
        0xFF        // MAC address byte 5
    };
    _Static_assert (sizeof (invalid_data) == RE_6_DATA_LENGTH);
    uint8_t        raw_buf[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER };
    uint8_t * const p_payload                  = &raw_buf[RE_6_OFFSET_PAYLOAD];
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_encode (p_payload, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (invalid_data, p_payload, sizeof (invalid_data));
    re_6_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (data.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.pressure_pa), lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (data.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (data.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (data.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (data.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (data.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (data.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (data.seq_cnt2, decoded_data.seq_cnt2);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress,
                       decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte3, decoded_data.mac_addr_24.byte3);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte4, decoded_data.mac_addr_24.byte4);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte5, decoded_data.mac_addr_24.byte5);
}

void
test_ruuvi_endpoint_6_underflow (void)
{
    static const re_6_flags_t flags =
    {
        .flag_calibration_in_progress = false,
        .flag_button_pressed          = false,
        .flag_rtc_running_on_boot     = false,
    };
    static const re_6_data_t data =
    {
        .temperature_c = -164,
        .humidity_rh   = -1,
        .pressure_pa   = 49999,
        .pm2p5_ppm     = -1,
        .co2           = -1,
        .voc           = -1,
        .nox           = -1,
        .luminosity    = -1,
        .sound_avg_dba = 17,
        .seq_cnt2      = 0,
        .flags         = flags,
        .mac_addr_24   = RE_6_DATA_MAC_ADDR_INIT (0x00, 0x00, 0x00),
    };
    static const uint8_t min_data[] =
    {
        0x06,       // Data type
        0x80, 0x01, // Temperature
        0x00, 0x00, // Humidity
        0x00, 0x00, // Pressure
        0x00, 0x00, // PM2.5
        0x00, 0x00, // CO2
        0x00,       // VOC
        0x00,       // NOX
        0x00,       // Luminosity
        0x00,       // Sound dBA avg
        0x00,       // Seq cnt2
        0x00,       // Flags
        0x00,       // MAC address byte 3
        0x00,       // MAC address byte 4
        0x00        // MAC address byte 5
    };
    _Static_assert (sizeof (min_data) == RE_6_DATA_LENGTH);
    uint8_t test_buffer[RE_6_DATA_LENGTH] = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_encode (test_buffer, &data));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (min_data, test_buffer, sizeof (min_data));
}

void
test_ruuvi_endpoint_6_overflow (void)
{
    static const re_6_flags_t flags =
    {
        .flag_calibration_in_progress = true,
        .flag_button_pressed          = true,
        .flag_rtc_running_on_boot     = true,
    };
    static const re_6_data_t m_re_6_data_overflow =
    {
        .temperature_c = 163.9f,
        .humidity_rh   = 100.1f,
        .pressure_pa   = 115535.0f,
        .pm2p5_ppm     = 1000.1f,
        .co2           = 40001,
        .voc           = 501,
        .nox           = 501,
        .luminosity    = 65536,
        .sound_avg_dba = 121,
        .seq_cnt2      = 255,
        .flags         = flags,
        .mac_addr_24   = RE_6_DATA_MAC_ADDR_INIT (0xFF, 0xFF, 0xFF),
    };
    static const uint8_t max_data[] =
    {
        0x06,       // Data type
        0x7F, 0xFF, // Temperature
        0x9C, 0x40, // Humidity
        0xFF, 0xFE, // Pressure
        0x27, 0x10, // PM2.5
        0x9C, 0x40, // CO2
        0xFA,       // VOC
        0xFA,       // NOX
        0xFE,       // Luminosity
        0xFF,       // Sound dBA avg
        0xFF,       // Seq cnt2
        0x07,       // Flags
        0xFF,       // MAC address byte 3
        0xFF,       // MAC address byte 4
        0xFF        // MAC address byte 5
    };
    _Static_assert (sizeof (max_data) == RE_6_DATA_LENGTH);
    uint8_t test_buffer[RE_6_DATA_LENGTH] = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_encode (test_buffer, &m_re_6_data_overflow));
    TEST_ASSERT_EQUAL_HEX8_ARRAY (max_data, test_buffer, sizeof (max_data));
}

void
test_ruuvi_endpoint_6_check_format_ok (void)
{
    uint8_t raw_buf[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER, 0x06 };
    TEST_ASSERT_TRUE (re_6_check_format (raw_buf));
}

void
test_ruuvi_endpoint_6_check_format_fail (void)
{
    {
        uint8_t raw_buf_byte0[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER, 0x06 };
        raw_buf_byte0[0] += 1;
        TEST_ASSERT_FALSE (re_6_check_format (raw_buf_byte0));
    }
    {
        uint8_t raw_buf_byte1[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER, 0x06 };
        raw_buf_byte1[1] += 1;
        TEST_ASSERT_FALSE (re_6_check_format (raw_buf_byte1));
    }
    {
        uint8_t raw_buf_byte3[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER, 0x06 };
        raw_buf_byte3[1] += 1;
        TEST_ASSERT_FALSE (re_6_check_format (raw_buf_byte3));
    }
    {
        uint8_t raw_buf_byte4[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER, 0x06 };
        raw_buf_byte4[4] += 1;
        TEST_ASSERT_FALSE (re_6_check_format (raw_buf_byte4));
    }
    {
        uint8_t raw_buf_byte5[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER, 0x06 };
        raw_buf_byte5[5] += 1;
        TEST_ASSERT_FALSE (re_6_check_format (raw_buf_byte5));
    }
    {
        uint8_t raw_buf_byte6[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER, 0x06 };
        raw_buf_byte6[6] += 1;
        TEST_ASSERT_FALSE (re_6_check_format (raw_buf_byte6));
    }
    {
        uint8_t raw_buf_len[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER, 0x06 };
        raw_buf_len[7] -= 1; // Length byte
        TEST_ASSERT_FALSE (re_6_check_format (raw_buf_len));
    }
    {
        uint8_t raw_buf_type[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER, 0x06 };
        raw_buf_type[8] -= 1; // Payload type byte
        TEST_ASSERT_FALSE (re_6_check_format (raw_buf_type));
    }
    {
        uint8_t raw_buf_manufacturer_id_2[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER, 0x06 };
        raw_buf_manufacturer_id_2[9] += 1; // Manufacturer ID byte 1
        TEST_ASSERT_FALSE (re_6_check_format (raw_buf_manufacturer_id_2));
    }
    {
        uint8_t raw_buf_manufacturer_id_1[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER, 0x06 };
        raw_buf_manufacturer_id_1[10] += 1; // Manufacturer ID byte 0
        TEST_ASSERT_FALSE (re_6_check_format (raw_buf_manufacturer_id_1));
    }
    {
        uint8_t raw_buf_wrong_payload_format[RE_6_RAW_BUF_SIZE] = { RE_6_BLE_PACKET_HEADER, 0x06 };
        raw_buf_wrong_payload_format[11] += 1;
        TEST_ASSERT_FALSE (re_6_check_format (raw_buf_wrong_payload_format));
    }
}

void
test_re_6_data_invalid (void)
{
    const uint16_t    measurement_cnt = 0xDABA;
    const uint64_t    radio_mac       = 0xCBB8334C884FULL;
    const re_6_data_t data            = re_6_data_invalid (measurement_cnt, radio_mac);
    TEST_ASSERT_TRUE (isnan (data.temperature_c));
    TEST_ASSERT_TRUE (isnan (data.humidity_rh));
    TEST_ASSERT_TRUE (isnan (data.pressure_pa));
    TEST_ASSERT_TRUE (isnan (data.pm2p5_ppm));
    TEST_ASSERT_TRUE (isnan (data.co2));
    TEST_ASSERT_TRUE (isnan (data.voc));
    TEST_ASSERT_TRUE (isnan (data.nox));
    TEST_ASSERT_TRUE (isnan (data.luminosity));
    TEST_ASSERT_TRUE (isnan (data.sound_avg_dba));
    TEST_ASSERT_EQUAL (data.seq_cnt2, measurement_cnt & 0xFF);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress, false);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed, false);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot, false);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte3, (radio_mac >> 16) & 0xFFU);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte4, (radio_mac >> 8) & 0xFFU);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte5, (radio_mac >> 0) & 0xFFU);
}
