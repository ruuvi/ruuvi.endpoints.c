#include "unity.h"

#include "ruuvi_endpoint_7.h"
#include "ruuvi_endpoints.h"
#include <string.h>
#include <math.h>

/* Typical valid data */
/* tilt_x = 15°, tilt_y = -30° -> acc_x = sin(15°) ≈ 0.259, acc_y = sin(-30°) = -0.5 */
/* acc_z = sqrt(1 - 0.259² - 0.5²) ≈ 0.826 */
static const re_7_data_t m_re_7_data_ok =
{
    .temperature_c = 24.3f,
    .humidity_rh = 53.49f,
    .pressure_pa = 100044.0f,
    .acceleration_x_g = 0.259f,
    .acceleration_y_g = -0.5f,
    .acceleration_z_g = 0.826f,
    .tilt_x_deg = 15.0f,   /* Expected after decode */
    .tilt_y_deg = -30.0f,  /* Expected after decode */
    .luminosity_lux = 350.0f,
    .color_temp_k = 4500U,
    .battery_v = 2.9f,
    .motion_intensity = 5U,
    .motion_count = 42U,
    .sequence_counter = 123U,
    .motion_detected = true,
    .presence_detected = true,
    .address = 0xCBB8334C88ULL
};

/* Maximum valid data */
/* tilt_x = 90°, tilt_y = 0° -> acc_x = 1.0, acc_y = 0.0 (device tilted fully in +X) */
static const re_7_data_t m_re_7_data_ok_max =
{
    .temperature_c = 163.835f,
    .humidity_rh = 163.835f,
    .pressure_pa = 115534.0f,
    .acceleration_x_g = 1.0f,
    .acceleration_y_g = 0.0f,
    .acceleration_z_g = 0.0f,
    .tilt_x_deg = 90.0f,   /* Expected after decode */
    .tilt_y_deg = 0.0f,    /* Expected after decode */
    .luminosity_lux = 65534.0f,
    .color_temp_k = 7650U,
    .battery_v = 3.6f,
    .motion_intensity = 15U,
    .motion_count = 254U,
    .sequence_counter = 254U,
    .motion_detected = true,
    .presence_detected = true,
    .address = 0xFFFFFFFFFFULL
};

/* Minimum valid data */
/* tilt_x = -90°, tilt_y = 0° -> acc_x = -1.0, acc_y = 0.0 (device tilted fully in -X) */
static const re_7_data_t m_re_7_data_ok_min =
{
    .temperature_c = -163.835f,
    .humidity_rh = 0.0f,
    .pressure_pa = 50000.0f,
    .acceleration_x_g = -1.0f,
    .acceleration_y_g = 0.0f,
    .acceleration_z_g = 0.0f,
    .tilt_x_deg = -90.0f,  /* Expected after decode */
    .tilt_y_deg = 0.0f,    /* Expected after decode */
    .luminosity_lux = 0.0f,
    .color_temp_k = 1000U,
    .battery_v = 1.8f,
    .motion_intensity = 0U,
    .motion_count = 0U,
    .sequence_counter = 0U,
    .motion_detected = false,
    .presence_detected = false,
    .address = 0x0ULL
};

/* Invalid data (NAN/out of range) */
static const re_7_data_t m_re_7_data_invalid =
{
    .temperature_c = NAN,
    .humidity_rh = NAN,
    .pressure_pa = NAN,
    .acceleration_x_g = NAN,
    .acceleration_y_g = NAN,
    .acceleration_z_g = NAN,
    .tilt_x_deg = NAN,
    .tilt_y_deg = NAN,
    .luminosity_lux = NAN,
    .color_temp_k = 0U,  /* Invalid */
    .battery_v = NAN,
    .motion_intensity = 16U,  /* > max */
    .motion_count = 255U,  /* Invalid */
    .sequence_counter = 255U,  /* Invalid */
    .motion_detected = false,
    .presence_detected = false,
    .address = 0xFFFFFFFFFFULL
};

/* Underflow data (below valid range) */
/* Acceleration pointing in -X direction for -90° tilt */
static const re_7_data_t m_re_7_data_underflow =
{
    .temperature_c = -170.0f,
    .humidity_rh = -1.0f,
    .pressure_pa = 49999.0f,
    .acceleration_x_g = -1.0f,  /* Points to -90° tilt */
    .acceleration_y_g = 0.0f,
    .acceleration_z_g = 0.0f,
    .tilt_x_deg = RE_7_TILT_MIN,  /* Expected clamped value */
    .tilt_y_deg = 0.0f,
    .luminosity_lux = -1.0f,
    .color_temp_k = 999U,  /* Below min */
    .battery_v = 1.5f,
    .motion_intensity = 0U,
    .motion_count = 0U,
    .sequence_counter = 0U,
    .motion_detected = false,
    .presence_detected = false,
    .address = 0x0ULL
};

/* Overflow data (above valid range) */
/* Acceleration pointing in +X direction for +90° tilt */
static const re_7_data_t m_re_7_data_overflow =
{
    .temperature_c = 170.0f,
    .humidity_rh = 200.0f,
    .pressure_pa = 200000.0f,
    .acceleration_x_g = 1.0f,  /* Points to +90° tilt */
    .acceleration_y_g = 0.0f,
    .acceleration_z_g = 0.0f,
    .tilt_x_deg = RE_7_TILT_MAX,  /* Expected clamped value */
    .tilt_y_deg = 0.0f,
    .luminosity_lux = 70000.0f,
    .color_temp_k = 8000U,  /* Above max */
    .battery_v = 4.0f,
    .motion_intensity = 20U,
    .motion_count = 254U,  /* Clamped to max */
    .sequence_counter = 254U,  /* Clamped to max */
    .motion_detected = true,
    .presence_detected = true,
    .address = 0xCBB8334C88ULL
};

void setUp (void)
{
}

void tearDown (void)
{
}

/**
 * @brief Typical encode operation
 */
void test_ruuvi_endpoint_7_encode_ok (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[RE_7_DATA_LENGTH] = {0};
    err_code = re_7_encode (test_buffer, &m_re_7_data_ok);
    TEST_ASSERT_EQUAL (RE_SUCCESS, err_code);
    /* Verify header */
    TEST_ASSERT_EQUAL_HEX8 (RE_7_DESTINATION, test_buffer[RE_7_OFFSET_HEADER]);
    /* Verify sequence */
    TEST_ASSERT_EQUAL_UINT8 (123U, test_buffer[RE_7_OFFSET_SEQ]);
    /* Verify flags: motion=1, presence=1 -> 0x03 */
    TEST_ASSERT_EQUAL_HEX8 (0x03U, test_buffer[RE_7_OFFSET_FLAGS]);
}

/**
 * @brief Encode-decode roundtrip test
 */
void test_ruuvi_endpoint_7_encode_decode_roundtrip (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[RE_7_DATA_LENGTH] = {0};
    err_code = re_7_encode (test_buffer, &m_re_7_data_ok);
    TEST_ASSERT_EQUAL (RE_SUCCESS, err_code);
    /* Build raw BLE advertisement packet */
    uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x17, 0xFF, 0x99, 0x04};
    memcpy (&raw_buf[7], test_buffer, sizeof (test_buffer));
    re_7_data_t decoded_data = {0};
    err_code = re_7_decode (raw_buf, &decoded_data);
    TEST_ASSERT_EQUAL (RE_SUCCESS, err_code);
    /* Temperature: 0.005°C resolution */
    TEST_ASSERT_FLOAT_WITHIN (0.01f, m_re_7_data_ok.temperature_c, decoded_data.temperature_c);
    /* Humidity: 0.0025% resolution */
    TEST_ASSERT_FLOAT_WITHIN (0.01f, m_re_7_data_ok.humidity_rh, decoded_data.humidity_rh);
    /* Pressure: 1 Pa resolution */
    TEST_ASSERT_FLOAT_WITHIN (1.0f, m_re_7_data_ok.pressure_pa, decoded_data.pressure_pa);
    /* Tilt: 0.71° resolution */
    TEST_ASSERT_FLOAT_WITHIN (1.0f, m_re_7_data_ok.tilt_x_deg, decoded_data.tilt_x_deg);
    TEST_ASSERT_FLOAT_WITHIN (1.0f, m_re_7_data_ok.tilt_y_deg, decoded_data.tilt_y_deg);
    /* Luminosity: 1 lux resolution */
    TEST_ASSERT_FLOAT_WITHIN (1.0f, m_re_7_data_ok.luminosity_lux, decoded_data.luminosity_lux);
    /* Color temp: 26K steps */
    TEST_ASSERT_UINT16_WITHIN (26U, m_re_7_data_ok.color_temp_k, decoded_data.color_temp_k);
    /* Battery: ~120mV resolution */
    TEST_ASSERT_FLOAT_WITHIN (0.15f, m_re_7_data_ok.battery_v, decoded_data.battery_v);
    /* Motion intensity: exact */
    TEST_ASSERT_EQUAL_UINT8 (m_re_7_data_ok.motion_intensity, decoded_data.motion_intensity);
    /* Motion count: exact */
    TEST_ASSERT_EQUAL_UINT8 (m_re_7_data_ok.motion_count, decoded_data.motion_count);
    /* Sequence counter: exact */
    TEST_ASSERT_EQUAL_UINT8 (m_re_7_data_ok.sequence_counter, decoded_data.sequence_counter);
    /* Flags: exact */
    TEST_ASSERT_EQUAL (m_re_7_data_ok.motion_detected, decoded_data.motion_detected);
    TEST_ASSERT_EQUAL (m_re_7_data_ok.presence_detected, decoded_data.presence_detected);
    /* MAC: exact (5 bytes) */
    TEST_ASSERT_EQUAL_UINT64 (m_re_7_data_ok.address, decoded_data.address);
}

/**
 * @brief Maximum values encode-decode roundtrip
 */
void test_ruuvi_endpoint_7_encode_decode_max (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[RE_7_DATA_LENGTH] = {0};
    err_code = re_7_encode (test_buffer, &m_re_7_data_ok_max);
    TEST_ASSERT_EQUAL (RE_SUCCESS, err_code);
    uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x17, 0xFF, 0x99, 0x04};
    memcpy (&raw_buf[7], test_buffer, sizeof (test_buffer));
    re_7_data_t decoded_data = {0};
    err_code = re_7_decode (raw_buf, &decoded_data);
    TEST_ASSERT_EQUAL (RE_SUCCESS, err_code);
    TEST_ASSERT_FLOAT_WITHIN (0.01f, RE_7_TEMP_MAX, decoded_data.temperature_c);
    TEST_ASSERT_FLOAT_WITHIN (0.01f, RE_7_HUMI_MAX, decoded_data.humidity_rh);
    TEST_ASSERT_FLOAT_WITHIN (1.0f, RE_7_PRES_MAX, decoded_data.pressure_pa);
    TEST_ASSERT_FLOAT_WITHIN (1.0f, RE_7_TILT_MAX, decoded_data.tilt_x_deg);  /* acc_x=1 -> 90° */
    TEST_ASSERT_FLOAT_WITHIN (1.0f, 0.0f, decoded_data.tilt_y_deg);           /* acc_y=0 -> 0° */
    TEST_ASSERT_FLOAT_WITHIN (1.0f, RE_7_LUMI_MAX, decoded_data.luminosity_lux);
    TEST_ASSERT_EQUAL_UINT8 (RE_7_MOTION_CNT_MAX, decoded_data.motion_count);
    TEST_ASSERT_EQUAL_UINT8 (RE_7_SEQCTR_MAX, decoded_data.sequence_counter);
    TEST_ASSERT_EQUAL (m_re_7_data_ok_max.motion_detected, decoded_data.motion_detected);
    TEST_ASSERT_EQUAL (m_re_7_data_ok_max.presence_detected, decoded_data.presence_detected);
}

/**
 * @brief Minimum values encode-decode roundtrip
 */
void test_ruuvi_endpoint_7_encode_decode_min (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[RE_7_DATA_LENGTH] = {0};
    err_code = re_7_encode (test_buffer, &m_re_7_data_ok_min);
    TEST_ASSERT_EQUAL (RE_SUCCESS, err_code);
    uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x17, 0xFF, 0x99, 0x04};
    memcpy (&raw_buf[7], test_buffer, sizeof (test_buffer));
    re_7_data_t decoded_data = {0};
    err_code = re_7_decode (raw_buf, &decoded_data);
    TEST_ASSERT_EQUAL (RE_SUCCESS, err_code);
    TEST_ASSERT_FLOAT_WITHIN (0.01f, RE_7_TEMP_MIN, decoded_data.temperature_c);
    TEST_ASSERT_FLOAT_WITHIN (0.01f, RE_7_HUMI_MIN, decoded_data.humidity_rh);
    TEST_ASSERT_FLOAT_WITHIN (1.0f, RE_7_PRES_MIN, decoded_data.pressure_pa);
    TEST_ASSERT_FLOAT_WITHIN (1.0f, RE_7_TILT_MIN, decoded_data.tilt_x_deg);  /* acc_x=-1 -> -90° */
    TEST_ASSERT_FLOAT_WITHIN (1.0f, 0.0f, decoded_data.tilt_y_deg);           /* acc_y=0 -> 0° */
    TEST_ASSERT_FLOAT_WITHIN (1.0f, RE_7_LUMI_MIN, decoded_data.luminosity_lux);
    TEST_ASSERT_EQUAL_UINT8 (RE_7_MOTION_CNT_MIN, decoded_data.motion_count);
    TEST_ASSERT_EQUAL_UINT8 (RE_7_SEQCTR_MIN, decoded_data.sequence_counter);
    TEST_ASSERT_EQUAL (m_re_7_data_ok_min.motion_detected, decoded_data.motion_detected);
    TEST_ASSERT_EQUAL (m_re_7_data_ok_min.presence_detected, decoded_data.presence_detected);
}

/**
 * @brief Invalid data encodes to invalid markers
 */
void test_ruuvi_endpoint_7_encode_invalid_data (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[RE_7_DATA_LENGTH] = {0};
    err_code = re_7_encode (test_buffer, &m_re_7_data_invalid);
    TEST_ASSERT_EQUAL (RE_SUCCESS, err_code);
    /* Verify invalid temperature marker */
    uint16_t temp = (test_buffer[RE_7_OFFSET_TEMP_MSB] << 8) | test_buffer[RE_7_OFFSET_TEMP_LSB];
    TEST_ASSERT_EQUAL_HEX16 (RE_7_INVALID_TEMPERATURE, temp);
    /* Verify invalid humidity marker */
    uint16_t humi = (test_buffer[RE_7_OFFSET_HUMI_MSB] << 8) | test_buffer[RE_7_OFFSET_HUMI_LSB];
    TEST_ASSERT_EQUAL_HEX16 (RE_7_INVALID_HUMIDITY, humi);
    /* Verify invalid pressure marker */
    uint16_t pres = (test_buffer[RE_7_OFFSET_PRES_MSB] << 8) | test_buffer[RE_7_OFFSET_PRES_LSB];
    TEST_ASSERT_EQUAL_HEX16 (RE_7_INVALID_PRESSURE, pres);
    /* Verify invalid luminosity marker */
    uint16_t lumi = (test_buffer[RE_7_OFFSET_LUMI_MSB] << 8) | test_buffer[RE_7_OFFSET_LUMI_LSB];
    TEST_ASSERT_EQUAL_HEX16 (RE_7_INVALID_LUMINOSITY, lumi);
    /* Verify invalid sequence counter */
    TEST_ASSERT_EQUAL_HEX8 (RE_7_INVALID_SEQUENCE, test_buffer[RE_7_OFFSET_SEQ]);
    /* Verify invalid motion count */
    TEST_ASSERT_EQUAL_HEX8 (RE_7_INVALID_MOTION_COUNT, test_buffer[RE_7_OFFSET_MOTION_CNT]);
}

/**
 * @brief Decode returns NAN for invalid values
 */
void test_ruuvi_endpoint_7_decode_invalid_returns_nan (void)
{
    uint8_t test_buffer[RE_7_DATA_LENGTH] = {0};
    re_7_encode (test_buffer, &m_re_7_data_invalid);
    uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x17, 0xFF, 0x99, 0x04};
    memcpy (&raw_buf[7], test_buffer, sizeof (test_buffer));
    re_7_data_t decoded_data = {0};
    re_status_t err_code = re_7_decode (raw_buf, &decoded_data);
    TEST_ASSERT_EQUAL (RE_SUCCESS, err_code);
    TEST_ASSERT_TRUE (isnan (decoded_data.temperature_c));
    TEST_ASSERT_TRUE (isnan (decoded_data.humidity_rh));
    TEST_ASSERT_TRUE (isnan (decoded_data.pressure_pa));
    TEST_ASSERT_TRUE (isnan (decoded_data.luminosity_lux));
    TEST_ASSERT_TRUE (isnan (decoded_data.battery_v));
}

/**
 * @brief Underflow values are clamped to minimum
 */
void test_ruuvi_endpoint_7_encode_underflow (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[RE_7_DATA_LENGTH] = {0};
    err_code = re_7_encode (test_buffer, &m_re_7_data_underflow);
    TEST_ASSERT_EQUAL (RE_SUCCESS, err_code);
    uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x17, 0xFF, 0x99, 0x04};
    memcpy (&raw_buf[7], test_buffer, sizeof (test_buffer));
    re_7_data_t decoded_data = {0};
    err_code = re_7_decode (raw_buf, &decoded_data);
    TEST_ASSERT_EQUAL (RE_SUCCESS, err_code);
    /* Should be clamped to min values */
    TEST_ASSERT_FLOAT_WITHIN (0.01f, RE_7_TEMP_MIN, decoded_data.temperature_c);
    TEST_ASSERT_FLOAT_WITHIN (0.01f, RE_7_HUMI_MIN, decoded_data.humidity_rh);
    TEST_ASSERT_FLOAT_WITHIN (1.0f, RE_7_PRES_MIN, decoded_data.pressure_pa);
    TEST_ASSERT_FLOAT_WITHIN (1.0f, RE_7_TILT_MIN, decoded_data.tilt_x_deg);
    TEST_ASSERT_FLOAT_WITHIN (1.0f, 0.0f, decoded_data.tilt_y_deg);
    TEST_ASSERT_FLOAT_WITHIN (1.0f, RE_7_LUMI_MIN, decoded_data.luminosity_lux);
    TEST_ASSERT_FLOAT_WITHIN (0.15f, RE_7_VOLTAGE_MIN, decoded_data.battery_v);
}

/**
 * @brief Overflow values are clamped to maximum
 */
void test_ruuvi_endpoint_7_encode_overflow (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[RE_7_DATA_LENGTH] = {0};
    err_code = re_7_encode (test_buffer, &m_re_7_data_overflow);
    TEST_ASSERT_EQUAL (RE_SUCCESS, err_code);
    uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x17, 0xFF, 0x99, 0x04};
    memcpy (&raw_buf[7], test_buffer, sizeof (test_buffer));
    re_7_data_t decoded_data = {0};
    err_code = re_7_decode (raw_buf, &decoded_data);
    TEST_ASSERT_EQUAL (RE_SUCCESS, err_code);
    /* Should be clamped to max values */
    TEST_ASSERT_FLOAT_WITHIN (0.01f, RE_7_TEMP_MAX, decoded_data.temperature_c);
    TEST_ASSERT_FLOAT_WITHIN (0.01f, RE_7_HUMI_MAX, decoded_data.humidity_rh);
    TEST_ASSERT_FLOAT_WITHIN (1.0f, RE_7_PRES_MAX, decoded_data.pressure_pa);
    TEST_ASSERT_FLOAT_WITHIN (1.0f, RE_7_TILT_MAX, decoded_data.tilt_x_deg);
    TEST_ASSERT_FLOAT_WITHIN (1.0f, 0.0f, decoded_data.tilt_y_deg);
    TEST_ASSERT_FLOAT_WITHIN (1.0f, RE_7_LUMI_MAX, decoded_data.luminosity_lux);
    TEST_ASSERT_FLOAT_WITHIN (0.15f, RE_7_VOLTAGE_MAX, decoded_data.battery_v);
}

/**
 * @brief Null buffer returns error
 */
void test_ruuvi_endpoint_7_encode_null_buffer (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t * const p_test_buffer = NULL;
    err_code = re_7_encode (p_test_buffer, &m_re_7_data_ok);
    TEST_ASSERT_EQUAL (RE_ERROR_NULL, err_code);
}

/**
 * @brief Null data returns error
 */
void test_ruuvi_endpoint_7_encode_null_data (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t test_buffer[RE_7_DATA_LENGTH] = {0};
    const re_7_data_t * p_re_7_data = NULL;
    err_code = re_7_encode (test_buffer, p_re_7_data);
    TEST_ASSERT_EQUAL (RE_ERROR_NULL, err_code);
}

/**
 * @brief Null buffer in decode returns error
 */
void test_ruuvi_endpoint_7_decode_null_buffer (void)
{
    re_7_data_t decoded_data = {0};
    re_status_t err_code = re_7_decode (NULL, &decoded_data);
    TEST_ASSERT_EQUAL (RE_ERROR_NULL, err_code);
}

/**
 * @brief Null data in decode returns error
 */
void test_ruuvi_endpoint_7_decode_null_data (void)
{
    uint8_t raw_buf[31] = {0};
    re_status_t err_code = re_7_decode (raw_buf, NULL);
    TEST_ASSERT_EQUAL (RE_ERROR_NULL, err_code);
}

/**
 * @brief Wrong format header returns error
 */
void test_ruuvi_endpoint_7_decode_wrong_format (void)
{
    uint8_t test_buffer[RE_7_DATA_LENGTH] = {0};
    re_7_encode (test_buffer, &m_re_7_data_ok);
    test_buffer[RE_7_OFFSET_HEADER] = 0x05U;  /* Wrong format */
    uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x17, 0xFF, 0x99, 0x04};
    memcpy (&raw_buf[7], test_buffer, sizeof (test_buffer));
    re_7_data_t decoded_data = {0};
    re_status_t err_code = re_7_decode (raw_buf, &decoded_data);
    TEST_ASSERT_EQUAL (RE_ERROR_INVALID_PARAM, err_code);
}

/**
 * @brief CRC mismatch returns error
 */
void test_ruuvi_endpoint_7_decode_crc_error (void)
{
    uint8_t test_buffer[RE_7_DATA_LENGTH] = {0};
    re_7_encode (test_buffer, &m_re_7_data_ok);
    /* Corrupt the CRC */
    test_buffer[RE_7_OFFSET_CRC] ^= 0xFFU;
    uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x17, 0xFF, 0x99, 0x04};
    memcpy (&raw_buf[7], test_buffer, sizeof (test_buffer));
    re_7_data_t decoded_data = {0};
    re_status_t err_code = re_7_decode (raw_buf, &decoded_data);
    TEST_ASSERT_EQUAL (RE_ERROR_DECODING_CRC, err_code);
}

/**
 * @brief Check format returns true for valid DF7
 */
void test_ruuvi_endpoint_7_check_format_ok (void)
{
    const uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x17, 0xFF, 0x99, 0x04, 0x07};
    TEST_ASSERT_TRUE (re_7_check_format (raw_buf));
}

/**
 * @brief Check format returns false for wrong format header
 */
void test_ruuvi_endpoint_7_check_format_wrong_header (void)
{
    const uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x17, 0xFF, 0x99, 0x04, 0x05};
    TEST_ASSERT_FALSE (re_7_check_format (raw_buf));
}

/**
 * @brief Check format returns false for wrong manufacturer ID
 */
void test_ruuvi_endpoint_7_check_format_wrong_manufacturer (void)
{
    const uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x17, 0xFF, 0x9A, 0x04, 0x07};
    TEST_ASSERT_FALSE (re_7_check_format (raw_buf));
}

/**
 * @brief Check format returns false for wrong length
 */
void test_ruuvi_endpoint_7_check_format_wrong_length (void)
{
    const uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x18, 0xFF, 0x99, 0x04, 0x07};
    TEST_ASSERT_FALSE (re_7_check_format (raw_buf));
}

/**
 * @brief Check format returns false for NULL
 */
void test_ruuvi_endpoint_7_check_format_null (void)
{
    TEST_ASSERT_FALSE (re_7_check_format (NULL));
}

/**
 * @brief Flags encode/decode motion only
 */
void test_ruuvi_endpoint_7_flags_motion_only (void)
{
    re_7_data_t data = m_re_7_data_ok;
    data.motion_detected = true;
    data.presence_detected = false;
    uint8_t test_buffer[RE_7_DATA_LENGTH] = {0};
    re_7_encode (test_buffer, &data);
    TEST_ASSERT_EQUAL_HEX8 (0x01U, test_buffer[RE_7_OFFSET_FLAGS]);
    uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x17, 0xFF, 0x99, 0x04};
    memcpy (&raw_buf[7], test_buffer, sizeof (test_buffer));
    re_7_data_t decoded = {0};
    re_7_decode (raw_buf, &decoded);
    TEST_ASSERT_TRUE (decoded.motion_detected);
    TEST_ASSERT_FALSE (decoded.presence_detected);
}

/**
 * @brief Flags encode/decode presence only
 */
void test_ruuvi_endpoint_7_flags_presence_only (void)
{
    re_7_data_t data = m_re_7_data_ok;
    data.motion_detected = false;
    data.presence_detected = true;
    uint8_t test_buffer[RE_7_DATA_LENGTH] = {0};
    re_7_encode (test_buffer, &data);
    TEST_ASSERT_EQUAL_HEX8 (0x02U, test_buffer[RE_7_OFFSET_FLAGS]);
    uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x17, 0xFF, 0x99, 0x04};
    memcpy (&raw_buf[7], test_buffer, sizeof (test_buffer));
    re_7_data_t decoded = {0};
    re_7_decode (raw_buf, &decoded);
    TEST_ASSERT_FALSE (decoded.motion_detected);
    TEST_ASSERT_TRUE (decoded.presence_detected);
}

/**
 * @brief Flags encode/decode neither flag
 */
void test_ruuvi_endpoint_7_flags_none (void)
{
    re_7_data_t data = m_re_7_data_ok;
    data.motion_detected = false;
    data.presence_detected = false;
    uint8_t test_buffer[RE_7_DATA_LENGTH] = {0};
    re_7_encode (test_buffer, &data);
    TEST_ASSERT_EQUAL_HEX8 (0x00U, test_buffer[RE_7_OFFSET_FLAGS]);
    uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x17, 0xFF, 0x99, 0x04};
    memcpy (&raw_buf[7], test_buffer, sizeof (test_buffer));
    re_7_data_t decoded = {0};
    re_7_decode (raw_buf, &decoded);
    TEST_ASSERT_FALSE (decoded.motion_detected);
    TEST_ASSERT_FALSE (decoded.presence_detected);
}

/**
 * @brief Color temperature encoding/decoding
 */
void test_ruuvi_endpoint_7_color_temp (void)
{
    re_7_data_t data = m_re_7_data_ok;
    /* Test 1000K (min) */
    data.color_temp_k = 1000U;
    uint8_t test_buffer[RE_7_DATA_LENGTH] = {0};
    re_7_encode (test_buffer, &data);
    TEST_ASSERT_EQUAL_HEX8 (0x00U, test_buffer[RE_7_OFFSET_COLOR_TEMP]);
    /* Test ~4000K */
    data.color_temp_k = 4000U;
    re_7_encode (test_buffer, &data);
    /* (4000 - 1000) / 26 = 115.38 -> 115 */
    TEST_ASSERT_EQUAL_HEX8 (115U, test_buffer[RE_7_OFFSET_COLOR_TEMP]);
    /* Test max (255 * 26 + 1000 = 7630K, clamped to 7650K) */
    data.color_temp_k = 7650U;
    re_7_encode (test_buffer, &data);
    /* (7650 - 1000) / 26 = 255.76 -> 255 */
    TEST_ASSERT_EQUAL_HEX8 (255U, test_buffer[RE_7_OFFSET_COLOR_TEMP]);
}

/**
 * @brief Battery and motion intensity packed encoding
 */
void test_ruuvi_endpoint_7_batt_motion_packed (void)
{
    re_7_data_t data = m_re_7_data_ok;
    data.battery_v = 2.7f;  /* ~7.5 in 4-bit scale */
    data.motion_intensity = 10U;
    uint8_t test_buffer[RE_7_DATA_LENGTH] = {0};
    re_7_encode (test_buffer, &data);
    uint8_t packed = test_buffer[RE_7_OFFSET_BATT_MOTION];
    uint8_t batt_nibble = (packed >> 4) & 0x0F;
    uint8_t motion_nibble = packed & 0x0F;
    /* Battery: (2.7 - 1.8) * (15/1.8) = 7.5 -> 8 */
    TEST_ASSERT_UINT8_WITHIN (1U, 8U, batt_nibble);
    TEST_ASSERT_EQUAL_UINT8 (10U, motion_nibble);
}
/**
 * @brief Acceleration to tilt calculation - device flat (Z pointing up)
 */
void test_ruuvi_endpoint_7_accel_to_tilt_flat (void)
{
    re_7_data_t data = m_re_7_data_ok;
    /* Device flat: acc = (0, 0, 1) -> tilt = (0°, 0°) */
    data.acceleration_x_g = 0.0f;
    data.acceleration_y_g = 0.0f;
    data.acceleration_z_g = 1.0f;
    uint8_t test_buffer[RE_7_DATA_LENGTH] = {0};
    re_status_t err_code = re_7_encode (test_buffer, &data);
    TEST_ASSERT_EQUAL (RE_SUCCESS, err_code);
    uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x17, 0xFF, 0x99, 0x04};
    memcpy (&raw_buf[7], test_buffer, sizeof (test_buffer));
    re_7_data_t decoded_data = {0};
    re_7_decode (raw_buf, &decoded_data);
    TEST_ASSERT_FLOAT_WITHIN (1.0f, 0.0f, decoded_data.tilt_x_deg);
    TEST_ASSERT_FLOAT_WITHIN (1.0f, 0.0f, decoded_data.tilt_y_deg);
}

/**
 * @brief Acceleration to tilt calculation - device tilted 45° in X
 */
void test_ruuvi_endpoint_7_accel_to_tilt_45deg (void)
{
    re_7_data_t data = m_re_7_data_ok;
    /* Device tilted 45° in X: acc = (sin(45°), 0, cos(45°)) ≈ (0.707, 0, 0.707) */
    data.acceleration_x_g = 0.7071f;
    data.acceleration_y_g = 0.0f;
    data.acceleration_z_g = 0.7071f;
    uint8_t test_buffer[RE_7_DATA_LENGTH] = {0};
    re_status_t err_code = re_7_encode (test_buffer, &data);
    TEST_ASSERT_EQUAL (RE_SUCCESS, err_code);
    uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x17, 0xFF, 0x99, 0x04};
    memcpy (&raw_buf[7], test_buffer, sizeof (test_buffer));
    re_7_data_t decoded_data = {0};
    re_7_decode (raw_buf, &decoded_data);
    TEST_ASSERT_FLOAT_WITHIN (1.5f, 45.0f, decoded_data.tilt_x_deg);
    TEST_ASSERT_FLOAT_WITHIN (1.0f, 0.0f, decoded_data.tilt_y_deg);
}

/**
 * @brief Acceleration to tilt calculation - higher than 1G magnitude (should normalize)
 */
void test_ruuvi_endpoint_7_accel_to_tilt_high_g (void)
{
    re_7_data_t data = m_re_7_data_ok;
    /* 2G magnitude pointing in X: acc = (2, 0, 0) -> normalized (1, 0, 0) -> tilt = 90° */
    data.acceleration_x_g = 2.0f;
    data.acceleration_y_g = 0.0f;
    data.acceleration_z_g = 0.0f;
    uint8_t test_buffer[RE_7_DATA_LENGTH] = {0};
    re_status_t err_code = re_7_encode (test_buffer, &data);
    TEST_ASSERT_EQUAL (RE_SUCCESS, err_code);
    uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x17, 0xFF, 0x99, 0x04};
    memcpy (&raw_buf[7], test_buffer, sizeof (test_buffer));
    re_7_data_t decoded_data = {0};
    re_7_decode (raw_buf, &decoded_data);
    TEST_ASSERT_FLOAT_WITHIN (1.5f, 90.0f, decoded_data.tilt_x_deg);
    TEST_ASSERT_FLOAT_WITHIN (1.0f, 0.0f, decoded_data.tilt_y_deg);
}

/**
 * @brief Acceleration to tilt calculation - zero magnitude (should output invalid)
 */
void test_ruuvi_endpoint_7_accel_to_tilt_zero_mag (void)
{
    re_7_data_t data = m_re_7_data_ok;
    /* Zero acceleration: cannot determine tilt */
    data.acceleration_x_g = 0.0f;
    data.acceleration_y_g = 0.0f;
    data.acceleration_z_g = 0.0f;
    uint8_t test_buffer[RE_7_DATA_LENGTH] = {0};
    re_status_t err_code = re_7_encode (test_buffer, &data);
    TEST_ASSERT_EQUAL (RE_SUCCESS, err_code);
    uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x17, 0xFF, 0x99, 0x04};
    memcpy (&raw_buf[7], test_buffer, sizeof (test_buffer));
    re_7_data_t decoded_data = {0};
    re_7_decode (raw_buf, &decoded_data);
    /* Zero magnitude should result in invalid (NAN) tilt */
    TEST_ASSERT_TRUE (isnan (decoded_data.tilt_x_deg));
    TEST_ASSERT_TRUE (isnan (decoded_data.tilt_y_deg));
}

/**
 * @brief Acceleration to tilt calculation - negative tilt
 */
void test_ruuvi_endpoint_7_accel_to_tilt_negative (void)
{
    re_7_data_t data = m_re_7_data_ok;
    /* Device tilted -30° in Y: acc = (0, sin(-30°), cos(-30°)) ≈ (0, -0.5, 0.866) */
    data.acceleration_x_g = 0.0f;
    data.acceleration_y_g = -0.5f;
    data.acceleration_z_g = 0.866f;
    uint8_t test_buffer[RE_7_DATA_LENGTH] = {0};
    re_status_t err_code = re_7_encode (test_buffer, &data);
    TEST_ASSERT_EQUAL (RE_SUCCESS, err_code);
    uint8_t raw_buf[31] = {0x02, 0x01, 0x04, 0x17, 0xFF, 0x99, 0x04};
    memcpy (&raw_buf[7], test_buffer, sizeof (test_buffer));
    re_7_data_t decoded_data = {0};
    re_7_decode (raw_buf, &decoded_data);
    TEST_ASSERT_FLOAT_WITHIN (1.0f, 0.0f, decoded_data.tilt_x_deg);
    TEST_ASSERT_FLOAT_WITHIN (1.5f, -30.0f, decoded_data.tilt_y_deg);
}