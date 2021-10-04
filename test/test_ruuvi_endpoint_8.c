#ifdef TEST

#include "unity.h"
#include <string.h>
#include "ruuvi_endpoints.h"
#include "ruuvi_endpoint_8.h"

void setUp (void)
{
}

void tearDown (void)
{
}

// "RuuviComAABBCCDDEEFF0011"
static const uint8_t TEST_KEY[RE_8_CIPHERTEXT_LENGTH] =
{
    0x52, 0x75, 0x75, 0x76, 0x69, 0x43, 0x6F, 0x6D,
    0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11
};

static const re_8_data_t m_re_8_data =
{
    .humidity_rh = 53.49,
    .pressure_pa = 100044,
    .temperature_c = 24.3,
    .battery_v = 2.977,
    .message_counter = 205,
    .movement_count = 6607,
    .address = 0xCBB8334C884F,
    .tx_power = 4
};

static const uint8_t TEST_VECT_IN[RE_8_CIPHERTEXT_LENGTH] =
{
    0x12, 0xFC, // Temperature
    0x53, 0x94, // Humidity
    0xC3, 0x7C, // Pressure
    0xAC, 0x36, // Power
    0x19, 0xCF, // Movement
    0x00, 0xCD, // Measurement
    0x00, 0x00, // Reserved
    0x00, 0x00, // Reserved
};

// "gaTivuuRmoCivuuR"
static const uint8_t TEST_VECT_OUT[RE_8_CIPHERTEXT_LENGTH] =
{
    0x67, 0x61, 0x54, 0x69, 0x76, 0x75, 0x75, 0x52,
    0x6D, 0x6F, 0x43, 0x69, 0x76, 0x75, 0x75, 0x52
};

static const uint8_t TEST_DATA_OK[RE_8_DATA_LENGTH] =
{
    0x08,                                              // Header
    0x67, 0x61, 0x54, 0x69, 0x76, 0x75, 0x75, 0x52,    // Encrypted
    0x6D, 0x6F, 0x43, 0x69, 0x76, 0x75, 0x75, 0x52,    // Encrypted
    0x3F, 0xCB, 0xB8, 0x33, 0x4C, 0x88, 0x4F           // CRC + MAC
};

/**
 * Return test vector values
 */
uint32_t mock_encrypt (const uint8_t * const cleartext,
                       uint8_t * const ciphertext,
                       const size_t data_size,
                       const uint8_t * const key,
                       const size_t key_size)
{
    uint32_t ret_code = 0;

    if (memcmp (key, TEST_KEY, RE_8_CIPHERTEXT_LENGTH))
    {
        ret_code = 1;
    }
    else if (memcmp (cleartext, TEST_VECT_IN, RE_8_CIPHERTEXT_LENGTH))
    {
        ret_code = 2;
    }
    else
    {
        memcpy (ciphertext, TEST_VECT_OUT, RE_8_CIPHERTEXT_LENGTH);
    }

    return ret_code;
}

void test_re_8_encode_ok (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t buffer[24] = {0};
    err_code = re_8_encode (buffer, &m_re_8_data, &mock_encrypt, TEST_KEY,
                            RE_8_CIPHERTEXT_LENGTH);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (!memcmp (buffer, TEST_DATA_OK, RE_8_DATA_LENGTH));
}

#endif // TEST
