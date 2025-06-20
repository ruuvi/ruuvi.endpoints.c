#include "unity.h"

#include "ruuvi_endpoint_6.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

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
    static const re_6_data_t m_re_6_data_ok =
    {
        .temperature_c = 29.5f,
        .humidity_rh = 55.3f,
        .pressure_pa = 101102.0f,
        .pm2p5_ppm = 11.2f,
        .co2 = 201,
        .voc = 10,
        .nox = 2,
        .luminosity = 13027,
        .sound_dba_avg = 56.6f,
        .seq_cnt2 = 205,
        .flags = {
            .flag_calibration_in_progress = false,
            .flag_button_pressed = false,
            .flag_rtc_running_on_boot = false,
        },
        .mac_addr_24 = {
            .byte3 = 0x4C,
            .byte4 = 0x88,
            .byte5 = 0x4F,
        }
    };
    static const uint8_t valid_data[RE_6_DATA_LENGTH] =
    {
        0x06, 0x17, 0x0C, // Temperature
        0x56, 0x68,       // Humidity
        0xC7, 0x9E,       // Pressure
        0x00, 0x70,       // PM2.5
        0x00, 0xC9,       // CO2
        0x0A,             // VOC
        0x02,             // NOX
        0xD9,             // Luminosity
        0x94,             // Sound dBA avg
        0xCD,             // Seq cnt2
        0x00,             // Flags
        0x4C,             // MAC address byte 3
        0x88,             // MAC address byte 4
        0x4F              // MAC address byte 5
    };
    re_status_t err_code                      = RE_SUCCESS;
    uint8_t     test_buffer[RE_6_DATA_LENGTH] = { 0 };
    err_code                                  = re_6_encode (test_buffer, &m_re_6_data_ok);
    TEST_ASSERT_EQUAL (RE_SUCCESS, err_code);
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, test_buffer, sizeof (valid_data));
    uint8_t raw_buf[31] = { 0x02, 0x01, 0x06, 0x03, 0x03, 0x98, 0xFC, 0x17, 0xFF, 0x99, 0x04 };
    memcpy (&raw_buf[RE_6_OFFSET_PAYLOAD], test_buffer, sizeof (test_buffer));
    re_6_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok.pressure_pa),
                       lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok.sound_dba_avg * 10.0f),
                       lrintf (decoded_data.sound_dba_avg * 10.0f));
    TEST_ASSERT_EQUAL (m_re_6_data_ok.seq_cnt2, decoded_data.seq_cnt2);
    TEST_ASSERT_EQUAL (
        m_re_6_data_ok.flags.flag_calibration_in_progress,
        decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (m_re_6_data_ok.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (m_re_6_data_ok.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (m_re_6_data_ok.mac_addr_24.byte3, decoded_data.mac_addr_24.byte3);
    TEST_ASSERT_EQUAL (m_re_6_data_ok.mac_addr_24.byte4, decoded_data.mac_addr_24.byte4);
    TEST_ASSERT_EQUAL (m_re_6_data_ok.mac_addr_24.byte5, decoded_data.mac_addr_24.byte5);
}

void
test_ruuvi_endpoint_6_get_ok_max (void)
{
    static const re_6_data_t m_re_6_data_ok_max =
    {
        .temperature_c = 163.835f,
        .humidity_rh = 100.0f,
        .pressure_pa = 115534.0f,
        .pm2p5_ppm = 1000.0f,
        .co2 = 40000,
        .voc = 500,
        .nox = 500,
        .luminosity = 65535,
        .sound_dba_avg = 129,
        .seq_cnt2 = 255,
        .flags = {
            .flag_calibration_in_progress = true,
            .flag_button_pressed = true,
            .flag_rtc_running_on_boot = true,
        },
        .mac_addr_24 = {
            .byte3 = 0xFF,
            .byte4 = 0xFF,
            .byte5 = 0xFF,
        }
    };
    static const uint8_t max_data[] =
    {
        0x06, 0x7F, 0xFF, // Temperature
        0x9C, 0x40,       // Humidity
        0xFF, 0xFE,       // Pressure
        0x27, 0x10,       // PM2.5
        0x9C, 0x40,       // CO2
        0xF4,             // VOC
        0xF4,             // NOX
        0xFE,             // Luminosity
        0xFE,             // Sound dBA avg
        0xFF,             // Seq cnt2
        0xD7,             // Flags
        0xFF,             // MAC address byte 3
        0xFF,             // MAC address byte 4
        0xFF              // MAC address byte 5
    };
    re_status_t err_code                      = RE_SUCCESS;
    uint8_t     test_buffer[RE_6_DATA_LENGTH] = { 0 };
    err_code = re_6_encode ( (uint8_t * const) &test_buffer,
                             (const re_6_data_t *) &m_re_6_data_ok_max);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT_EQUAL_HEX8_ARRAY (max_data, test_buffer, sizeof (max_data));
    uint8_t raw_buf[31] = { 0x02, 0x01, 0x06, 0x03, 0x03, 0x98, 0xFC, 0x17, 0xFF, 0x99, 0x04 };
    memcpy (&raw_buf[RE_6_OFFSET_PAYLOAD], test_buffer, sizeof (test_buffer));
    re_6_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_max.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_max.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_max.pressure_pa),
                       lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_max.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_max.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_max.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_max.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_max.luminosity),
                       lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_max.sound_dba_avg * 10.0f),
                       lrintf (decoded_data.sound_dba_avg * 10.0f));
    TEST_ASSERT_EQUAL (m_re_6_data_ok_max.seq_cnt2, decoded_data.seq_cnt2);
    TEST_ASSERT_EQUAL (
        m_re_6_data_ok_max.flags.flag_calibration_in_progress,
        decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (m_re_6_data_ok_max.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (m_re_6_data_ok_max.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (m_re_6_data_ok_max.mac_addr_24.byte3, decoded_data.mac_addr_24.byte3);
    TEST_ASSERT_EQUAL (m_re_6_data_ok_max.mac_addr_24.byte4, decoded_data.mac_addr_24.byte4);
    TEST_ASSERT_EQUAL (m_re_6_data_ok_max.mac_addr_24.byte5, decoded_data.mac_addr_24.byte5);
}

void
test_ruuvi_endpoint_6_get_ok_min (void)
{
    static const re_6_data_t m_re_6_data_ok_min =
    {
        .temperature_c = -163.835f,
        .humidity_rh = 0.0f,
        .pressure_pa = 50000.0f,
        .pm2p5_ppm = 0.0f,
        .co2 = 0,
        .voc = 0,
        .nox = 0,
        .luminosity = 0,
        .sound_dba_avg = 27,
        .seq_cnt2 = 0,
        .flags = {
            .flag_calibration_in_progress = false,
            .flag_button_pressed = false,
            .flag_rtc_running_on_boot = false,
        },
        .mac_addr_24 = {
            .byte3 = 0x00,
            .byte4 = 0x00,
            .byte5 = 0x00,
        }
    };
    static const uint8_t min_data[] =
    {
        0x06, 0x80, 0x01, // Temperature
        0x00, 0x00,       // Humidity
        0x00, 0x00,       // Pressure
        0x00, 0x00,       // PM2.5
        0x00, 0x00,       // CO2
        0x00,             // VOC
        0x00,             // NOX
        0x00,             // Luminosity
        0x00,             // Sound dBA avg
        0x00,             // Seq cnt2
        0x00,             // Flags
        0x00,             // MAC address byte 3
        0x00,             // MAC address byte 4
        0x00              // MAC address byte 5
    };
    re_status_t err_code                      = RE_SUCCESS;
    uint8_t     test_buffer[RE_6_DATA_LENGTH] = { 0 };
    err_code = re_6_encode ( (uint8_t * const) &test_buffer,
                             (const re_6_data_t *) &m_re_6_data_ok_min);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT_EQUAL_HEX8_ARRAY (min_data, test_buffer, sizeof (min_data));
    uint8_t raw_buf[31] = { 0x02, 0x01, 0x06, 0x03, 0x03, 0x98, 0xFC, 0x17, 0xFF, 0x99, 0x04 };
    memcpy (&raw_buf[RE_6_OFFSET_PAYLOAD], test_buffer, sizeof (test_buffer));
    re_6_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_min.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_min.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_min.pressure_pa),
                       lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_min.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_min.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_min.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_min.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_min.luminosity),
                       lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_min.sound_dba_avg * 10.0f),
                       lrintf (decoded_data.sound_dba_avg * 10.0f));
    TEST_ASSERT_EQUAL (m_re_6_data_ok_min.seq_cnt2, decoded_data.seq_cnt2);
    TEST_ASSERT_EQUAL (
        m_re_6_data_ok_min.flags.flag_calibration_in_progress,
        decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (m_re_6_data_ok_min.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (m_re_6_data_ok_min.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (m_re_6_data_ok_min.mac_addr_24.byte3, decoded_data.mac_addr_24.byte3);
    TEST_ASSERT_EQUAL (m_re_6_data_ok_min.mac_addr_24.byte4, decoded_data.mac_addr_24.byte4);
    TEST_ASSERT_EQUAL (m_re_6_data_ok_min.mac_addr_24.byte5, decoded_data.mac_addr_24.byte5);
}

/**
 * @brief Null buffer
 *
 * @retval RE_ERROR_NULL on success test.
 */
void
test_ruuvi_endpoint_6_get_error_null_buffer (void)
{
    static const re_6_data_t m_re_6_data_ok =
    {
        .temperature_c = 29.5f,
        .humidity_rh = 55.3f,
        .pressure_pa = 101102.0f,
        .pm2p5_ppm = 11.2f,
        .co2 = 201,
        .voc = 10,
        .nox = 2,
        .luminosity = 13027,
        .sound_dba_avg = 56.6f,
        .seq_cnt2 = 205,
        .flags = {
            .flag_calibration_in_progress = false,
            .flag_button_pressed = false,
            .flag_rtc_running_on_boot = false,
        },
        .mac_addr_24 = {
            .byte3 = 0x4C,
            .byte4 = 0x88,
            .byte5 = 0x4F,
        }
    };
    re_status_t    err_code      = RE_ERROR_NULL;
    uint8_t * const p_test_buffer = NULL;
    err_code                     = re_6_encode (p_test_buffer, &m_re_6_data_ok);
    TEST_ASSERT (RE_ERROR_NULL == err_code);
}

/**
 * @brief Null data
 *
 * @retval RE_ERROR_NULL on success test.
 */
void
test_ruuvi_endpoint_6_get_error_null_data (void)
{
    re_status_t        err_code                      = RE_ERROR_NULL;
    uint8_t            test_buffer[RE_6_DATA_LENGTH] = { 0 };
    const re_6_data_t * p_re_6_data                   = NULL;
    err_code                                         = re_6_encode ( (
                uint8_t * const) &test_buffer, p_re_6_data);
    TEST_ASSERT (RE_ERROR_NULL == err_code);
}

/**
 * @brief True to test encode operation with invalid data
 *
 * @retval RD_SUCCESS on success test.
 */
void
test_ruuvi_endpoint_6_get_invalid_data (void)
{
    static const re_6_data_t m_re_6_data_invalid =
    {
        .temperature_c = NAN,
        .humidity_rh = NAN,
        .pressure_pa = NAN,
        .pm2p5_ppm = NAN,
        .co2 = NAN,
        .voc = NAN,
        .nox = NAN,
        .luminosity = NAN,
        .sound_dba_avg = NAN,
        .seq_cnt2 = 255,
        .flags = {
            .flag_calibration_in_progress = false,
            .flag_button_pressed = false,
            .flag_rtc_running_on_boot = false,
        },
        .mac_addr_24 = {
            .byte3 = 0xFF,
            .byte4 = 0xFF,
            .byte5 = 0xFF,
        }
    };
    static const uint8_t invalid_data[] =
    {
        0x06, 0x80, 0x00, // Temperature
        0xFF, 0xFF,       // Humidity
        0xFF, 0xFF,       // Pressure
        0xFF, 0xFF,       // PM2.5
        0xFF, 0xFF,       // CO2
        0xFF,             // VOC
        0xFF,             // NOX
        0xFF,             // Luminosity
        0xFF,             // Sound dBA avg
        0xFF,             // Seq cnt2
        0xD0,             // Flags
        0xFF,             // MAC address byte 3
        0xFF,             // MAC address byte 4
        0xFF              // MAC address byte 5
    };
    re_status_t err_code                      = RE_SUCCESS;
    uint8_t     test_buffer[RE_6_DATA_LENGTH] = { 0 };
    err_code = re_6_encode ( (uint8_t * const) &test_buffer,
                             (const re_6_data_t *) &m_re_6_data_invalid);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT_EQUAL_HEX8_ARRAY (invalid_data, test_buffer, sizeof (invalid_data));
    uint8_t raw_buf[31] = { 0x02, 0x01, 0x06, 0x03, 0x03, 0x98, 0xFC, 0x17, 0xFF, 0x99, 0x04 };
    memcpy (&raw_buf[RE_6_OFFSET_PAYLOAD], test_buffer, sizeof (test_buffer));
    re_6_data_t decoded_data = { 0 };
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_invalid.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_invalid.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_invalid.pressure_pa),
                       lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_invalid.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_invalid.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_invalid.voc), lrintf (decoded_data.voc));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_invalid.nox), lrintf (decoded_data.nox));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_invalid.luminosity),
                       lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_invalid.sound_dba_avg * 10.0f),
                       lrintf (decoded_data.sound_dba_avg * 10.0f));
    TEST_ASSERT_EQUAL (m_re_6_data_invalid.seq_cnt2, decoded_data.seq_cnt2);
    TEST_ASSERT_EQUAL (
        m_re_6_data_invalid.flags.flag_calibration_in_progress,
        decoded_data.flags.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (m_re_6_data_invalid.flags.flag_button_pressed,
                       decoded_data.flags.flag_button_pressed);
    TEST_ASSERT_EQUAL (m_re_6_data_invalid.flags.flag_rtc_running_on_boot,
                       decoded_data.flags.flag_rtc_running_on_boot);
    TEST_ASSERT_EQUAL (m_re_6_data_invalid.mac_addr_24.byte3, decoded_data.mac_addr_24.byte3);
    TEST_ASSERT_EQUAL (m_re_6_data_invalid.mac_addr_24.byte4, decoded_data.mac_addr_24.byte4);
    TEST_ASSERT_EQUAL (m_re_6_data_invalid.mac_addr_24.byte5, decoded_data.mac_addr_24.byte5);
}

void
test_ruuvi_endpoint_6_underflow (void)
{
    static const re_6_data_t m_re_6_data_underflow =
    {
        .temperature_c = -164,
        .humidity_rh = -1,
        .pressure_pa = 49999,
        .pm2p5_ppm = -1,
        .co2 = -1,
        .voc = -1,
        .nox = -1,
        .luminosity = -1,
        .sound_dba_avg = 26,
        .seq_cnt2 = 0,
        .flags = {
            .flag_calibration_in_progress = false,
            .flag_button_pressed = false,
            .flag_rtc_running_on_boot = false,
        },
        .mac_addr_24 = {
            .byte3 = 0x00,
            .byte4 = 0x00,
            .byte5 = 0x00,
        }
    };
    static const uint8_t min_data[] =
    {
        0x06, 0x80, 0x01, // Temperature
        0x00, 0x00,       // Humidity
        0x00, 0x00,       // Pressure
        0x00, 0x00,       // PM2.5
        0x00, 0x00,       // CO2
        0x00,             // VOC
        0x00,             // NOX
        0x00,             // Luminosity
        0x00,             // Sound dBA avg
        0x00,             // Seq cnt2
        0x00,             // Flags
        0x00,             // MAC address byte 3
        0x00,             // MAC address byte 4
        0x00              // MAC address byte 5
    };
    re_status_t err_code                      = RE_SUCCESS;
    uint8_t     test_buffer[RE_6_DATA_LENGTH] = { 0 };
    err_code                                  = re_6_encode (test_buffer,
            &m_re_6_data_underflow);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT_EQUAL_HEX8_ARRAY (min_data, test_buffer, sizeof (min_data));
}

void
test_ruuvi_endpoint_6_overflow (void)
{
    static const re_6_data_t m_re_6_data_overflow =
    {
        .temperature_c = 163.9,
        .humidity_rh = 100.1f,
        .pressure_pa = 115535.0f,
        .pm2p5_ppm = 1000.1f,
        .co2 = 40001,
        .voc = 501,
        .nox = 501,
        .luminosity = 65536,
        .sound_dba_avg = 130,
        .seq_cnt2 = 255,
        .flags = {
            .flag_calibration_in_progress = true,
            .flag_button_pressed = true,
            .flag_rtc_running_on_boot = true,
        },
        .mac_addr_24 = {
            .byte3 = 0xFF,
            .byte4 = 0xFF,
            .byte5 = 0xFF,
        }
    };
    static const uint8_t max_data[] =
    {
        0x06, 0x7F, 0xFF, // Temperature
        0x9C, 0x40,       // Humidity
        0xFF, 0xFE,       // Pressure
        0x27, 0x10,       // PM2.5
        0x9C, 0x40,       // CO2
        0xF4,             // VOC
        0xF4,             // NOX
        0xFE,             // Luminosity
        0xFE,             // Sound dBA avg
        0xFF,             // Seq cnt2
        0xD7,             // Flags
        0xFF,             // MAC address byte 3
        0xFF,             // MAC address byte 4
        0xFF              // MAC address byte 5
    };
    re_status_t err_code                      = RE_SUCCESS;
    uint8_t     test_buffer[RE_6_DATA_LENGTH] = { 0 };
    err_code                                  = re_6_encode (test_buffer,
            &m_re_6_data_overflow);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT_EQUAL_HEX8_ARRAY (max_data, test_buffer, sizeof (max_data));
}

void
test_ruuvi_endpoint_6_check_format_ok (void)
{
    uint8_t raw_buf[31] = { 0x02, 0x01, 0x06, 0x03, 0x03, 0x98, 0xFC, 0x17, 0xFF, 0x99, 0x04, 0x06 };
    TEST_ASSERT_TRUE (re_6_check_format (raw_buf));
}

void
test_ruuvi_endpoint_6_check_format_fail (void)
{
    const uint8_t raw_buf_payload_format[31] =
    {
        0x02, 0x01, 0x06, 0x03, 0x03, 0x98, 0xFC, 0x17, 0xFF, 0x99, 0x04, 0x05
    };
    TEST_ASSERT_FALSE (re_6_check_format (raw_buf_payload_format));
    const uint8_t raw_buf_manufacturer_id_1[31] = { 0x02, 0x01, 0x06, 0x03, 0x03, 0x98,
                                                    0xFC, 0x17, 0xFF, 0x99, 0x05, 0x06
                                                  };
    TEST_ASSERT_FALSE (re_6_check_format (raw_buf_manufacturer_id_1));
    const uint8_t raw_buf_manufacturer_id_2[31] = { 0x02, 0x01, 0x06, 0x03, 0x03, 0x98,
                                                    0xFC, 0x17, 0xFF, 0x9A, 0x04, 0x06
                                                  };
    TEST_ASSERT_FALSE (re_6_check_format (raw_buf_manufacturer_id_2));
    const uint8_t raw_buf_type[31] = { 0x02, 0x01, 0x06, 0x03, 0x03, 0x98, 0xFC, 0x17, 0xFE, 0x99, 0x04, 0x06 };
    TEST_ASSERT_FALSE (re_6_check_format (raw_buf_type));
    const uint8_t raw_buf_len[31] = { 0x02, 0x01, 0x06, 0x03, 0x03, 0x98, 0xFC, 0x16, 0xFF, 0x99, 0x04, 0x06 };
    TEST_ASSERT_FALSE (re_6_check_format (raw_buf_len));
    const uint8_t raw_buf_byte0[31] = { 0x03, 0x01, 0x06, 0x03, 0x03, 0x98, 0xFC, 0x17, 0xFF, 0x99, 0x04, 0x06 };
    TEST_ASSERT_FALSE (re_6_check_format (raw_buf_byte0));
    const uint8_t raw_buf_byte1[31] = { 0x02, 0x02, 0x06, 0x03, 0x03, 0x98, 0xFC, 0x17, 0xFF, 0x99, 0x04, 0x06 };
    TEST_ASSERT_FALSE (re_6_check_format (raw_buf_byte1));
    const uint8_t raw_buf_byte3[31] = { 0x02, 0x01, 0x06, 0x04, 0x03, 0x98, 0xFC, 0x17, 0xFF, 0x99, 0x04, 0x06 };
    TEST_ASSERT_FALSE (re_6_check_format (raw_buf_byte3));
    const uint8_t raw_buf_byte4[31] = { 0x02, 0x01, 0x06, 0x03, 0x04, 0x98, 0xFC, 0x17, 0xFF, 0x99, 0x04, 0x06 };
    TEST_ASSERT_FALSE (re_6_check_format (raw_buf_byte4));
    const uint8_t raw_buf_byte5[31] = { 0x02, 0x01, 0x06, 0x03, 0x03, 0x99, 0xFC, 0x17, 0xFF, 0x99, 0x04, 0x06 };
    TEST_ASSERT_FALSE (re_6_check_format (raw_buf_byte5));
    const uint8_t raw_buf_byte6[31] = { 0x02, 0x01, 0x06, 0x03, 0x03, 0x98, 0xFD, 0x17, 0xFF, 0x99, 0x04, 0x06 };
    TEST_ASSERT_FALSE (re_6_check_format (raw_buf_byte6));
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
    TEST_ASSERT_TRUE (isnan (data.sound_dba_avg));
    TEST_ASSERT_EQUAL (data.seq_cnt2, measurement_cnt & 0xFF);
    TEST_ASSERT_EQUAL (data.flags.flag_calibration_in_progress, false);
    TEST_ASSERT_EQUAL (data.flags.flag_button_pressed, false);
    TEST_ASSERT_EQUAL (data.flags.flag_rtc_running_on_boot, false);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte3, (radio_mac >> 16) & 0xFFU);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte4, (radio_mac >> 8) & 0xFFU);
    TEST_ASSERT_EQUAL (data.mac_addr_24.byte5, (radio_mac >> 0) & 0xFFU);
}
