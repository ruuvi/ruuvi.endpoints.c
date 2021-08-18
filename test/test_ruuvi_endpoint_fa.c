#ifdef TEST

#include "unity.h"
#include <string.h>
#include "ruuvi_endpoints.h"
#include "ruuvi_endpoint_fa.h"
#include "mock_ruuvi_endpoint_3.h"

void setUp (void)
{
}

void tearDown (void)
{
}

// "RuuviComRuuviTag"
static const uint8_t TEST_KEY[RE_FA_CIPHERTEXT_LENGTH] =
{
    0x52, 0x75, 0x75, 0x76, 0x69, 0x43, 0x6F, 0x6D,
    0x52, 0x75, 0x75, 0x76, 0x69, 0x54, 0x61, 0x67
};

// "RuuviComRuuvi\0x01\0x00\0x00"
static const uint8_t TEST_VECT_IN[RE_FA_CIPHERTEXT_LENGTH] =
{
    0x52, 0x75, 0x75, 0x76, 0x69, 0x43, 0x6F, 0x6D,
    0x52, 0x75, 0x75, 0x76, 0x69, 0x01, 0x00, 0x00
};

// "gaTivuuRmoCivuuR"
static const uint8_t TEST_VECT_OUT[RE_FA_CIPHERTEXT_LENGTH] =
{
    0x67, 0x61, 0x54, 0x69, 0x76, 0x75, 0x75, 0x52,
    0x6D, 0x6F, 0x43, 0x69, 0x76, 0x75, 0x75, 0x52
};

static const uint8_t TEST_DATA_OK[RE_FA_DATA_LENGTH] =
{
    0xFA,
    0x67, 0x61, 0x54, 0x69, 0x76, 0x75, 0x75, 0x52,
    0x6D, 0x6F, 0x43, 0x69, 0x76, 0x75, 0x75, 0x52,
    0xC0, 0x00, 0xDE, 0xAD, 0xBE, 0xEF
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

    if (memcmp (key, TEST_KEY, RE_FA_CIPHERTEXT_LENGTH))
    {
        ret_code = 1;
    }
    else if (memcmp (cleartext, TEST_VECT_IN, RE_FA_CIPHERTEXT_LENGTH))
    {
        ret_code = 2;
    }
    else
    {
        memcpy (ciphertext, TEST_VECT_OUT, RE_FA_CIPHERTEXT_LENGTH);
    }

    return ret_code;
}

/**
 * @brief Encode data to Ruuvi Format FA.
 *
 * @param[out] buffer 23-byte buffer into which data is encoded.
 * @param[in]  data Data to encode.
 * @param[in]  invalid re_float value treated as not available. NAN recommended.
 * @param[in]  Encryption key for data format.
 * @param[in]
 * @retval RE_SUCCESS if data was encoded successfully.
 */
void test_re_fa_encode_ok (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t buffer[23] = {0};
    static uint8_t re_3_value[RE_3_DATA_LENGTH] = {0};
    memcpy (re_3_value + 1, TEST_VECT_IN, RE_3_DATA_LENGTH - 1);
    re_fa_data_t data =
    {
        .address = 0xC000DEADBEEF,
        .message_counter = 1
    };
    re_3_encode_ExpectAndReturn (buffer, NULL, NAN, RE_SUCCESS);
    re_3_encode_IgnoreArg_data();
    re_3_encode_ReturnArrayThruPtr_buffer (re_3_value, RE_3_DATA_LENGTH);
    err_code = re_fa_encode (buffer, &data, NAN, &mock_encrypt, TEST_KEY,
                             RE_FA_CIPHERTEXT_LENGTH);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (!memcmp (buffer, TEST_DATA_OK, RE_FA_DATA_LENGTH));
}

#endif // TEST
