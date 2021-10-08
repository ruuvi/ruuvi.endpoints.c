#include "ruuvi_endpoints.h"
#include "ruuvi_endpoint_8.h"
#include <math.h>
#include <string.h>

#define RE_8_INVALID_MAC    (0xFFFFFFFFFFFFU)
#define RE_8_ENCODE_MAC_MAX (0xFFFFFFFFFFFEU)

#define RE_8_ACC_RATIO           (1000.0f)
#define RE_8_HUMI_RATIO          (400.0f)
#define RE_8_TEMP_RATIO          (200.0f)
#define RE_8_PRES_RATIO          (1.0f)
#define RE_8_PRES_OFFSET         (-50000.0f)
#define RE_8_BATT_RATIO          (1000.0f)
#define RE_8_BATT_OFFSET         (1600)
#define RE_8_BATT_MIN            (1.6f)
#define RE_8_TXPWR_RATIO         (2)
#define RE_8_TXPWR_OFFSET        (40)
#define RE_8_BYTE_VOLTAGE_OFFSET (5U)

static void re_8_encode_set_address (uint8_t * const buffer,
                                     const re_8_data_t * data)
{
    uint64_t mac = data->address;

    if (RE_8_ENCODE_MAC_MAX < data->address)
    {
        mac = RE_8_INVALID_MAC;
    }

    for (int8_t offset = 5; offset >= 0; offset--)
    {
        buffer[RE_8_OFFSET_ADDR_LSB - offset] = (mac >> (offset * 8)) & (0xFFU);
    }
}

static void re_8_encode_humidity (uint8_t * const buffer, const re_8_data_t * data)
{
    uint16_t coded_humidity = RE_8_INVALID_HUMIDITY;
    float humidity = data->humidity_rh;

    if (!isnan (humidity))
    {
        re_clip (&humidity, RE_8_HUMI_MIN, RE_8_HUMI_MAX);
        coded_humidity = (uint16_t) roundf (humidity * RE_8_HUMI_RATIO);
    }

    buffer[RE_8_OFFSET_HUMI_MSB] = coded_humidity >> 8U;
    buffer[RE_8_OFFSET_HUMI_LSB] = coded_humidity & 0xFFU;
}

static void re_8_encode_temperature (uint8_t * const buffer, const re_8_data_t * data)
{
    uint16_t coded_temperature = RE_8_INVALID_TEMPERATURE;
    float temperature = data->temperature_c;

    if (!isnan (temperature))
    {
        re_clip (&temperature, RE_8_TEMP_MIN, RE_8_TEMP_MAX);
        coded_temperature = (uint16_t) roundf (temperature * RE_8_TEMP_RATIO);
    }

    buffer[RE_8_OFFSET_TEMP_MSB] = coded_temperature >> 8U;
    buffer[RE_8_OFFSET_TEMP_LSB] = coded_temperature & 0xFFU;
}

static void re_8_encode_pressure (uint8_t * const buffer, const re_8_data_t * data)
{
    uint16_t coded_pressure = RE_8_INVALID_PRESSURE;
    float pressure = data->pressure_pa;

    if (!isnan (pressure))
    {
        re_clip (&pressure, RE_8_PRES_MIN, RE_8_PRES_MAX);
        pressure += RE_8_PRES_OFFSET;
        coded_pressure = (uint16_t) roundf (pressure * RE_8_PRES_RATIO);
    }

    buffer[RE_8_OFFSET_PRES_MSB] = coded_pressure >> 8U;
    buffer[RE_8_OFFSET_PRES_LSB] = coded_pressure & 0xFFU;
}

static void re_8_encode_pwr (uint8_t * const buffer, const re_8_data_t * data)
{
    uint16_t coded_voltage = RE_8_INVALID_VOLTAGE;
    float voltage = data->battery_v;
    uint16_t coded_tx_power = RE_8_INVALID_POWER;
    float tx_power = (float) data->tx_power;

    if (!isnan (voltage))
    {
        re_clip (&voltage, RE_8_VOLTAGE_MIN, RE_8_VOLTAGE_MAX);
        coded_voltage = (uint16_t) roundf ( (voltage * RE_8_BATT_RATIO)
                                            - RE_8_BATT_OFFSET);
    }

    // Check against original int value
    if (RE_8_INVALID_POWER != data->tx_power)
    {
        re_clip (&tx_power, RE_8_TXPWR_MIN, RE_8_TXPWR_MAX);
        coded_tx_power = (uint16_t) roundf ( (tx_power
                                              + RE_8_TXPWR_OFFSET)
                                             / RE_8_TXPWR_RATIO);
    }

    uint16_t power_info = ( (uint16_t) (coded_voltage << RE_8_BYTE_VOLTAGE_OFFSET))
                          + coded_tx_power;
    buffer[RE_8_OFFSET_POWER_MSB] = (power_info >> 8U);
    buffer[RE_8_OFFSET_POWER_LSB] = (power_info & 0xFFU);
}

static void re_8_encode_movement (uint8_t * const buffer, const re_8_data_t * data)
{
    uint16_t movement_count = RE_8_INVALID_MOVEMENT;

    if (RE_8_MVTCTR_MAX >= data->movement_count)
    {
        movement_count = data->movement_count;
    }

    buffer[RE_8_OFFSET_MVTCTR_MSB] = (movement_count >> 8U);
    buffer[RE_8_OFFSET_MVTCTR_LSB] = (movement_count & 0xFFU);
}

static void re_8_encode_sequence (uint8_t * const buffer, const re_8_data_t * data)
{
    uint16_t measurement_seq = RE_8_INVALID_SEQUENCE;

    if (RE_8_SEQCTR_MAX >= data->message_counter)
    {
        measurement_seq = data->message_counter;
    }

    buffer[RE_8_OFFSET_SEQCTR_MSB] = (measurement_seq >> 8U);
    buffer[RE_8_OFFSET_SEQCTR_LSB] = (measurement_seq & 0xFFU);
}

static void re_8_encode_reserved (uint8_t * const buffer)
{
    for (size_t ii = 0; ii < RE_8_RESERVED_BYTES; ii++)
    {
        buffer[RE_8_OFFSET_RESERVED + ii] = 0;
    }
}

static void re_8_encode_crc (uint8_t * const buffer)
{
    buffer[RE_8_OFFSET_CRC8] = re_calc_crc8 (buffer + RE_8_OFFSET_CIPHER,
                               RE_8_CIPHERTEXT_LENGTH);
}

/**
 * @brief Encode data to Ruuvi Format 8.
 *
 * @param[out] buffer 24-byte buffer into which data is encoded.
 * @param[in]  data Data to encode. Use NAN for values not available.
 * @param[in]  cipher Pointer to encryption function.
 * @param[in]  key Encryption key for data format.
 * @param[in]  key_size Encryption key length in bytes.
 * @retval RE_SUCCESS if data was encoded successfully.
 */
re_status_t re_8_encode (uint8_t * const buffer,
                         const re_8_data_t * const data,
                         re_8_encrypt_fp cipher,
                         const uint8_t * const key,
                         const size_t key_size)
{
    re_status_t result = RE_SUCCESS;

    if ( (NULL == buffer) || (NULL == data) || (NULL == cipher) || (NULL == key))
    {
        result |= RE_ERROR_NULL;
    }
    else
    {
        uint32_t cipher_status = 0;
        uint8_t cleartext[RE_8_CIPHERTEXT_LENGTH] = {0};
        buffer[RE_8_OFFSET_HEADER] = RE_8_DESTINATION;
        re_8_encode_humidity (buffer, data);
        re_8_encode_temperature (buffer, data);
        re_8_encode_pressure (buffer, data);
        re_8_encode_movement (buffer, data);
        re_8_encode_sequence (buffer, data);
        re_8_encode_pwr (buffer, data);
        re_8_encode_reserved (buffer);
        re_8_encode_crc (buffer);
        memcpy (cleartext, buffer + RE_8_OFFSET_CIPHER, RE_8_CIPHERTEXT_LENGTH);
        cipher_status = cipher (cleartext, buffer + RE_8_OFFSET_CIPHER,
                                RE_8_CIPHERTEXT_LENGTH, key, key_size);
        re_8_encode_set_address (buffer, data);

        if (0 != cipher_status)
        {
            result |= RE_ERROR_ENCODING;
        }
    }

    return result;
}