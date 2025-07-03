#include "unity.h"

#include "ruuvi_endpoint_e0.h"
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
void test_ruuvi_endpoint_e0_get_ok (void)
{
    static const re_e0_data_t m_re_e0_data_ok =
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
        .sound_avg_dba = 20.5f,
        .sound_peak_spl_db = 21.0f,
        .measurement_count = 65533,
        .voltage = 3.2f,
        .flag_usb_on = true,
        .flag_low_battery = false,
        .flag_calibration_in_progress = false,
        .flag_boost_mode = false,
        .address = 0xCBB8334C884F,
    };
    static const uint8_t valid_data[] =
    {
        0xE0,
        5300U >> 8U, 5300U & 0xFFU, // Temperature
              32200U >> 8U, 32200U & 0xFFU, // Humidity
              51355U >> 8U, 51355U & 0xFFU, // Pressure
              102U >> 8U, 102U & 0xFFU, // PM1.0
              113U >> 8U, 113U & 0xFFU, // PM2.5
              124U >> 8U, 124U & 0xFFU, // PM4.0
              135U >> 8U, 135U & 0xFFU, // PM10.0
              1129U >> 8U, 1129U & 0xFFU, // CO2
              11U >> 8U, 11U & 0xFFU, // VOX
              12U >> 8U, 12U & 0xFFU, // NOX
              15123U >> 8U, 15123U & 0xFFU, // Luminosity
              41, // Sound avg
              42, // Sound peak
              65533U >> 8U, 65533U & 0xFFU, // Measurement count
              107, // Voltage
              0x01, // Flags
              0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
              0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F
    };
    _Static_assert (sizeof (valid_data) == RE_E0_DATA_LENGTH);
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[40] = {0};
    _Static_assert (sizeof (valid_data) == sizeof (test_buffer), "Invalid test data size");
    err_code = re_e0_encode (test_buffer, &m_re_e0_data_ok);
    TEST_ASSERT_EQUAL (RE_SUCCESS, err_code);
    TEST_ASSERT_EQUAL_HEX8_ARRAY (valid_data, test_buffer, sizeof (valid_data));
    const uint8_t raw_buf_prefix[] = {0x2B, 0xFF, 0x99, 0x04};
    _Static_assert (sizeof (raw_buf_prefix) == RE_E0_OFFSET_PAYLOAD,
                    "Invalid raw_buf_prefix size");
    uint8_t raw_buf[48];
    memcpy (&raw_buf[0], raw_buf_prefix, sizeof (raw_buf_prefix));
    memcpy (&raw_buf[RE_E0_OFFSET_PAYLOAD], test_buffer, sizeof (test_buffer));
    re_e0_data_t decoded_data = {0};
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e0_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok.voc_index), lrintf (decoded_data.voc_index));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok.nox_index), lrintf (decoded_data.nox_index));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok.luminosity), lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (m_re_e0_data_ok.measurement_count, decoded_data.measurement_count);
    TEST_ASSERT_EQUAL (321 /* 3.20 */, lrintf (decoded_data.voltage * 100.0f));
    TEST_ASSERT_EQUAL (m_re_e0_data_ok.flag_usb_on, decoded_data.flag_usb_on);
    TEST_ASSERT_EQUAL (m_re_e0_data_ok.flag_low_battery, decoded_data.flag_low_battery);
    TEST_ASSERT_EQUAL (m_re_e0_data_ok.flag_calibration_in_progress,
                       decoded_data.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (m_re_e0_data_ok.flag_boost_mode, decoded_data.flag_boost_mode);
    TEST_ASSERT_EQUAL (m_re_e0_data_ok.address, decoded_data.address);
}

void test_ruuvi_endpoint_e0_get_ok_max (void)
{
    static const re_e0_data_t m_re_e0_data_ok_max =
    {
        .temperature_c = 126.0f,
        .humidity_rh = 100.0f,
        .pressure_pa = 115534,
        .pm1p0_ppm = 1000.0f,
        .pm2p5_ppm = 1000.0f,
        .pm4p0_ppm = 1000.0f,
        .pm10p0_ppm = 1000.0f,
        .co2 = 40000,
        .voc_index = 500,
        .nox_index = 500,
        .luminosity = 65534,
        .sound_avg_dba = 127.0f,
        .sound_peak_spl_db = 127.0f,
        .measurement_count = 65534,
        .voltage = 7.62f,
        .flag_usb_on = true,
        .flag_low_battery = true,
        .flag_calibration_in_progress = true,
        .flag_boost_mode = true,
        .address = 0xCBB8334C884F,
    };
    static const uint8_t max_data[] =
    {
        0xE0,
        25200U >> 8U, 25200U & 0xFFU, // Temperature
               40000U >> 8U, 40000U & 0xFFU, // Humidity
               65534U >> 8U, 65534U & 0xFFU, // Pressure
               10000U >> 8U, 10000U & 0xFFU, // PM1.0
               10000U >> 8U, 10000U & 0xFFU, // PM2.5
               10000U >> 8U, 10000U & 0xFFU, // PM4.0
               10000U >> 8U, 10000U & 0xFFU, // PM10.0
               40000U >> 8U, 40000U & 0xFFU, // CO2
               500U >> 8U, 500U & 0xFFU, // VOX
               500U >> 8U, 500U & 0xFFU, // NOX
               65534U >> 8U, 65534U & 0xFFU, // Luminosity
               254U, // Sound avg
               254U, // Sound peak
               65534U >> 8U, 65534U & 0xFFU, // Measurement count
               254, // Voltage
               0x0F, // Flags
               0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
               0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F
    };
    _Static_assert (sizeof (max_data) == RE_E0_DATA_LENGTH);
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[40] = {0};
    _Static_assert (sizeof (max_data) == sizeof (test_buffer), "Invalid test data size");
    err_code = re_e0_encode ( (uint8_t * const) &test_buffer, &m_re_e0_data_ok_max);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT_EQUAL_HEX8_ARRAY (max_data, test_buffer, sizeof (max_data));
    const uint8_t raw_buf_prefix[] = {0x2B, 0xFF, 0x99, 0x04};
    _Static_assert (sizeof (raw_buf_prefix) == RE_E0_OFFSET_PAYLOAD,
                    "Invalid raw_buf_prefix size");
    uint8_t raw_buf[48];
    memcpy (&raw_buf[0], raw_buf_prefix, sizeof (raw_buf_prefix));
    memcpy (&raw_buf[RE_E0_OFFSET_PAYLOAD], test_buffer, sizeof (test_buffer));
    re_e0_data_t decoded_data = {0};
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e0_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_max.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_max.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_max.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_max.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_max.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_max.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_max.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_max.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_max.voc_index),
                       lrintf (decoded_data.voc_index));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_max.nox_index),
                       lrintf (decoded_data.nox_index));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_max.luminosity),
                       lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_max.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_max.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (m_re_e0_data_ok_max.measurement_count, decoded_data.measurement_count);
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_max.voltage * 100.0f),
                       lrintf (decoded_data.voltage * 100.0f));
    TEST_ASSERT_EQUAL (m_re_e0_data_ok_max.flag_usb_on, decoded_data.flag_usb_on);
    TEST_ASSERT_EQUAL (m_re_e0_data_ok_max.flag_low_battery, decoded_data.flag_low_battery);
    TEST_ASSERT_EQUAL (m_re_e0_data_ok_max.flag_calibration_in_progress,
                       decoded_data.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (m_re_e0_data_ok_max.flag_boost_mode, decoded_data.flag_boost_mode);
    TEST_ASSERT_EQUAL (m_re_e0_data_ok_max.address, decoded_data.address);
}

void test_ruuvi_endpoint_e0_get_ok_min (void)
{
    static const re_e0_data_t m_re_e0_data_ok_min =
    {
        .temperature_c = -126.0,
        .humidity_rh = 0,
        .pressure_pa = 50000,
        .pm1p0_ppm = 0,
        .pm2p5_ppm = 0,
        .pm4p0_ppm = 0,
        .pm10p0_ppm = 0,
        .co2 = 0,
        .voc_index = 1,
        .nox_index = 1,
        .luminosity = 0,
        .sound_avg_dba = 0,
        .sound_peak_spl_db = 0,
        .measurement_count = 0,
        .voltage = 0.0f,
        .flag_usb_on = false,
        .flag_low_battery = false,
        .flag_calibration_in_progress = false,
        .flag_boost_mode = false,
        .address = 0xCBB8334C884F,
    };
    static const uint8_t min_data[] =
    {
        0xE0,
        40336U >> 8U, 40336U & 0xFFU, // Temperature
               0U >> 8U, 0U & 0xFFU, // Humidity
               0U >> 8U, 0U & 0xFFU, // Pressure
               0U >> 8U, 0U & 0xFFU, // PM1.0
               0U >> 8U, 0U & 0xFFU, // PM2.5
               0U >> 8U, 0U & 0xFFU, // PM4.0
               0U >> 8U, 0U & 0xFFU, // PM10.0
               0U >> 8U, 0U & 0xFFU, // CO2
               1U >> 8U, 1U & 0xFFU, // VOX
               1U >> 8U, 1U & 0xFFU, // NOX
               0U >> 8U, 0U & 0xFFU, // Luminosity
               0U, // Sound avg
               0U, // Sound peak
               0U >> 8U, 0U & 0xFFU, // Measurement count
               0, // Voltage
               0x00, // Flags
               0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
               0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F
    };
    _Static_assert (sizeof (min_data) == RE_E0_DATA_LENGTH);
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[40] = {0};
    _Static_assert (sizeof (min_data) == sizeof (test_buffer), "Invalid test data size");
    err_code = re_e0_encode ( (uint8_t * const) &test_buffer, &m_re_e0_data_ok_min);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT_EQUAL_HEX8_ARRAY (min_data, test_buffer, sizeof (min_data));
    uint8_t raw_buf[48];
    const uint8_t raw_buf_prefix[] = {0x2B, 0xFF, 0x99, 0x04};
    _Static_assert (sizeof (raw_buf_prefix) == RE_E0_OFFSET_PAYLOAD,
                    "Invalid raw_buf_prefix size");
    memcpy (&raw_buf[0], raw_buf_prefix, sizeof (raw_buf_prefix));
    memcpy (&raw_buf[RE_E0_OFFSET_PAYLOAD], test_buffer, sizeof (test_buffer));
    re_e0_data_t decoded_data = {0};
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e0_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_min.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_min.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_min.pressure_pa * 10.0f),
                       lrintf (decoded_data.pressure_pa * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_min.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_min.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_min.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_min.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_min.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_min.voc_index),
                       lrintf (decoded_data.voc_index));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_min.nox_index),
                       lrintf (decoded_data.nox_index));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_min.luminosity),
                       lrintf (decoded_data.luminosity));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_min.sound_avg_dba * 10.0f),
                       lrintf (decoded_data.sound_avg_dba * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_e0_data_ok_min.sound_peak_spl_db * 10.0f),
                       lrintf (decoded_data.sound_peak_spl_db * 10.0f));
    TEST_ASSERT_EQUAL (m_re_e0_data_ok_min.measurement_count, decoded_data.measurement_count);
    TEST_ASSERT_EQUAL (0.0f, lrintf (decoded_data.voltage * 100.0f));
    TEST_ASSERT_EQUAL (m_re_e0_data_ok_min.flag_usb_on, decoded_data.flag_usb_on);
    TEST_ASSERT_EQUAL (m_re_e0_data_ok_min.flag_low_battery, decoded_data.flag_low_battery);
    TEST_ASSERT_EQUAL (m_re_e0_data_ok_min.flag_calibration_in_progress,
                       decoded_data.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (m_re_e0_data_ok_min.flag_boost_mode, decoded_data.flag_boost_mode);
    TEST_ASSERT_EQUAL (m_re_e0_data_ok_min.address, decoded_data.address);
}

/**
 * @brief Null buffer
 *
 * @retval RE_ERROR_NULL on success test.
 */
void test_ruuvi_endpoint_e0_get_error_null_buffer (void)
{
    static const re_e0_data_t m_re_e0_data_ok =
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
        .sound_avg_dba = 20.5f,
        .sound_peak_spl_db = 21.0f,
        .measurement_count = 65533,
        .voltage = 3.2f,
        .flag_usb_on = true,
        .flag_low_battery = false,
        .flag_calibration_in_progress = false,
        .flag_boost_mode = false,
        .address = 0xCBB8334C884F,
    };
    re_status_t err_code = RE_ERROR_NULL;
    uint8_t * const p_test_buffer = NULL;
    err_code = re_e0_encode (p_test_buffer, &m_re_e0_data_ok);
    TEST_ASSERT (RE_ERROR_NULL == err_code);
}

/**
 * @brief Null data
 *
 * @retval RE_ERROR_NULL on success test.
 */
void test_ruuvi_endpoint_e0_get_error_null_data (void)
{
    re_status_t err_code = RE_ERROR_NULL;
    uint8_t test_buffer[40] = {0};
    const re_e0_data_t * p_re_e0_data = NULL;
    err_code = re_e0_encode ( (uint8_t * const) &test_buffer, p_re_e0_data);
    TEST_ASSERT (RE_ERROR_NULL == err_code);
}

/**
 * @brief True to test encode operation with invalid data
 *
 * @retval RD_SUCCESS on success test.
 */
void test_ruuvi_endpoint_e0_get_invalid_data (void)
{
    static const re_e0_data_t m_re_e0_data_invalid =
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
        .sound_avg_dba = NAN,
        .sound_peak_spl_db = NAN,
        .measurement_count = RE_E0_INVALID_SEQUENCE,
        .voltage = NAN,
        .flag_usb_on = false,
        .flag_low_battery = false,
        .flag_calibration_in_progress = false,
        .flag_boost_mode = false,
        .address = 0xFFFFFFFFFFFF,
    };
    static const uint8_t invalid_data[] =
    {
        0xE0,
        0x8000U >> 8U, 0x8000U & 0xFFU, // Temperature
                0xFFFFU >> 8U, 0xFFFFU & 0xFFU, // Humidity
                0xFFFFU >> 8U, 0xFFFFU & 0xFFU, // Pressure
                0xFFFFU >> 8U, 0xFFFFU & 0xFFU, // PM1.0
                0xFFFFU >> 8U, 0xFFFFU & 0xFFU, // PM2.5
                0xFFFFU >> 8U, 0xFFFFU & 0xFFU, // PM4.0
                0xFFFFU >> 8U, 0xFFFFU & 0xFFU, // PM10.0
                0xFFFFU >> 8U, 0xFFFFU & 0xFFU, // CO2
                511U >> 8U, 511U & 0xFFU, // VOX
                511U >> 8U, 511U & 0xFFU, // NOX
                0xFFFFU >> 8U, 0xFFFFU & 0xFFU, // Luminosity
                0xFFU, // Sound avg
                0xFFU, // Sound peak
                0xFFFFU >> 8U, 0xFFFFU & 0xFFU, // Measurement count
                0xFFU, // Voltage
                0x00, // Flags
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };
    _Static_assert (sizeof (invalid_data) == RE_E0_DATA_LENGTH);
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[40] = {0};
    err_code = re_e0_encode ( (uint8_t * const) &test_buffer, &m_re_e0_data_invalid);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT_EQUAL_HEX8_ARRAY (invalid_data, test_buffer, sizeof (invalid_data));
    const uint8_t raw_buf_prefix[] = {0x2B, 0xFF, 0x99, 0x04};
    _Static_assert (sizeof (raw_buf_prefix) == RE_E0_OFFSET_PAYLOAD,
                    "Invalid raw_buf_prefix size");
    uint8_t raw_buf[48];
    memcpy (&raw_buf[0], raw_buf_prefix, sizeof (raw_buf_prefix));
    memcpy (&raw_buf[RE_E0_OFFSET_PAYLOAD], test_buffer, sizeof (test_buffer));
    re_e0_data_t decoded_data = {0};
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_e0_decode (raw_buf, &decoded_data));
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
    TEST_ASSERT (isnan (decoded_data.sound_avg_dba));
    TEST_ASSERT (isnan (decoded_data.sound_peak_spl_db));
    TEST_ASSERT_EQUAL (m_re_e0_data_invalid.measurement_count,
                       decoded_data.measurement_count);
    TEST_ASSERT (isnan (decoded_data.voltage));
    TEST_ASSERT_EQUAL (m_re_e0_data_invalid.flag_usb_on, decoded_data.flag_usb_on);
    TEST_ASSERT_EQUAL (m_re_e0_data_invalid.flag_low_battery, decoded_data.flag_low_battery);
    TEST_ASSERT_EQUAL (m_re_e0_data_invalid.flag_calibration_in_progress,
                       decoded_data.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (m_re_e0_data_invalid.flag_boost_mode, decoded_data.flag_boost_mode);
    TEST_ASSERT_EQUAL (m_re_e0_data_invalid.address, decoded_data.address);
}

void test_ruuvi_endpoint_e0_underflow (void)
{
    static const re_e0_data_t m_re_fe_data_underflow =
    {
        .temperature_c = -126.1,
        .humidity_rh = -1,
        .pressure_pa = 49999,
        .pm1p0_ppm = -1,
        .pm2p5_ppm = -1,
        .pm4p0_ppm = -1,
        .pm10p0_ppm = -1,
        .co2 = -1,
        .voc_index = 0,
        .nox_index = 0,
        .luminosity = -1,
        .sound_avg_dba = -1,
        .sound_peak_spl_db = -1,
        .measurement_count = 0,
        .voltage = -0.1f,
        .flag_usb_on = false,
        .flag_low_battery = false,
        .flag_calibration_in_progress = false,
        .flag_boost_mode = false,
        .address = 0xCBB8334C884F,
    };
    static const uint8_t min_data[] =
    {
        0xE0,
        40336U >> 8U, 40336U & 0xFFU, // Temperature
               0U >> 8U, 0U & 0xFFU, // Humidity
               0U >> 8U, 0U & 0xFFU, // Pressure
               0U >> 8U, 0U & 0xFFU, // PM1.0
               0U >> 8U, 0U & 0xFFU, // PM2.5
               0U >> 8U, 0U & 0xFFU, // PM4.0
               0U >> 8U, 0U & 0xFFU, // PM10.0
               0U >> 8U, 0U & 0xFFU, // CO2
               511U >> 8U, 511U & 0xFFU, // VOX
               511U >> 8U, 511U & 0xFFU, // NOX
               0U >> 8U, 0U & 0xFFU, // Luminosity
               0U, // Sound avg
               0U, // Sound peak
               0U >> 8U, 0U & 0xFFU, // Measurement count
               0, // Voltage
               0x00, // Flags
               0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
               0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F
    };
    _Static_assert (sizeof (min_data) == RE_E0_DATA_LENGTH);
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[40] = {0};
    err_code = re_e0_encode (test_buffer, &m_re_fe_data_underflow);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT_EQUAL_HEX8_ARRAY (min_data, test_buffer, sizeof (min_data));
}

void test_ruuvi_endpoint_e0_overflow (void)
{
    static const re_e0_data_t m_re_fe_data_overflow =
    {
        .temperature_c = 126.1,
        .humidity_rh = 100.1,
        .pressure_pa = 115535,
        .pm1p0_ppm = 1000.1,
        .pm2p5_ppm = 1000.1,
        .pm4p0_ppm = 1000.1,
        .pm10p0_ppm = 1000.1,
        .co2 = 40001,
        .voc_index = 501,
        .nox_index = 501,
        .luminosity = 65535,
        .sound_avg_dba = 127.1f,
        .sound_peak_spl_db = 127.1f,
        .measurement_count = 65534,
        .voltage = 7.63f,
        .flag_usb_on = true,
        .flag_low_battery = true,
        .flag_calibration_in_progress = true,
        .flag_boost_mode = true,
        .address = 0xCBB8334C884F,
    };
    static const uint8_t max_data[] =
    {
        0xE0,
        25200U >> 8U, 25200U & 0xFFU, // Temperature
               40000U >> 8U, 40000U & 0xFFU, // Humidity
               65534U >> 8U, 65534U & 0xFFU, // Pressure
               10000U >> 8U, 10000U & 0xFFU, // PM1.0
               10000U >> 8U, 10000U & 0xFFU, // PM2.5
               10000U >> 8U, 10000U & 0xFFU, // PM4.0
               10000U >> 8U, 10000U & 0xFFU, // PM10.0
               40000U >> 8U, 40000U & 0xFFU, // CO2
               511U >> 8U, 511U & 0xFFU, // VOX
               511U >> 8U, 511U & 0xFFU, // NOX
               65534U >> 8U, 65534U & 0xFFU, // Luminosity
               254U, // Sound avg
               254U, // Sound peak
               65534U >> 8U, 65534U & 0xFFU, // Measurement count
               254, // Voltage
               0x0F, // Flags
               0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
               0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F
    };
    _Static_assert (sizeof (max_data) == RE_E0_DATA_LENGTH);
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[40] = {0};
    err_code = re_e0_encode (test_buffer, &m_re_fe_data_overflow);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT_EQUAL_HEX8_ARRAY (max_data, test_buffer, sizeof (max_data));
}

void test_ruuvi_endpoint_e0_check_format_ok (void)
{
    const uint8_t raw_buf[48] = {0x2B, 0xFF, 0x99, 0x04, 0xE0};
    TEST_ASSERT_TRUE (re_e0_check_format (raw_buf));
}

void test_ruuvi_endpoint_e0_check_format_fail (void)
{
    const uint8_t raw_buf_payload_format_E1[48] = {0x02, 0x01, 0x04, 0x1B, 0xFF, 0x99, 0x04, 0xE1};
    TEST_ASSERT_FALSE (re_e0_check_format (raw_buf_payload_format_E1));
    const uint8_t raw_buf_manufacturer_id_1[48] = {0x02, 0x01, 0x04, 0x1B, 0xFF, 0x99, 0x05, 0xE0};
    TEST_ASSERT_FALSE (re_e0_check_format (raw_buf_manufacturer_id_1));
    const uint8_t raw_buf_manufacturer_id_2[48] = {0x02, 0x01, 0x04, 0x1B, 0xFF, 0x9a, 0x04, 0xE0};
    TEST_ASSERT_FALSE (re_e0_check_format (raw_buf_manufacturer_id_2));
    const uint8_t raw_buf_type[48] = {0x02, 0x01, 0x04, 0x1B, 0xFE, 0x99, 0x04, 0xE0};
    TEST_ASSERT_FALSE (re_e0_check_format (raw_buf_type));
    const uint8_t raw_buf_len[48] = {0x02, 0x01, 0x04, 0x1A, 0xFF, 0x99, 0x04, 0xE0};
    TEST_ASSERT_FALSE (re_e0_check_format (raw_buf_len));
    const uint8_t raw_buf_byte1[48] = {0x02, 0x02, 0x04, 0x1B, 0xFF, 0x99, 0x04, 0xE0};
    TEST_ASSERT_FALSE (re_e0_check_format (raw_buf_byte1));
    const uint8_t raw_buf_byte0[48] = {0x03, 0x01, 0x04, 0x1B, 0xFF, 0x99, 0x04, 0xE0};
    TEST_ASSERT_FALSE (re_e0_check_format (raw_buf_byte0));
}

void test_re_6_data_invalid (void)
{
    const uint16_t measurement_cnt = 123;
    const uint64_t radio_mac = 0xCBB8334C884FULL;
    const re_e0_data_t data = re_e0_data_invalid (measurement_cnt, radio_mac);
    TEST_ASSERT (isnan (data.temperature_c));
    TEST_ASSERT (isnan (data.humidity_rh));
    TEST_ASSERT (isnan (data.pressure_pa));
    TEST_ASSERT (isnan (data.pm1p0_ppm));
    TEST_ASSERT (isnan (data.pm1p0_ppm));
    TEST_ASSERT (isnan (data.pm1p0_ppm));
    TEST_ASSERT (isnan (data.pm1p0_ppm));
    TEST_ASSERT (isnan (data.co2));
    TEST_ASSERT (isnan (data.voc_index));
    TEST_ASSERT (isnan (data.nox_index));
    TEST_ASSERT (isnan (data.luminosity));
    TEST_ASSERT (isnan (data.sound_avg_dba));
    TEST_ASSERT (isnan (data.sound_peak_spl_db));
    TEST_ASSERT (isnan (data.voltage));
    TEST_ASSERT_EQUAL (false, data.flag_usb_on);
    TEST_ASSERT_EQUAL (false, data.flag_low_battery);
    TEST_ASSERT_EQUAL (false, data.flag_calibration_in_progress);
    TEST_ASSERT_EQUAL (false, data.flag_boost_mode);
    TEST_ASSERT_EQUAL (data.measurement_count, measurement_cnt);
    TEST_ASSERT_EQUAL (data.address, radio_mac);
}
