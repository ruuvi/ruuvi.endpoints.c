#include "unity.h"

#include "ruuvi_endpoint_6.h"
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
void test_ruuvi_endpoint_6_get_ok (void)
{
    static const re_6_data_t m_re_6_data_ok =
    {
        .pm1p0_ppm = 10.1,
        .pm2p5_ppm = 11.2,
        .pm4p0_ppm = 12.3,
        .pm10p0_ppm = 13.4,
        .co2 = 201,
        .humidity_rh = 55.3,
        .voc_index = 10,
        .nox_index = 2,
        .temperature_c = 29.5,
        .measurement_count = 205,
        .address = 0xCBB8334C884F,
    };
    static const uint8_t valid_data[] =
    {
        0x06,
        0x00, 0x65, 0x00, 0x70, 0x00, 0x7B, 0x00, 0x86,
        0x00, 0xC9,
        0x8A, 0x41, 0x40, 0x21, 0x27,
        0x00, 0xCD,
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F
    };
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[24] = {0};
    err_code = re_6_encode (test_buffer, &m_re_6_data_ok);
    TEST_ASSERT_EQUAL (RE_SUCCESS, err_code);
    TEST_ASSERT (! (memcmp (test_buffer, valid_data, sizeof (valid_data))));
    uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x1B, 0xFF, 0x99, 0x04};
    memcpy (&raw_buf[7], test_buffer, sizeof (test_buffer));
    re_6_data_t decoded_data = {0};
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok.voc_index), lrintf (decoded_data.voc_index));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok.nox_index), lrintf (decoded_data.nox_index));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (m_re_6_data_ok.measurement_count, decoded_data.measurement_count);
    TEST_ASSERT_EQUAL (m_re_6_data_ok.address, decoded_data.address);
}

void test_ruuvi_endpoint_6_get_ok_max (void)
{
    static const re_6_data_t m_re_6_data_ok_max =
    {
        .pm1p0_ppm = 1000.0,
        .pm2p5_ppm = 1000.0,
        .pm4p0_ppm = 1000.0,
        .pm10p0_ppm = 1000.0,
        .co2 = 40000,
        .humidity_rh = 100.0,
        .voc_index = 500,
        .nox_index = 500,
        .temperature_c = 102.3,
        .measurement_count = 65534,
        .address = 0xCBB8334C884F,
    };
    static const uint8_t max_data[] =
    {
        0x06,
        0x27, 0x10, 0x27, 0x10, 0x27, 0x10, 0x27, 0x10,
        0x9C, 0x40,
        0xFA, 0x3E, 0x9F, 0x43, 0xFF,
        0xFF, 0xFE,
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F
    };
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[24] = {0};
    err_code = re_6_encode ( (uint8_t * const) &test_buffer,
                             (const re_6_data_t *) &m_re_6_data_ok_max);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (! (memcmp (test_buffer, max_data, sizeof (max_data))));
    uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x1B, 0xFF, 0x99, 0x04};
    memcpy (&raw_buf[7], test_buffer, sizeof (test_buffer));
    re_6_data_t decoded_data = {0};
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_max.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_max.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_max.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_max.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_max.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_max.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_max.voc_index),
                       lrintf (decoded_data.voc_index));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_max.nox_index),
                       lrintf (decoded_data.nox_index));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_max.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (m_re_6_data_ok_max.measurement_count, decoded_data.measurement_count);
    TEST_ASSERT_EQUAL (m_re_6_data_ok_max.address, decoded_data.address);
}

void test_ruuvi_endpoint_6_get_ok_min (void)
{
    static const re_6_data_t m_re_6_data_ok_min =
    {
        .pm1p0_ppm = 0,
        .pm2p5_ppm = 0,
        .pm4p0_ppm = 0,
        .pm10p0_ppm = 0,
        .co2 = 0,
        .humidity_rh = 0,
        .voc_index = 1,
        .nox_index = 1,
        .temperature_c = -102.3,
        .measurement_count = 0,
        .address = 0xCBB8334C884F,
    };
    static const uint8_t min_data[] =
    {
        0x06,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00,
        0x00, 0x00, 0x20, 0x14, 0x01,
        0x00, 0x00,
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F
    };
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[24] = {0};
    err_code = re_6_encode ( (uint8_t * const) &test_buffer,
                             (const re_6_data_t *) &m_re_6_data_ok_min);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (! (memcmp (test_buffer, min_data, sizeof (min_data))));
    uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x1B, 0xFF, 0x99, 0x04};
    memcpy (&raw_buf[7], test_buffer, sizeof (test_buffer));
    re_6_data_t decoded_data = {0};
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_min.pm1p0_ppm * 10.0f),
                       lrintf (decoded_data.pm1p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_min.pm2p5_ppm * 10.0f),
                       lrintf (decoded_data.pm2p5_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_min.pm4p0_ppm * 10.0f),
                       lrintf (decoded_data.pm4p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_min.pm10p0_ppm * 10.0f),
                       lrintf (decoded_data.pm10p0_ppm * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_min.co2), lrintf (decoded_data.co2));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_min.humidity_rh * 10.0f),
                       lrintf (decoded_data.humidity_rh * 10.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_min.voc_index),
                       lrintf (decoded_data.voc_index));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_min.nox_index),
                       lrintf (decoded_data.nox_index));
    TEST_ASSERT_EQUAL (lrintf (m_re_6_data_ok_min.temperature_c * 10.0f),
                       lrintf (decoded_data.temperature_c * 10.0f));
    TEST_ASSERT_EQUAL (m_re_6_data_ok_min.measurement_count, decoded_data.measurement_count);
    TEST_ASSERT_EQUAL (m_re_6_data_ok_min.address, decoded_data.address);
}

/**
 * @brief Null buffer
 *
 * @retval RE_ERROR_NULL on success test.
 */
void test_ruuvi_endpoint_6_get_error_null_buffer (void)
{
    static const re_6_data_t m_re_6_data_ok =
    {
        .pm1p0_ppm = 10.1,
        .pm2p5_ppm = 11.2,
        .pm4p0_ppm = 12.3,
        .pm10p0_ppm = 13.4,
        .co2 = 201,
        .humidity_rh = 55.3,
        .voc_index = 10,
        .nox_index = 2,
        .temperature_c = 25.5,
        .address = 0xCBB8334C884F,
    };
    re_status_t err_code = RE_ERROR_NULL;
    uint8_t * const p_test_buffer = NULL;
    err_code = re_6_encode (p_test_buffer, &m_re_6_data_ok);
    TEST_ASSERT (RE_ERROR_NULL == err_code);
}

/**
 * @brief Null data
 *
 * @retval RE_ERROR_NULL on success test.
 */
void test_ruuvi_endpoint_6_get_error_null_data (void)
{
    re_status_t err_code = RE_ERROR_NULL;
    uint8_t test_buffer[24] = {0};
    const re_6_data_t * p_re_6_data = NULL;
    err_code = re_6_encode ( (uint8_t * const) &test_buffer, p_re_6_data);
    TEST_ASSERT (RE_ERROR_NULL == err_code);
}

/**
 * @brief True to test encode operation with invalid data
 *
 * @retval RD_SUCCESS on success test.
 */
void test_ruuvi_endpoint_6_get_invalid_data (void)
{
    static const re_6_data_t m_re_6_data_invalid =
    {
        .pm1p0_ppm = NAN,
        .pm2p5_ppm = NAN,
        .pm4p0_ppm = NAN,
        .pm10p0_ppm = NAN,
        .co2 = NAN,
        .humidity_rh = NAN,
        .voc_index = NAN,
        .nox_index = NAN,
        .temperature_c = NAN,
        .measurement_count = 65535,
        .address = 0xFFFFFFFFFFFF,
    };
    static const uint8_t invalid_data[] =
    {
        0x06,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xF4, 0x00,
        0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[24] = {0};
    err_code = re_6_encode ( (uint8_t * const) &test_buffer,
                             (const re_6_data_t *) &m_re_6_data_invalid);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (! (memcmp (test_buffer, invalid_data, sizeof (invalid_data))));
    uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x1B, 0xFF, 0x99, 0x04};
    memcpy (&raw_buf[7], test_buffer, sizeof (test_buffer));
    re_6_data_t decoded_data = {0};
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_6_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (m_re_6_data_invalid.pm1p0_ppm, decoded_data.pm1p0_ppm);
    TEST_ASSERT_EQUAL (m_re_6_data_invalid.pm2p5_ppm, decoded_data.pm1p0_ppm);
    TEST_ASSERT_EQUAL (m_re_6_data_invalid.pm4p0_ppm, decoded_data.pm1p0_ppm);
    TEST_ASSERT_EQUAL (m_re_6_data_invalid.pm10p0_ppm, decoded_data.pm1p0_ppm);
    TEST_ASSERT_EQUAL (m_re_6_data_invalid.co2, decoded_data.co2);
    TEST_ASSERT_EQUAL (m_re_6_data_invalid.humidity_rh, decoded_data.humidity_rh);
    TEST_ASSERT_EQUAL (m_re_6_data_invalid.voc_index, decoded_data.voc_index);
    TEST_ASSERT_EQUAL (m_re_6_data_invalid.nox_index, decoded_data.nox_index);
    TEST_ASSERT_EQUAL (m_re_6_data_invalid.temperature_c, decoded_data.temperature_c);
    TEST_ASSERT_EQUAL (m_re_6_data_invalid.measurement_count, decoded_data.measurement_count);
    TEST_ASSERT_EQUAL (m_re_6_data_invalid.address, decoded_data.address);
}

void test_ruuvi_endpoint_6_underflow (void)
{
    static const re_6_data_t m_re_6_data_underflow =
    {
        .pm1p0_ppm = -1,
        .pm2p5_ppm = -1,
        .pm4p0_ppm = -1,
        .pm10p0_ppm = -1,
        .co2 = -1,
        .humidity_rh = -1,
        .voc_index = 0,
        .nox_index = 0,
        .temperature_c = -102.4,
        .measurement_count = 0,
        .address = 0xCBB8334C884F,
    };
    static const uint8_t min_data[] =
    {
        0x06,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00,
        0x00, 0x3F, 0xFF, 0xF4, 0x01,
        0x00, 0x00,
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F
    };
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[24] = {0};
    err_code = re_6_encode (test_buffer, &m_re_6_data_underflow);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (! (memcmp (test_buffer, min_data, sizeof (min_data))));
}

void test_ruuvi_endpoint_6_overflow (void)
{
    static const re_6_data_t m_re_6_data_overflow =
    {
        .pm1p0_ppm = 1000.1,
        .pm2p5_ppm = 1000.1,
        .pm4p0_ppm = 1000.1,
        .pm10p0_ppm = 1000.1,
        .co2 = 40001,
        .humidity_rh = 100.1,
        .voc_index = 501,
        .nox_index = 501,
        .temperature_c = 102.4,
        .measurement_count = 65535,
        .address = 0xCBB8334C884F,
    };
    static const uint8_t max_data[] =
    {
        0x06,
        0x27, 0x10, 0x27, 0x10, 0x27, 0x10, 0x27, 0x10,
        0x9C, 0x40,
        0xFA, 0x3F, 0xFF, 0xF3, 0xFF,
        0xFF, 0xFF,
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F
    };
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[24] = {0};
    err_code = re_6_encode (test_buffer, &m_re_6_data_overflow);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (! (memcmp (test_buffer, max_data, sizeof (max_data))));
}

void test_ruuvi_endpoint_6_check_format_ok (void)
{
    const uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x1B, 0xFF, 0x99, 0x04, 0x06};
    TEST_ASSERT_TRUE (re_6_check_format (raw_buf));
}

void test_ruuvi_endpoint_6_check_format_fail (void)
{
    const uint8_t raw_buf_payload_format[31] = {0x02, 0x01, 0x04, 0x1B, 0xFF, 0x99, 0x04, 0x05};
    TEST_ASSERT_FALSE (re_6_check_format (raw_buf_payload_format));
    const uint8_t raw_buf_manufacturer_id_1[31] = {0x02, 0x01, 0x04, 0x1B, 0xFF, 0x99, 0x05, 0x06};
    TEST_ASSERT_FALSE (re_6_check_format (raw_buf_manufacturer_id_1));
    const uint8_t raw_buf_manufacturer_id_2[31] = {0x02, 0x01, 0x04, 0x1B, 0xFF, 0x9a, 0x04, 0x06};
    TEST_ASSERT_FALSE (re_6_check_format (raw_buf_manufacturer_id_2));
    const uint8_t raw_buf_type[31] = {0x02, 0x01, 0x04, 0x1B, 0xFE, 0x99, 0x04, 0x06};
    TEST_ASSERT_FALSE (re_6_check_format (raw_buf_type));
    const uint8_t raw_buf_len[31] = {0x02, 0x01, 0x04, 0x1A, 0xFF, 0x99, 0x04, 0x06};
    TEST_ASSERT_FALSE (re_6_check_format (raw_buf_len));
    const uint8_t raw_buf_byte1[31] = {0x02, 0x02, 0x04, 0x1B, 0xFF, 0x99, 0x04, 0x06};
    TEST_ASSERT_FALSE (re_6_check_format (raw_buf_byte1));
    const uint8_t raw_buf_byte0[31] = {0x03, 0x01, 0x04, 0x1B, 0xFF, 0x99, 0x04, 0x06};
    TEST_ASSERT_FALSE (re_6_check_format (raw_buf_byte0));
}

void test_re_6_data_invalid (void)
{
    const uint16_t measurement_cnt = 123;
    const uint64_t radio_mac = 0xCBB8334C884FULL;
    const re_6_data_t data = re_6_data_invalid (measurement_cnt, radio_mac);
    static const re_6_data_t m_re_6_data_invalid =
    {
        .pm1p0_ppm = NAN,
        .pm2p5_ppm = NAN,
        .pm4p0_ppm = NAN,
        .pm10p0_ppm = NAN,
        .co2 = NAN,
        .humidity_rh = NAN,
        .voc_index = NAN,
        .nox_index = NAN,
        .temperature_c = NAN,
        .measurement_count = 65535,
        .address = 0xFFFFFFFFFFFF,
    };
    TEST_ASSERT_EQUAL (data.measurement_count, measurement_cnt);
    TEST_ASSERT_EQUAL (data.address, radio_mac);
    TEST_ASSERT_TRUE (isnan (data.pm1p0_ppm));
    TEST_ASSERT_TRUE (isnan (data.pm1p0_ppm));
    TEST_ASSERT_TRUE (isnan (data.pm1p0_ppm));
    TEST_ASSERT_TRUE (isnan (data.co2));
    TEST_ASSERT_TRUE (isnan (data.humidity_rh));
    TEST_ASSERT_TRUE (isnan (data.voc_index));
    TEST_ASSERT_TRUE (isnan (data.nox_index));
    TEST_ASSERT_TRUE (isnan (data.temperature_c));
}
