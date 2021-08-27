#include "ruuvi_endpoint_8.h"


static void re_8_encode_set_address (uint8_t * const buffer,
                                      const re_fa_data_t * data)
{
    uint64_t mac = data->address;

    if ( (RE_8_ENCODE_MAC_MAX < data->address) ||
            (RE_8_ENCODE_MAC_MIN > data->address))
    {
        mac = RE_8_INVALID_MAC;
    }

    for (int8_t offset = 5; offset >= 0; offset--)
    {
        buffer[RE_8_OFFSET_ADDRESS_LSB - offset] = (mac >> (offset * 8)) & (0xFFU);
    }
}

static void re_8_encode_humidity (uint8_t * const buffer, const re_8_data_t * data)
{
    uint16_t coded_humidity = RE_8_INVALID_HUMIDITY;
    float humidity = data->humidity_rh;

    if (!isnan (humidity))
    {
        clip (&humidity, RE_8_HUMI_MIN, RE_8_HUMI_MAX);
        coded_humidity = (uint16_t) roundf (humidity * RE_8_HUMI_RATIO);
    }

    buffer[RE_8_OFFSET_HUMI_MSB] = coded_humidity >> RE_8_BYTE_1_SHIFT;
    buffer[RE_8_OFFSET_HUMI_LSB] = coded_humidity & RE_8_BYTE_MASK;
}

static void re_8_encode_temperature (uint8_t * const buffer, const re_8_data_t * data)
{
    uint16_t coded_temperature = RE_8_INVALID_TEMPERATURE;
    float temperature = data->temperature_c;

    if (!isnan (temperature))
    {
        clip (&temperature, RE_8_TEMP_MIN, RE_8_TEMP_MAX);
        coded_temperature = (uint16_t) roundf (temperature * RE_8_TEMP_RATIO);
    }

    buffer[RE_8_OFFSET_TEMP_MSB] = coded_temperature >> RE_8_BYTE_1_SHIFT;
    buffer[RE_8_OFFSET_TEMP_LSB] = coded_temperature & RE_8_BYTE_MASK;
}

static void re_8_encode_pressure (uint8_t * const buffer, const re_8_data_t * data)
{
    uint16_t coded_pressure = RE_8_INVALID_PRESSURE;
    float pressure = data->pressure_pa;

    if (!isnan (pressure))
    {
        clip (&pressure, RE_8_PRES_MIN, RE_8_PRES_MAX);
        pressure += RE_8_PRES_OFFSET;
        coded_pressure = (uint16_t) roundf (pressure * RE_8_PRES_RATIO);
    }

    buffer[RE_8_OFFSET_PRES_MSB] = coded_pressure >> RE_8_BYTE_1_SHIFT;
    buffer[RE_8_OFFSET_PRES_LSB] = coded_pressure & RE_8_BYTE_MASK;
}

static void re_8_encode_pwr (uint8_t * const buffer, const re_8_data_t * data)
{
    uint16_t coded_voltage = RE_8_INVALID_VOLTAGE;
    float voltage = data->battery_v;
    uint16_t coded_tx_power = RE_8_INVALID_POWER;
    float tx_power = (float) data->tx_power;

    if (!isnan (voltage))
    {
        clip (&voltage, RE_8_VOLTAGE_MIN, RE_8_VOLTAGE_MAX);
        coded_voltage = (uint16_t) roundf ( (voltage * RE_8_BATT_RATIO)
                                            - RE_8_BATT_OFFSET);
    }

    // Check against original int value
    if (RE_8_INVALID_POWER != data->tx_power)
    {
        clip (&tx_power, RE_8_TXPWR_MIN, RE_8_TXPWR_MAX);
        coded_tx_power = (uint16_t) roundf ( (tx_power
                                              + RE_8_TXPWR_OFFSET)
                                             / RE_8_TXPWR_RATIO);
    }

    uint16_t power_info = ( (uint16_t) (coded_voltage << RE_8_BYTE_VOLTAGE_OFFSET))
                          + coded_tx_power;
    buffer[RE_8_OFFSET_POWER_MSB] = (power_info >> RE_8_BYTE_1_SHIFT);
    buffer[RE_8_OFFSET_POWER_LSB] = (power_info & RE_8_BYTE_MASK);
}

static void re_8_encode_movement (uint8_t * const buffer, const re_8_data_t * data)
{
    uint16_t movement_count = RE_8_INVALID_MOVEMENT;

    if (RE_8_MVTCTR_MAX >= data->movement_count)
    {
        movement_count = data->movement_count;
    }

    buffer[RE_8_OFFSET_MVTCTR_MSB] = (movement_count >> RE_8_BYTE_1_SHIFT);
    buffer[RE_8_OFFSET_MVTCTR_LSB] = (movement_count & RE_8_BYTE_MASK);
}

static void re_8_encode_sequence (uint8_t * const buffer, const re_8_data_t * data)
{
    uint16_t measurement_seq = RE_8_INVALID_SEQUENCE;

    if (RE_8_SEQCTR_MAX >= data->measurement_count)
    {
        measurement_seq = data->measurement_count;
    }

    buffer[RE_8_OFFSET_SEQCTR_MSB] = (measurement_seq >> RE_8_BYTE_1_SHIFT);
    buffer[RE_8_OFFSET_SEQCTR_LSB] = (measurement_seq & RE_8_BYTE_MASK);
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
                          const re_fa_data_t * const data,
                          re_fa_encrypt_fp cipher,
                          const uint8_t * const key,
                          const size_t key_size)
{
  
}