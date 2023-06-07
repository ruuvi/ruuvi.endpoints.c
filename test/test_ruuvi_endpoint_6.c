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
        .humidity_rh = 100.0, // 3e8
        .voc_index = 500, // 1f4
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
        .voc_index = 0,
        .nox_index = 0,
        .temperature_c = -102.3,
        .measurement_count = 0,
        .address = 0xCBB8334C884F,
    };
    static const uint8_t min_data[] =
    {
        0x06,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00,
        0x00, 0x00, 0x00, 0x04, 0x01,
        0x00, 0x00,
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F
    };
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[24] = {0};
    err_code = re_6_encode ( (uint8_t * const) &test_buffer,
                             (const re_6_data_t *) &m_re_6_data_ok_min);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (! (memcmp (test_buffer, min_data, sizeof (min_data))));
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
        0x00, 0x00, 0x00, 0x04, 0x01,
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
        0xFA, 0x3E, 0x9F, 0x43, 0xFF,
        0xFF, 0xFF,
        0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F
    };
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[24] = {0};
    err_code = re_6_encode (test_buffer, &m_re_6_data_overflow);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (! (memcmp (test_buffer, max_data, sizeof (max_data))));
}
