#include "ruuvi_endpoint_c5.h"
#include "ruuvi_endpoints.h"
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#if RE_C5_ENABLED

#define RE_C5_ACC_RATIO           (1000.0f)
#define RE_C5_HUMI_RATIO          (400.0f)
#define RE_C5_TEMP_RATIO          (200.0f)
#define RE_C5_PRES_RATIO          (1.0f)
#define RE_C5_PRES_OFFSET         (-50000.0f)
#define RE_C5_BATT_RATIO          (1000.0f)
#define RE_C5_BATT_OFFSET         (1600)
#define RE_C5_BATT_MIN            (1.6f)

#define RE_C5_TXPWR_RATIO         (2)
#define RE_C5_TXPWR_OFFSET        (40)

#define RE_C5_MAC_MAX             (281474976710655)
#define RE_C5_MAC_MIN             (0)

#define RE_C5_BYTE_0_SHIFT        (0U)
#define RE_C5_BYTE_1_SHIFT        (8U)
#define RE_C5_BYTE_2_SHIFT        (16U)
#define RE_C5_BYTE_3_SHIFT        (24U)
#define RE_C5_BYTE_4_SHIFT        (32U)
#define RE_C5_BYTE_5_SHIFT        (40U)
#define RE_C5_BYTE_MASK           (0xFFU)
#define RE_C5_BYTE_VOLTAGE_OFFSET (5U)
#define RE_C5_BYTE_VOLTAGE_MASK   (0x7FFU)
#define RE_C5_BYTE_TX_POWER_OFFSET (0U)
#define RE_C5_BYTE_TX_POWER_MASK   (0x1FU)

// Avoid mocking simple function
#ifdef TEST
void re_clip (re_float * const value, const re_float min, const re_float max)
{
    if (*value > max)
    {
        *value = max;
    }

    if (*value < min)
    {
        *value = min;
    }
}
#endif

static void re_c5_encode_set_address (uint8_t * const buffer, const re_c5_data_t * data)
{
    // Address is 64 bits, skip 2 first bytes
    uint8_t addr_offset = RE_C5_OFFSET_ADDR_MSB;
    uint64_t mac = data->address;

    if (RE_C5_MAC_MAX < data->address)
    {
        mac = RE_C5_INVALID_MAC;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"

    if (RE_C5_MAC_MIN > data->address) // cppcheck-suppress unsignedLessThanZero
    {
        mac = RE_C5_INVALID_MAC;
    }

#pragma GCC diagnostic pop
    buffer[addr_offset] = (mac >> RE_C5_BYTE_5_SHIFT) & RE_C5_BYTE_MASK;
    addr_offset++;
    buffer[addr_offset] = (mac >> RE_C5_BYTE_4_SHIFT) & RE_C5_BYTE_MASK;
    addr_offset++;
    buffer[addr_offset] = (mac >> RE_C5_BYTE_3_SHIFT) & RE_C5_BYTE_MASK;
    addr_offset++;
    buffer[addr_offset] = (mac >> RE_C5_BYTE_2_SHIFT) & RE_C5_BYTE_MASK;
    addr_offset++;
    buffer[addr_offset] = (mac >> RE_C5_BYTE_1_SHIFT) & RE_C5_BYTE_MASK;
    addr_offset++;
    buffer[addr_offset] = (mac >> 0) & RE_C5_BYTE_MASK;
}

static void re_c5_encode_humidity (uint8_t * const buffer, const re_c5_data_t * data)
{
    uint16_t coded_humidity = RE_C5_INVALID_HUMIDITY;
    re_float humidity = data->humidity_rh;

    if (!isnan (humidity))
    {
        re_clip (&humidity, RE_C5_HUMI_MIN, RE_C5_HUMI_MAX);
        coded_humidity = (uint16_t) roundf (humidity * RE_C5_HUMI_RATIO);
    }

    buffer[RE_C5_OFFSET_HUMI_MSB] = coded_humidity >> RE_C5_BYTE_1_SHIFT;
    buffer[RE_C5_OFFSET_HUMI_LSB] = coded_humidity & RE_C5_BYTE_MASK;
}

static void re_c5_encode_temperature (uint8_t * const buffer, const re_c5_data_t * data)
{
    uint16_t coded_temperature = RE_C5_INVALID_TEMPERATURE;
    re_float temperature = data->temperature_c;

    if (!isnan (temperature))
    {
        re_clip (&temperature, RE_C5_TEMP_MIN, RE_C5_TEMP_MAX);
        int16_t rounded_temperature = (int16_t) roundf (temperature * RE_C5_TEMP_RATIO);
        // Type cast adds 2^16 to a negative signed value, not changing bits.
        coded_temperature = (uint16_t) rounded_temperature;
    }

    buffer[RE_C5_OFFSET_TEMP_MSB] = coded_temperature >> RE_C5_BYTE_1_SHIFT;
    buffer[RE_C5_OFFSET_TEMP_LSB] = coded_temperature & RE_C5_BYTE_MASK;
}

static void re_c5_encode_pressure (uint8_t * const buffer, const re_c5_data_t * data)
{
    uint16_t coded_pressure = RE_C5_INVALID_PRESSURE;
    re_float pressure = data->pressure_pa;

    if (!isnan (pressure))
    {
        re_clip (&pressure, RE_C5_PRES_MIN, RE_C5_PRES_MAX);
        pressure += RE_C5_PRES_OFFSET;
        coded_pressure = (uint16_t) roundf (pressure * RE_C5_PRES_RATIO);
    }

    buffer[RE_C5_OFFSET_PRES_MSB] = coded_pressure >> RE_C5_BYTE_1_SHIFT;
    buffer[RE_C5_OFFSET_PRES_LSB] = coded_pressure & RE_C5_BYTE_MASK;
}


static void re_c5_encode_pwr (uint8_t * const buffer, const re_c5_data_t * data)
{
    uint16_t coded_voltage = RE_C5_INVALID_VOLTAGE;
    re_float voltage = data->battery_v;
    uint16_t coded_tx_power = RE_C5_INVALID_POWER;
    re_float tx_power = (re_float) data->tx_power;

    if (!isnan (voltage))
    {
        re_clip (&voltage, RE_C5_VOLTAGE_MIN, RE_C5_VOLTAGE_MAX);
        coded_voltage = (uint16_t) roundf ( (voltage * RE_C5_BATT_RATIO)
                                            - RE_C5_BATT_OFFSET);
    }

    // Check against original int value
    if (RE_C5_INVALID_POWER != data->tx_power)
    {
        re_clip (&tx_power, RE_C5_TXPWR_MIN, RE_C5_TXPWR_MAX);
        coded_tx_power = (uint16_t) roundf ( (tx_power
                                              + RE_C5_TXPWR_OFFSET)
                                             / RE_C5_TXPWR_RATIO);
    }

    uint16_t power_info = ( (uint16_t) (coded_voltage << RE_C5_BYTE_VOLTAGE_OFFSET))
                          + coded_tx_power;
    buffer[RE_C5_OFFSET_POWER_MSB] = (power_info >> RE_C5_BYTE_1_SHIFT);
    buffer[RE_C5_OFFSET_POWER_LSB] = (power_info & RE_C5_BYTE_MASK);
}

static void re_c5_encode_movement (uint8_t * const buffer, const re_c5_data_t * data)
{
    uint8_t movement_count = RE_C5_INVALID_MOVEMENT;

    if (RE_C5_MVTCTR_MAX >= data->movement_count)
    {
        movement_count = data->movement_count;
    }

    buffer[RE_C5_OFFSET_MVTCTR] = movement_count;
}

static void re_c5_encode_sequence (uint8_t * const buffer, const re_c5_data_t * data)
{
    uint16_t measurement_seq = RE_C5_INVALID_SEQUENCE;

    if (RE_C5_SEQCTR_MAX >= data->measurement_count)
    {
        measurement_seq = data->measurement_count;
    }

    buffer[RE_C5_OFFSET_SEQCTR_MSB] = (measurement_seq >> RE_C5_BYTE_1_SHIFT);
    buffer[RE_C5_OFFSET_SEQCTR_LSB] = (measurement_seq & RE_C5_BYTE_MASK);
}

re_status_t re_c5_encode (uint8_t * const buffer, const re_c5_data_t * data)
{
    re_status_t result = RE_SUCCESS;

    if ( (NULL == buffer) || (NULL == data))
    {
        result |= RE_ERROR_NULL;
    }
    else
    {
        buffer[RE_C5_OFFSET_HEADER] = RE_C5_DESTINATION;
        re_c5_encode_humidity (buffer, data);
        re_c5_encode_temperature (buffer, data);
        re_c5_encode_pressure (buffer, data);
        re_c5_encode_movement (buffer, data);
        re_c5_encode_sequence (buffer, data);
        re_c5_encode_pwr (buffer, data);
        re_c5_encode_set_address (buffer, data);
    }

    return result;
}

#endif
