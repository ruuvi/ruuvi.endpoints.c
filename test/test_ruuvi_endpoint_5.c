#include "unity.h"

#include "ruuvi_endpoint_5.h"
#include <string.h>

static const re_5_data_t m_re_5_data_ok =
{
    .humidity_rh = 53.49,
    .pressure_pa = 100044,
    .temperature_c = 24.3,
    .accelerationx_g = 0.004,
    .accelerationy_g = -0.004,
    .accelerationz_g = 1.036,
    .battery_v = 2.977,
    .measurement_count = 205,
    .movement_count = 66,
    .address = 0xCBB8334C884F,
    .tx_power = 4
};

static const uint8_t valid_data[] =
{
    0x05, 0x12, 0xFC, 0x53, 0x94, 0xC3, 0x7C, 0x00,
    0x04, 0xFF, 0xFC, 0x04, 0x0C, 0xAC, 0x36, 0x42,
    0x00, 0xCD, 0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F
};

static const re_5_data_t m_re_5_data_ok_max =
{
    .humidity_rh = 163.8350,
    .pressure_pa = 115534,
    .temperature_c = 163.8350,
    .accelerationx_g = 32.767,
    .accelerationy_g = 32.767,
    .accelerationz_g = 32.767,
    .battery_v = 3.646,
    .measurement_count = 65534,
    .movement_count = 254,
    .address = 0xCBB8334C884F,
    .tx_power = 20
};

static const uint8_t max_data[] =
{
    0x05, 0x7F, 0xFF, 0xFF, 0xFE, 0xFF, 0xFE, 0x7F,
    0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0xFF, 0xDE, 0xFE,
    0xFF, 0xFE, 0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F
};

static const re_5_data_t m_re_5_data_ok_min =
{
    .humidity_rh = 0.000,
    .pressure_pa = 50000,
    .temperature_c = -163.835,
    .accelerationx_g = -32.767,
    .accelerationy_g = -32.767,
    .accelerationz_g = -32.767,
    .battery_v = 1.600,
    .measurement_count = 0,
    .movement_count = 0,
    .address = 0xCBB8334C884F,
    .tx_power = -40
};

static const uint8_t min_data[] =
{
    0x05, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x80,
    0x01, 0x80, 0x01, 0x80, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F
};

static const re_5_data_t m_re_5_data_underflow =
{
    .humidity_rh = -1,
    .pressure_pa = 49999,
    .temperature_c = -170,
    .accelerationx_g = -40,
    .accelerationy_g = -40,
    .accelerationz_g = -40,
    .battery_v = 1.500,
    .measurement_count = 0,
    .movement_count = 0,
    .address = 0xCBB8334C884F,
    .tx_power = -41
};


static const re_5_data_t m_re_5_data_overflow =
{
    .humidity_rh = 200,
    .pressure_pa = 200044,
    .temperature_c = 170,
    .accelerationx_g = 40,
    .accelerationy_g = 40,
    .accelerationz_g = 40,
    .battery_v = 3.700,
    .measurement_count = 65534,
    .movement_count = 254,
    .address = 0xCBB8334C884F,
    .tx_power = 25
};

static const re_5_data_t m_re_5_data_invalid =
{
    .humidity_rh = NAN,
    .pressure_pa = NAN,
    .temperature_c = NAN,
    .accelerationx_g = NAN,
    .accelerationy_g = NAN,
    .accelerationz_g = NAN,
    .battery_v = NAN,
    .measurement_count = 65535,
    .movement_count = 255,
    .address = 0xFFFFFFFFFFFF,
    .tx_power = RE_5_INVALID_POWER
};

static const uint8_t invalid_data[] =
{
    0x05, 0x80, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x80,
    0x00, 0x80, 0x00, 0x80, 0x00, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

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
void test_ruuvi_endpoint_5_get_ok (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[24] = {0};
    err_code = re_5_encode (test_buffer, &m_re_5_data_ok);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (! (memcmp (test_buffer, valid_data, sizeof (valid_data))));
    uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x1B, 0xFF, 0x99, 0x04};
    memcpy (&raw_buf[7], test_buffer, sizeof (test_buffer));
    re_5_data_t decoded_data = {0};
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_5_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (m_re_5_data_ok.humidity_rh * 10000.0f),
                       lrintf (decoded_data.humidity_rh * 10000.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_5_data_ok.temperature_c * 1000.0f),
                       lrintf (decoded_data.temperature_c * 1000.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_5_data_ok.pressure_pa),
                       lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (m_re_5_data_ok.accelerationx_g * 1000.0f),
                       lrintf (decoded_data.accelerationx_g * 1000.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_5_data_ok.accelerationy_g * 1000.0f),
                       lrintf (decoded_data.accelerationy_g * 1000.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_5_data_ok.accelerationz_g * 1000.0f),
                       lrintf (decoded_data.accelerationz_g * 1000.0f));
    TEST_ASSERT_EQUAL (m_re_5_data_ok.movement_count, decoded_data.movement_count);
    TEST_ASSERT_EQUAL (lrintf (m_re_5_data_ok.battery_v * 1000.0f),
                       lrintf (decoded_data.battery_v * 1000.0f));
    TEST_ASSERT_EQUAL (m_re_5_data_ok.tx_power, decoded_data.tx_power);
    TEST_ASSERT_EQUAL (m_re_5_data_ok.measurement_count, decoded_data.measurement_count);
    TEST_ASSERT_EQUAL (m_re_5_data_ok.address, decoded_data.address);
}

void test_ruuvi_endpoint_5_get_ok_max (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[24] = {0};
    err_code = re_5_encode ( (uint8_t * const) &test_buffer,
                             (const re_5_data_t *) &m_re_5_data_ok_max);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (! (memcmp (test_buffer, max_data, sizeof (max_data))));
    uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x1B, 0xFF, 0x99, 0x04};
    memcpy (&raw_buf[7], test_buffer, sizeof (test_buffer));
    re_5_data_t decoded_data = {0};
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_5_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (m_re_5_data_ok_max.humidity_rh * 10000.0f),
                       lrintf (decoded_data.humidity_rh * 10000.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_5_data_ok_max.temperature_c * 1000.0f),
                       lrintf (decoded_data.temperature_c * 1000.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_5_data_ok_max.pressure_pa),
                       lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (m_re_5_data_ok_max.accelerationx_g * 1000.0f),
                       lrintf (decoded_data.accelerationx_g * 1000.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_5_data_ok_max.accelerationy_g * 1000.0f),
                       lrintf (decoded_data.accelerationy_g * 1000.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_5_data_ok_max.accelerationz_g * 1000.0f),
                       lrintf (decoded_data.accelerationz_g * 1000.0f));
    TEST_ASSERT_EQUAL (m_re_5_data_ok_max.movement_count, decoded_data.movement_count);
    TEST_ASSERT_EQUAL (lrintf (m_re_5_data_ok_max.battery_v * 1000.0f),
                       lrintf (decoded_data.battery_v * 1000.0f));
    TEST_ASSERT_EQUAL (m_re_5_data_ok_max.tx_power, decoded_data.tx_power);
    TEST_ASSERT_EQUAL (m_re_5_data_ok_max.measurement_count, decoded_data.measurement_count);
    TEST_ASSERT_EQUAL (m_re_5_data_ok_max.address, decoded_data.address);
}

void test_ruuvi_endpoint_5_get_ok_min (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[24] = {0};
    err_code = re_5_encode ( (uint8_t * const) &test_buffer,
                             (const re_5_data_t *) &m_re_5_data_ok_min);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (! (memcmp (test_buffer, min_data, sizeof (min_data))));
    uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x1B, 0xFF, 0x99, 0x04};
    memcpy (&raw_buf[7], test_buffer, sizeof (test_buffer));
    re_5_data_t decoded_data = {0};
    TEST_ASSERT_EQUAL (RE_SUCCESS, re_5_decode (raw_buf, &decoded_data));
    TEST_ASSERT_EQUAL (lrintf (m_re_5_data_ok_min.humidity_rh * 10000.0f),
                       lrintf (decoded_data.humidity_rh * 10000.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_5_data_ok_min.temperature_c * 1000.0f),
                       lrintf (decoded_data.temperature_c * 1000.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_5_data_ok_min.pressure_pa),
                       lrintf (decoded_data.pressure_pa));
    TEST_ASSERT_EQUAL (lrintf (m_re_5_data_ok_min.accelerationx_g * 1000.0f),
                       lrintf (decoded_data.accelerationx_g * 1000.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_5_data_ok_min.accelerationy_g * 1000.0f),
                       lrintf (decoded_data.accelerationy_g * 1000.0f));
    TEST_ASSERT_EQUAL (lrintf (m_re_5_data_ok_min.accelerationz_g * 1000.0f),
                       lrintf (decoded_data.accelerationz_g * 1000.0f));
    TEST_ASSERT_EQUAL (m_re_5_data_ok_min.movement_count, decoded_data.movement_count);
    TEST_ASSERT_EQUAL (lrintf (m_re_5_data_ok_min.battery_v * 1000.0f),
                       lrintf (decoded_data.battery_v * 1000.0f));
    TEST_ASSERT_EQUAL (m_re_5_data_ok_min.tx_power, decoded_data.tx_power);
    TEST_ASSERT_EQUAL (m_re_5_data_ok_min.measurement_count, decoded_data.measurement_count);
    TEST_ASSERT_EQUAL (m_re_5_data_ok_min.address, decoded_data.address);
}

/**
 * @brief Null buffer
 *
 * @retval RE_ERROR_NULL on success test.
 */
void test_ruuvi_endpoint_5_get_error_null_buffer (void)
{
    re_status_t err_code = RE_ERROR_NULL;
    uint8_t * const p_test_buffer = NULL;
    err_code = re_5_encode (p_test_buffer, &m_re_5_data_ok);
    TEST_ASSERT (RE_ERROR_NULL == err_code);
}

/**
 * @brief Null data
 *
 * @retval RE_ERROR_NULL on success test.
 */
void test_ruuvi_endpoint_5_get_error_null_data (void)
{
    re_status_t err_code = RE_ERROR_NULL;
    uint8_t test_buffer[24] = {0};
    const re_5_data_t * p_re_5_data = NULL;
    err_code = re_5_encode ( (uint8_t * const) &test_buffer,
                             p_re_5_data);
    TEST_ASSERT (RE_ERROR_NULL == err_code);
}

/**
 * @brief True to test encode operation with invalid data
 *
 * @retval RD_SUCCESS on success test.
 */
void test_ruuvi_endpoint_5_get_invalid_data (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[24] = {0};
    err_code = re_5_encode ( (uint8_t * const) &test_buffer,
                             (const re_5_data_t *) &m_re_5_data_invalid);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (! (memcmp (test_buffer, invalid_data, sizeof (invalid_data))));
}

void test_ruuvi_endpoint_5_underflow (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[24] = {0};
    err_code = re_5_encode (test_buffer, &m_re_5_data_underflow);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (! (memcmp (test_buffer, min_data, sizeof (min_data))));
}

void test_ruuvi_endpoint_5_overflow (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[24] = {0};
    err_code = re_5_encode (test_buffer, &m_re_5_data_overflow);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (! (memcmp (test_buffer, max_data, sizeof (max_data))));
}

void test_ruuvi_endpoint_5_check_format_ok (void)
{
    const uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x1B, 0xFF, 0x99, 0x04, 0x05};
    TEST_ASSERT_TRUE (re_5_check_format (raw_buf));
}

void test_ruuvi_endpoint_5_check_format_fail (void)
{
    const uint8_t raw_buf_payload_format[31] = {0x02, 0x01, 0x04, 0x1B, 0xFF, 0x99, 0x04, 0x06};
    TEST_ASSERT_FALSE (re_5_check_format (raw_buf_payload_format));
    const uint8_t raw_buf_manufacturer_id_1[31] = {0x02, 0x01, 0x04, 0x1B, 0xFF, 0x99, 0x05, 0x05};
    TEST_ASSERT_FALSE (re_5_check_format (raw_buf_manufacturer_id_1));
    const uint8_t raw_buf_manufacturer_id_2[31] = {0x02, 0x01, 0x04, 0x1B, 0xFF, 0x9a, 0x04, 0x05};
    TEST_ASSERT_FALSE (re_5_check_format (raw_buf_manufacturer_id_2));
    const uint8_t raw_buf_type[31] = {0x02, 0x01, 0x04, 0x1B, 0xFE, 0x99, 0x04, 0x05};
    TEST_ASSERT_FALSE (re_5_check_format (raw_buf_type));
    const uint8_t raw_buf_len[31] = {0x02, 0x01, 0x04, 0x1A, 0xFF, 0x99, 0x04, 0x05};
    TEST_ASSERT_FALSE (re_5_check_format (raw_buf_len));
    const uint8_t raw_buf_byte1[31] = {0x02, 0x02, 0x04, 0x1B, 0xFF, 0x99, 0x04, 0x05};
    TEST_ASSERT_FALSE (re_5_check_format (raw_buf_byte1));
    const uint8_t raw_buf_byte0[31] = {0x03, 0x01, 0x04, 0x1B, 0xFF, 0x99, 0x04, 0x05};
    TEST_ASSERT_FALSE (re_5_check_format (raw_buf_byte0));
}
