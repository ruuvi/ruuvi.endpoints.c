#include "unity.h"

#include "ruuvi_endpoint_5.h"

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

static const re_5_data_t m_re_5_data_ok_max =
{
    .humidity_rh = 163.8350,
    .pressure_pa = 115534,
    .temperature_c = 163.835,
    .accelerationx_g = 32.767,
    .accelerationy_g = 32.767,
    .accelerationz_g = 32.767,
    .battery_v = 3.646,
    .measurement_count = 65534,
    .movement_count = 254,
    .address = 0xCBB8334C884F,
    .tx_power = 20
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

static const re_5_data_t m_re_5_data_invalid_min_1 =
{
    .humidity_rh = 53.49,
    .pressure_pa = 49999,
    .temperature_c = 24.3,
    .accelerationx_g = 0.004,
    .accelerationy_g = -0.004,
    .accelerationz_g = 1.036,
    .battery_v = 1.500,
    .measurement_count = 205,
    .movement_count = 66,
    .address = 0xCBB8334C884F,
    .tx_power = 4
};

static const re_5_data_t m_re_5_data_invalid_min_2 =
{
    .humidity_rh = 53.49,
    .pressure_pa = 49999,
    .temperature_c = 24.3,
    .accelerationx_g = 0.004,
    .accelerationy_g = -0.004,
    .accelerationz_g = 1.036,
    .battery_v = 2.977,
    .measurement_count = 205,
    .movement_count = 66,
    .address = 0xCBB8334C884F,
    .tx_power = -41
};

static const re_5_data_t m_re_5_data_invalid_max =
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
    .measurement_count = 205,
    .movement_count = 66,
    .address = 0xCBB8334C884F,
    .tx_power = RE_5_INVALID_POWER
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
    err_code = re_5_encode ( (uint8_t * const) &test_buffer,
                             (const re_5_data_t *) &m_re_5_data_ok);
    TEST_ASSERT (RE_SUCCESS == err_code);
}

void test_ruuvi_endpoint_5_get_ok_max (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[24] = {0};
    err_code = re_5_encode ( (uint8_t * const) &test_buffer,
                             (const re_5_data_t *) &m_re_5_data_ok_max);
    TEST_ASSERT (RE_SUCCESS == err_code);
}

void test_ruuvi_endpoint_5_get_ok_min (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[24] = {0};
    err_code = re_5_encode ( (uint8_t * const) &test_buffer,
                             (const re_5_data_t *) &m_re_5_data_ok_min);
    TEST_ASSERT (RE_SUCCESS == err_code);
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
    err_code = re_5_encode (p_test_buffer,
                            (const re_5_data_t *) &m_re_5_data_ok);
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
    TEST_ASSERT (RE_SUCCESS == err_code &&
                 RE_5_DESTINATION == test_buffer[RE_5_OFFSET_HEADER] &&
                 (RE_5_INVALID_PRESSURE >> 8) == test_buffer[RE_5_OFFSET_PRESSURE_MSB] &&
                 (RE_5_INVALID_PRESSURE & 0xFF) == test_buffer[RE_5_OFFSET_PRESSURE_LSB] &&
                 (RE_5_INVALID_TEMPERATURE >> 8) == test_buffer[RE_5_OFFSET_TEMPERATURE_MSB] &&
                 (RE_5_INVALID_TEMPERATURE & 0xFF) == test_buffer[RE_5_OFFSET_TEMPERATURE_LSB] &&
                 (RE_5_INVALID_HUMIDITY >> 8) == test_buffer[RE_5_OFFSET_HUMIDITY_MSB] &&
                 (RE_5_INVALID_HUMIDITY & 0xFF) == test_buffer[RE_5_OFFSET_HUMIDITY_LSB] &&
                 (RE_5_INVALID_ACCELERATION >> 8) == test_buffer[RE_5_OFFSET_ACCELERATIONX_MSB] &&
                 (RE_5_INVALID_ACCELERATION & 0xFF) == test_buffer[RE_5_OFFSET_ACCELERATIONX_LSB] &&
                 (RE_5_INVALID_ACCELERATION >> 8) == test_buffer[RE_5_OFFSET_ACCELERATIONY_MSB] &&
                 (RE_5_INVALID_ACCELERATION & 0xFF) == test_buffer[RE_5_OFFSET_ACCELERATIONY_LSB] &&
                 (RE_5_INVALID_ACCELERATION >> 8) == test_buffer[RE_5_OFFSET_ACCELERATIONZ_MSB] &&
                 (RE_5_INVALID_ACCELERATION & 0xFF) == test_buffer[RE_5_OFFSET_ACCELERATIONZ_LSB] &&
                 0xFF == test_buffer[RE_5_OFFSET_POWER_MSB] &&
                 0xFF == test_buffer[RE_5_OFFSET_POWER_LSB]);
}

void test_ruuvi_endpoint_5_get_min_1_invalid_data (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[24] = {0};
    err_code = re_5_encode ( (uint8_t * const) &test_buffer,
                             (const re_5_data_t * const) &m_re_5_data_invalid_min_1);
    TEST_ASSERT (RE_SUCCESS == err_code &&
                 (RE_5_INVALID_PRESSURE >> 8) == test_buffer[RE_5_OFFSET_PRESSURE_MSB] &&
                 (RE_5_INVALID_PRESSURE & 0xFF) == test_buffer[RE_5_OFFSET_PRESSURE_LSB] &&
                 0xFF == test_buffer[RE_5_OFFSET_POWER_MSB] &&
                 0x70 == (test_buffer[RE_5_OFFSET_POWER_LSB] & 0x70) &&
                 RE_5_INVALID_POWER != (test_buffer[RE_5_OFFSET_POWER_LSB] & 0x1F));
}

void test_ruuvi_endpoint_5_get_min_2_invalid_data (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[24] = {0};
    err_code = re_5_encode ( (uint8_t * const) &test_buffer,
                             (const re_5_data_t * const) &m_re_5_data_invalid_min_2);
    TEST_ASSERT (RE_SUCCESS == err_code &&
                 (RE_5_INVALID_PRESSURE >> 8) == test_buffer[RE_5_OFFSET_PRESSURE_MSB] &&
                 (RE_5_INVALID_PRESSURE & 0xFF) == test_buffer[RE_5_OFFSET_PRESSURE_LSB] &&
                 0xFF != test_buffer[RE_5_OFFSET_POWER_MSB] &&
                 0x70 != (test_buffer[RE_5_OFFSET_POWER_LSB] & 0x70) &&
                 RE_5_INVALID_POWER == (test_buffer[RE_5_OFFSET_POWER_LSB] & 0x1F));
}

void test_ruuvi_endpoint_5_get_max_invalid_data (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[24] = {0};
    err_code = re_5_encode ( (uint8_t * const) &test_buffer,
                             (const re_5_data_t * const) &m_re_5_data_invalid_max);
    TEST_ASSERT (RE_SUCCESS == err_code &&
                 0xFF != test_buffer[RE_5_OFFSET_POWER_MSB] &&
                 0x70 != (test_buffer[RE_5_OFFSET_POWER_LSB] & 0x70) &&
                 RE_5_INVALID_POWER == (test_buffer[RE_5_OFFSET_POWER_LSB] & 0x1F));
}
