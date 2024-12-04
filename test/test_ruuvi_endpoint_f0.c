#include "unity.h"

#include "ruuvi_endpoint_f0.h"
#include <string.h>
#include <stdint.h>


void setUp (void)
{
}

void tearDown (void)
{
}

/**
 * @brief Typical encode operation
 *
 * @retval RD_SUCCESS on success test.
 */
void test_ruuvi_endpoint_f0_get_ok (void)
{
    static const re_f0_data_t m_re_f0_data_ok =
    {
        .temperature_c = 26.5f,
        .humidity_rh = 80.5f,
        .pressure_pa = 101355,
        .pm1p0_ppm = 10.2f,
        .pm2p5_ppm = 11.3f,
        .pm4p0_ppm = 12.4f,
        .pm10p0_ppm = 13.5f,
        .co2 = 1129,
        .voc_index = 11,
        .nox_index = 12,
        .luminosity = 15123,
        .sound_dba_avg = 20.5f,
        .flag_usb_on = true,
        .flag_low_battery = false,
        .flag_calibration_in_progress = false,
        .flag_boost_mode = false,
        .flag_seq_cnt = 5,
        .address = 0xCBB8334C884F,
    };
    static const uint8_t valid_data[] =
    {
        0xF0,
        26, // Temperature
        161, // Humidity
        114, // Pressure
        89, // PM1.0
        92, // PM2.5
        95, // PM4.0
        98, // PM10.0
        169, // CO2
        98, // VOX
        102, // NOX
        231, // Luminosity
        41, // Sound avg
        0x51, // Flags
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F
    };
    _Static_assert (sizeof (valid_data) == RE_F0_DATA_LENGTH);
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[20] = {0};
    _Static_assert (sizeof (valid_data) == sizeof (test_buffer), "Invalid test data size");
    err_code = re_f0_encode (test_buffer, &m_re_f0_data_ok);
    TEST_ASSERT_EQUAL (RE_SUCCESS, err_code);
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, test_buffer, sizeof (valid_data));
    const uint8_t raw_buf_prefix[] = {0x02, 0x01, 0x06, 0x03, 0x03, 0x98, 0xFC, 0x17, 0xFF, 0x99, 0x04};
    _Static_assert (sizeof (raw_buf_prefix) == RE_F0_OFFSET_PAYLOAD,
                    "Invalid raw_buf_prefix size");
    uint8_t raw_buf[31];
    memcpy (&raw_buf[0], raw_buf_prefix, sizeof (raw_buf_prefix));
    memcpy (&raw_buf[RE_F0_OFFSET_PAYLOAD], test_buffer, sizeof (test_buffer));
    re_f0_data_t decoded_data = {0};
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_f0_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (260 /* 26.5 */, lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (805 /* 80.5 */, lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (101400 /* 101355 */, lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (103 /* 10.2 */, lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (112 /* 11.3 */, lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (122 /* 12.4 */, lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (134 /* 13.5 */, lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (1152 /* 1129 */, lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (11, lrintf (decoded_data.voc_index));
    TEST_ASSERT_EQUAL (12, lrintf (decoded_data.nox_index));
    TEST_ASSERT_EQUAL (15322 /* 15123 */, lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (205 /* 20.5 */, lrintf (decoded_data.sound_dba_avg * 10.0f));
    TEST_ASSERT_EQUAL (m_re_f0_data_ok.flag_usb_on, decoded_data.flag_usb_on);
    TEST_ASSERT_EQUAL (m_re_f0_data_ok.flag_low_battery, decoded_data.flag_low_battery);
    TEST_ASSERT_EQUAL (m_re_f0_data_ok.flag_calibration_in_progress,
                       decoded_data.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (m_re_f0_data_ok.flag_boost_mode, decoded_data.flag_boost_mode);
    TEST_ASSERT_EQUAL (m_re_f0_data_ok.flag_seq_cnt, decoded_data.flag_seq_cnt);
    TEST_ASSERT_EQUAL (m_re_f0_data_ok.address, decoded_data.address);
}

void test_ruuvi_endpoint_f0_get_ok_max (void)
{
    static const re_f0_data_t m_re_f0_data_ok_max =
    {
        .temperature_c = 126.0f,
        .humidity_rh = 100.0f,
        .pressure_pa = 115400,
        .pm1p0_ppm = 1000.0f,
        .pm2p5_ppm = 1000.0f,
        .pm4p0_ppm = 1000.0f,
        .pm10p0_ppm = 1000.0f,
        .co2 = 40000,
        .voc_index = 500,
        .nox_index = 500,
        .luminosity = 40000,
        .sound_dba_avg = 127.0f,
        .flag_usb_on = true,
        .flag_low_battery = true,
        .flag_calibration_in_progress = true,
        .flag_boost_mode = true,
        .flag_seq_cnt = 14,
        .address = 0xCBB8334C884F,
    };
    static const uint8_t max_data[] =
    {
        0xF0,
        126, // Temperature
        200, // Humidity
        254, // Pressure
        254, // PM1.0
        254, // PM2.5
        254, // PM4.0
        254, // PM10.0
        254, // CO2
        254, // VOX
        254, // NOX
        254, // Luminosity
        254, // Sound avg
        0xEF, // Flags
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F
    };
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[20] = {0};
    _Static_assert (sizeof (max_data) == sizeof (test_buffer), "Invalid test data size");
    err_code = re_f0_encode (test_buffer, &m_re_f0_data_ok_max);
    TEST_ASSERT_EQUAL (RE_SUCCESS, err_code);
    TEST_ASSERT_EQUAL_HEX8_ARRAY (max_data, test_buffer, sizeof (max_data));
    const uint8_t raw_buf_prefix[] = {0x02, 0x01, 0x06, 0x03, 0x03, 0x98, 0xFC, 0x17, 0xFF, 0x99, 0x04};
    _Static_assert (sizeof (raw_buf_prefix) == RE_F0_OFFSET_PAYLOAD,
                    "Invalid raw_buf_prefix size");
    uint8_t raw_buf[31];
    memcpy (&raw_buf[0], raw_buf_prefix, sizeof (raw_buf_prefix));
    memcpy (&raw_buf[RE_F0_OFFSET_PAYLOAD], test_buffer, sizeof (test_buffer));
    re_f0_data_t decoded_data = {0};
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_f0_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (1260 /* 126.0 */, lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (1000 /* 100.0 */, lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (115400 /* 115400 */, lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (10000 /* 1000.0 */, lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (10000 /* 1000.0 */, lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (10000 /* 1000.0 */, lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (10000 /* 1000.0 */, lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (40000 /* 40000 */, lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (500, lrintf (decoded_data.voc_index));
    TEST_ASSERT_EQUAL (500, lrintf (decoded_data.nox_index));
    TEST_ASSERT_EQUAL (40000 /* 40000 */, lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (1270 /* 127.0 */, lrintf (decoded_data.sound_dba_avg * 10.0f));
    TEST_ASSERT_EQUAL (m_re_f0_data_ok_max.flag_usb_on, decoded_data.flag_usb_on);
    TEST_ASSERT_EQUAL (m_re_f0_data_ok_max.flag_low_battery, decoded_data.flag_low_battery);
    TEST_ASSERT_EQUAL (m_re_f0_data_ok_max.flag_calibration_in_progress,
                       decoded_data.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (m_re_f0_data_ok_max.flag_boost_mode, decoded_data.flag_boost_mode);
    TEST_ASSERT_EQUAL (m_re_f0_data_ok_max.flag_seq_cnt, decoded_data.flag_seq_cnt);
    TEST_ASSERT_EQUAL (m_re_f0_data_ok_max.address, decoded_data.address);
}

void test_ruuvi_endpoint_f0_get_ok_min (void)
{
    static const re_f0_data_t m_re_f0_data_ok_min =
    {
        .temperature_c = -126.0,
        .humidity_rh = 0,
        .pressure_pa = 90000,
        .pm1p0_ppm = 0,
        .pm2p5_ppm = 0,
        .pm4p0_ppm = 0,
        .pm10p0_ppm = 0,
        .co2 = 0,
        .voc_index = 1,
        .nox_index = 1,
        .luminosity = 0,
        .sound_dba_avg = 0,
        .flag_usb_on = false,
        .flag_low_battery = false,
        .flag_calibration_in_progress = false,
        .flag_boost_mode = false,
        .flag_seq_cnt = 0,
        .address = 0xCBB8334C884F,
    };
    static const uint8_t min_data[] =
    {
        0xF0,
        -126, // Temperature
        0, // Humidity
        0, // Pressure
        0, // PM1.0
        0, // PM2.5
        0, // PM4.0
        0, // PM10.0
        0, // CO2
        0, // VOX
        0, // NOX
        0, // Luminosity
        0, // Sound avg
        0x00, // Flags
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F
    };
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[20] = {0};
    _Static_assert (sizeof (min_data) == sizeof (test_buffer), "Invalid test data size");
    err_code = re_f0_encode (test_buffer, &m_re_f0_data_ok_min);
    TEST_ASSERT_EQUAL (RE_SUCCESS, err_code);
    TEST_ASSERT_EQUAL_HEX8_ARRAY (min_data, test_buffer, sizeof (min_data));
    const uint8_t raw_buf_prefix[] = {0x02, 0x01, 0x06, 0x03, 0x03, 0x98, 0xFC, 0x17, 0xFF, 0x99, 0x04};
    _Static_assert (sizeof (raw_buf_prefix) == RE_F0_OFFSET_PAYLOAD,
                    "Invalid raw_buf_prefix size");
    uint8_t raw_buf[31];
    memcpy (&raw_buf[0], raw_buf_prefix, sizeof (raw_buf_prefix));
    memcpy (&raw_buf[RE_F0_OFFSET_PAYLOAD], test_buffer, sizeof (test_buffer));
    re_f0_data_t decoded_data = {0};
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_f0_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (-1260 /* -126.0 */, lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (0 /* 0.0 */, lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (90000 /* 90000 */, lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (0 /* 0.0 */, lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (0 /* 0.0 */, lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (0 /* 0.0 */, lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (0 /* 0.0 */, lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (0 /* 0 */, lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (1, lrintf (decoded_data.voc_index));
    TEST_ASSERT_EQUAL (1, lrintf (decoded_data.nox_index));
    TEST_ASSERT_EQUAL (0 /* 0 */, lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (0 /* 0.0 */, lrintf (decoded_data.sound_dba_avg * 10.0f));
    TEST_ASSERT_EQUAL (m_re_f0_data_ok_min.flag_usb_on, decoded_data.flag_usb_on);
    TEST_ASSERT_EQUAL (m_re_f0_data_ok_min.flag_low_battery, decoded_data.flag_low_battery);
    TEST_ASSERT_EQUAL (m_re_f0_data_ok_min.flag_calibration_in_progress,
                       decoded_data.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (m_re_f0_data_ok_min.flag_boost_mode, decoded_data.flag_boost_mode);
    TEST_ASSERT_EQUAL (m_re_f0_data_ok_min.flag_seq_cnt, decoded_data.flag_seq_cnt);
    TEST_ASSERT_EQUAL (m_re_f0_data_ok_min.address, decoded_data.address);
}

/**
 * @brief Null buffer
 *
 * @retval RE_ERROR_NULL on success test.
 */
void test_ruuvi_endpoint_f0_get_error_null_buffer (void)
{
    static const re_f0_data_t m_re_f0_data_ok =
    {
        .temperature_c = 26.5f,
        .humidity_rh = 80.5f,
        .pressure_pa = 101355,
        .pm1p0_ppm = 10.2f,
        .pm2p5_ppm = 11.3f,
        .pm4p0_ppm = 12.4f,
        .pm10p0_ppm = 13.5f,
        .co2 = 1129,
        .voc_index = 11,
        .nox_index = 12,
        .luminosity = 15123,
        .sound_dba_avg = 20.5f,
        .flag_usb_on = true,
        .flag_low_battery = false,
        .flag_calibration_in_progress = false,
        .flag_boost_mode = false,
        .flag_seq_cnt = 5,
        .address = 0xCBB8334C884F,
    };
    re_status_t err_code = RE_ERROR_NULL;
    uint8_t * const p_test_buffer = NULL;
    err_code = re_f0_encode (p_test_buffer, &m_re_f0_data_ok);
    TEST_ASSERT (RE_ERROR_NULL == err_code);
}

/**
 * @brief Null data
 *
 * @retval RE_ERROR_NULL on success test.
 */
void test_ruuvi_endpoint_f0_get_error_null_data (void)
{
    re_status_t err_code = RE_ERROR_NULL;
    uint8_t test_buffer[20] = {0};
    const re_f0_data_t * p_re_f0_data = NULL;
    err_code = re_f0_encode ( (uint8_t * const) &test_buffer, p_re_f0_data);
    TEST_ASSERT (RE_ERROR_NULL == err_code);
}

/**
 * @brief True to test encode operation with invalid data
 *
 * @retval RD_SUCCESS on success test.
 */
void test_ruuvi_endpoint_f0_get_invalid_data (void)
{
    static const re_f0_data_t m_re_f0_data_invalid =
    {
        .temperature_c = NAN,
        .humidity_rh = NAN,
        .pressure_pa = NAN,
        .pm1p0_ppm = NAN,
        .pm2p5_ppm = NAN,
        .pm4p0_ppm = NAN,
        .pm10p0_ppm = NAN,
        .co2 = NAN,
        .voc_index = NAN,
        .nox_index = NAN,
        .luminosity = NAN,
        .sound_dba_avg = NAN,
        .flag_usb_on = false,
        .flag_low_battery = false,
        .flag_calibration_in_progress = false,
        .flag_boost_mode = false,
        .flag_seq_cnt = 0,
        .address = 0xFFFFFFFFFFFF,
    };
    static const uint8_t invalid_data[] =
    {
        0xF0,
        0x80, // Temperature
        0xFF, // Humidity
        0xFF, // Pressure
        0xFF, // PM1.0
        0xFF, // PM2.5
        0xFF, // PM4.0
        0xFF, // PM10.0
        0xFF, // CO2
        0xFF, // VOX
        0xFF, // NOX
        0xFF, // Luminosity
        0xFF, // Sound avg
        0x00, // Flags
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[20] = {0};
    _Static_assert (sizeof (invalid_data) == sizeof (test_buffer), "Invalid test data size");
    err_code = re_f0_encode ( (uint8_t * const) &test_buffer,
                              (const re_f0_data_t *) &m_re_f0_data_invalid);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT_EQUAL_HEX8_ARRAY (invalid_data, test_buffer, sizeof (invalid_data));
    const uint8_t raw_buf_prefix[] = {0x02, 0x01, 0x06, 0x03, 0x03, 0x98, 0xFC, 0x17, 0xFF, 0x99, 0x04};
    _Static_assert (sizeof (raw_buf_prefix) == RE_F0_OFFSET_PAYLOAD,
                    "Invalid raw_buf_prefix size");
    uint8_t raw_buf[31];
    memcpy (&raw_buf[0], raw_buf_prefix, sizeof (raw_buf_prefix));
    memcpy (&raw_buf[RE_F0_OFFSET_PAYLOAD], test_buffer, sizeof (test_buffer));
    re_f0_data_t decoded_data = {0};
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_f0_decode (raw_buf, &decoded_data));
    TEST_ASSERT (isnan (decoded_data.temperature_c));
    TEST_ASSERT (isnan (decoded_data.humidity_rh));
    TEST_ASSERT (isnan (decoded_data.pressure_pa));
    TEST_ASSERT (isnan (decoded_data.pm1p0_ppm));
    TEST_ASSERT (isnan (decoded_data.pm1p0_ppm));
    TEST_ASSERT (isnan (decoded_data.pm1p0_ppm));
    TEST_ASSERT (isnan (decoded_data.pm1p0_ppm));
    TEST_ASSERT (isnan (decoded_data.co2));
    TEST_ASSERT (isnan (decoded_data.voc_index));
    TEST_ASSERT (isnan (decoded_data.nox_index));
    TEST_ASSERT (isnan (decoded_data.luminosity));
    TEST_ASSERT (isnan (decoded_data.sound_dba_avg));
    TEST_ASSERT_EQUAL (m_re_f0_data_invalid.flag_usb_on, decoded_data.flag_usb_on);
    TEST_ASSERT_EQUAL (m_re_f0_data_invalid.flag_low_battery, decoded_data.flag_low_battery);
    TEST_ASSERT_EQUAL (m_re_f0_data_invalid.flag_calibration_in_progress,
                       decoded_data.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (m_re_f0_data_invalid.flag_boost_mode, decoded_data.flag_boost_mode);
    TEST_ASSERT_EQUAL (m_re_f0_data_invalid.flag_seq_cnt, decoded_data.flag_seq_cnt);
    TEST_ASSERT_EQUAL (m_re_f0_data_invalid.address, decoded_data.address);
}

void test_ruuvi_endpoint_f0_underflow (void)
{
    static const re_f0_data_t m_re_f0_data_underflow =
    {
        .temperature_c = -126.1,
        .humidity_rh = -1,
        .pressure_pa = 89999,
        .pm1p0_ppm = -1,
        .pm2p5_ppm = -1,
        .pm4p0_ppm = -1,
        .pm10p0_ppm = -1,
        .co2 = -1,
        .voc_index = 0,
        .nox_index = 0,
        .luminosity = -1,
        .sound_dba_avg = -1,
        .flag_usb_on = false,
        .flag_low_battery = false,
        .flag_calibration_in_progress = false,
        .flag_boost_mode = false,
        .flag_seq_cnt = 0,
        .address = 0xCBB8334C884F,
    };
    static const uint8_t min_data[] =
    {
        0xF0,
        -126, // Temperature
        0, // Humidity
        0, // Pressure
        0, // PM1.0
        0, // PM2.5
        0, // PM4.0
        0, // PM10.0
        0, // CO2
        0xFF, // VOX
        0xFF, // NOX
        0, // Luminosity
        0, // Sound avg
        0x00, // Flags
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F
    };
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[20] = {0};
    err_code = re_f0_encode (test_buffer, &m_re_f0_data_underflow);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT_EQUAL_HEX8_ARRAY (min_data, test_buffer, sizeof (min_data));
}

void test_ruuvi_endpoint_f0_overflow (void)
{
    static const re_f0_data_t m_re_f0_data_overflow =
    {
        .temperature_c = 126.1,
        .humidity_rh = 100.1,
        .pressure_pa = 115401,
        .pm1p0_ppm = 1000.1,
        .pm2p5_ppm = 1000.1,
        .pm4p0_ppm = 1000.1,
        .pm10p0_ppm = 1000.1,
        .co2 = 40001,
        .voc_index = 501,
        .nox_index = 501,
        .luminosity = 40001,
        .sound_dba_avg = 127.1f,
        .flag_usb_on = true,
        .flag_low_battery = true,
        .flag_calibration_in_progress = true,
        .flag_boost_mode = true,
        .flag_seq_cnt = 14,
        .address = 0xCBB8334C884F,
    };
    static const uint8_t max_data[] =
    {
        0xF0,
        126, // Temperature
        200, // Humidity
        254, // Pressure
        254, // PM1.0
        254, // PM2.5
        254, // PM4.0
        254, // PM10.0
        254, // CO2
        0xFF, // VOX
        0xFF, // NOX
        254, // Luminosity
        254, // Sound avg
        0xEF, // Flags
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F
    };
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[20] = {0};
    err_code = re_f0_encode (test_buffer, &m_re_f0_data_overflow);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT_EQUAL_HEX8_ARRAY (max_data, test_buffer, sizeof (max_data));
}

void test_ruuvi_endpoint_f0_check_format_ok (void)
{
    const uint8_t raw_buf_prefix[] = {0x02, 0x01, 0x06, 0x03, 0x03, 0x98, 0xFC, 0x17, 0xFF, 0x99, 0x04};
    _Static_assert (sizeof (raw_buf_prefix) == RE_F0_OFFSET_PAYLOAD,
                    "Invalid raw_buf_prefix size");
    uint8_t raw_buf[31];
    memcpy (&raw_buf[0], raw_buf_prefix, sizeof (raw_buf_prefix));
    const uint8_t test_buffer[] = { 0xF0 };
    memcpy (&raw_buf[RE_F0_OFFSET_PAYLOAD], test_buffer, sizeof (test_buffer));
    TEST_ASSERT_TRUE (re_f0_check_format (raw_buf));
}

void test_ruuvi_endpoint_f0_check_format_fail (void)
{
    const uint8_t raw_buf_payload_format_F1[31] = {0x02, 0x01, 0x06, 0x03, 0x03, 0x98, 0xFC, 0x17, 0xFF, 0x99, 0x04, 0xF1};
    TEST_ASSERT_FALSE (re_f0_check_format (raw_buf_payload_format_F1));
    const uint8_t raw_buf_manufacturer_id_1[31] = {0x02, 0x01, 0x06, 0x03, 0x03, 0x98, 0xFC, 0x17, 0xFF, 0x99, 0x05, 0xF1};
    TEST_ASSERT_FALSE (re_f0_check_format (raw_buf_manufacturer_id_1));
    const uint8_t raw_buf_manufacturer_id_2[31] = {0x02, 0x01, 0x06, 0x03, 0x03, 0x98, 0xFC, 0x17, 0xFF, 0x9a, 0x04, 0xF1};
    TEST_ASSERT_FALSE (re_f0_check_format (raw_buf_manufacturer_id_2));
    const uint8_t raw_buf_payload_format_type[31] = {0x02, 0x01, 0x06, 0x03, 0x03, 0x98, 0xFC, 0x17, 0xFE, 0x99, 0x04, 0xF0};
    TEST_ASSERT_FALSE (re_f0_check_format (raw_buf_payload_format_type));
    const uint8_t raw_buf_payload_length[31] = {0x02, 0x01, 0x06, 0x03, 0x03, 0x98, 0xFC, 0x16, 0xFF, 0x99, 0x04, 0xF0};
    TEST_ASSERT_FALSE (re_f0_check_format (raw_buf_payload_length));
    const uint8_t raw_buf_uuid_1[31] = {0x02, 0x01, 0x06, 0x03, 0x03, 0x98, 0xFD, 0x17, 0xFF, 0x99, 0x04, 0xF0};
    TEST_ASSERT_FALSE (re_f0_check_format (raw_buf_uuid_1));
    const uint8_t raw_buf_uuid_2[31] = {0x02, 0x01, 0x06, 0x03, 0x03, 0x99, 0xFC, 0x17, 0xFF, 0x99, 0x04, 0xF0};
    TEST_ASSERT_FALSE (re_f0_check_format (raw_buf_uuid_2));
    const uint8_t raw_buf_uuid_type[31] = {0x02, 0x01, 0x06, 0x03, 0x02, 0x98, 0xFC, 0x17, 0xFF, 0x99, 0x04, 0xF0};
    TEST_ASSERT_FALSE (re_f0_check_format (raw_buf_uuid_type));
    const uint8_t raw_buf_uuid_len[31] = {0x02, 0x01, 0x06, 0x04, 0x03, 0x98, 0xFC, 0x17, 0xFF, 0x99, 0x04, 0xF0};
    TEST_ASSERT_FALSE (re_f0_check_format (raw_buf_uuid_len));
    const uint8_t raw_buf_flags_type[31] = {0x02, 0x02, 0x06, 0x03, 0x03, 0x98, 0xFC, 0x17, 0xFF, 0x99, 0x04, 0xF0};
    TEST_ASSERT_FALSE (re_f0_check_format (raw_buf_flags_type));
    const uint8_t raw_buf_flags_len[31] = {0x03, 0x01, 0x06, 0x03, 0x03, 0x98, 0xFC, 0x17, 0xFF, 0x99, 0x04, 0xF0};
    TEST_ASSERT_FALSE (re_f0_check_format (raw_buf_flags_len));
    const uint8_t raw_buf_format_6[31] = {0x02, 0x01, 0x04, 0x1B, 0xFF, 0x99, 0x04, 0x06};
    TEST_ASSERT_FALSE (re_f0_check_format (raw_buf_format_6));
}

void test_re_f0_data_invalid (void)
{
    const uint16_t measurement_cnt = 1;
    const uint64_t radio_mac = 0xCBB8334C884FULL;
    const re_f0_data_t data = re_f0_data_invalid (measurement_cnt, radio_mac);
    TEST_ASSERT_TRUE (isnan (data.temperature_c));
    TEST_ASSERT (isnan (data.pressure_pa));
    TEST_ASSERT (isnan (data.pm1p0_ppm));
    TEST_ASSERT (isnan (data.pm1p0_ppm));
    TEST_ASSERT (isnan (data.pm1p0_ppm));
    TEST_ASSERT (isnan (data.pm1p0_ppm));
    TEST_ASSERT (isnan (data.co2));
    TEST_ASSERT (isnan (data.voc_index));
    TEST_ASSERT (isnan (data.nox_index));
    TEST_ASSERT (isnan (data.luminosity));
    TEST_ASSERT (isnan (data.sound_dba_avg));
    TEST_ASSERT_EQUAL (false, data.flag_usb_on);
    TEST_ASSERT_EQUAL (false, data.flag_low_battery);
    TEST_ASSERT_EQUAL (false, data.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (false, data.flag_boost_mode);
    TEST_ASSERT_EQUAL (measurement_cnt, data.flag_seq_cnt);
    TEST_ASSERT_EQUAL (radio_mac, data.address);
}
