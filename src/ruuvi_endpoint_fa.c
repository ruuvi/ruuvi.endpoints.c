#include "ruuvi_endpoint_fa.h"
#include "ruuvi_endpoint_3.h"
#include <string.h>

#define RE_FA_INVALID_MAC (0xFFFFFFFFFFFFU)
#define RE_FA_ENCODE_MAC_MAX (0xFFFFFFFFFFFEU)
#define RE_FA_ENCODE_MAC_MIN (0U)

static void fill_re_3 (re_3_data_t * const re_3_data,
                       const re_fa_data_t * const re_fa_data)
{
    re_3_data->humidity_rh     = re_fa_data->humidity_rh;
    re_3_data->pressure_pa     = re_fa_data->pressure_pa;
    re_3_data->temperature_c   = re_fa_data->temperature_c;
    re_3_data->accelerationx_g = re_fa_data->accelerationx_g;
    re_3_data->accelerationy_g = re_fa_data->accelerationy_g;
    re_3_data->accelerationz_g = re_fa_data->accelerationz_g;
    re_3_data->battery_v       = re_fa_data->battery_v;
}

static void re_fa_encode_set_address (uint8_t * const buffer,
                                      const re_fa_data_t * data)
{
    uint64_t mac = data->address;

    if ( (RE_FA_ENCODE_MAC_MAX < data->address) ||
            (RE_FA_ENCODE_MAC_MIN > data->address))
    {
        mac = RE_FA_INVALID_MAC;
    }

    for (int8_t offset = 5; offset >= 0; offset--)
    {
        buffer[RE_FA_OFFSET_ADDRESS_LSB - offset] = (mac >> (offset * 8)) & (0xFFU);
    }
}

/**
 * @brief Encode data to Ruuvi Format FA.
 *
 * @param[out] buffer 23-byte buffer into which data is encoded.
 * @param[in]  data Data to encode.
 * @param[in] invalid re_float value treated as not available. NAN recommended.
 * @retval RE_SUCCESS if data was encoded successfully.
 */
re_status_t re_fa_encode (uint8_t * const buffer,
                          const re_fa_data_t * const data,
                          const re_float invalid,
                          re_fa_encrypt_fp cipher,
                          const uint8_t * const key,
                          const size_t key_size)
{
    re_status_t encoding_status = RE_SUCCESS;
    uint32_t cipher_status = 0;
    re_3_data_t re_3_data = {0};
    uint8_t ciphertext[RE_FA_CIPHERTEXT_LENGTH];
    fill_re_3 (&re_3_data, data);
    encoding_status |= re_3_encode (buffer, &re_3_data, invalid);
    buffer[0] = RE_FA_DESTINATION;
    buffer[RE_FA_OFFSET_COUNTER] = data->message_counter;
    buffer[RE_FA_OFFSET_TRAILING_NULL_1] = 0;
    buffer[RE_FA_OFFSET_TRAILING_NULL_2] = 0;
    cipher_status = cipher (buffer + 1, ciphertext, RE_FA_CIPHERTEXT_LENGTH, key, 
                            key_size);

    if(0 != cipher_status)
    {
        encoding_status |= RE_ERROR_ENCODING;
    }

    memcpy (buffer + 1, ciphertext, RE_FA_CIPHERTEXT_LENGTH);
    re_fa_encode_set_address(buffer, data);
    return encoding_status;
}