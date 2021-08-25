#include "ruuvi_endpoint_5.h"
#include "ruuvi_endpoints.h"
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define RE_5_ACC_RATIO   (1000.0f)
#define RE_5_HUMI_RATIO  (400.0f)
#define RE_5_TEMP_RATIO  (200.0f)
#define RE_5_PRES_RATIO  (1.0f)
#define RE_5_PRES_OFFSET (-50000.0f)
#define RE_5_BATT_RATIO  (1000.0f)
#define RE_5_BATT_OFFSET (1600)
#define RE_5_BATT_MIN    (1.6f)

#define RE_5_TXPWR_RATIO  (2)
#define RE_5_TXPWR_OFFSET (40)

#define RE_5_MVTCTR_MAX   (254)
#define RE_5_MVTCTR_MIN   (0)

#define RE_5_SEQCTR_MAX   (65534)
#define RE_5_SEQCTR_MIN   (0)

#define RE_5_MAC_MAX      (281474976710655)
#define RE_5_MAC_MIN      (0)

#define RE_5_BYTE_1_SHIFT                  (8U)
#define RE_5_BYTE_2_SHIFT                  (16U)
#define RE_5_BYTE_3_SHIFT                  (24U)
#define RE_5_BYTE_4_SHIFT                  (32U)
#define RE_5_BYTE_5_SHIFT                  (40U)
#define RE_5_BYTE_MASK                     (0xFFU)
#define RE_5_BYTE_VOLTAGE_OFFSET           (5U)

static void clip (float * const value, const float min, const float max)
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

static void re_5_encode_acceleration (uint8_t * const acceleration_slot,
                                      float acceleration)
{
    uint16_t coded_acceleration = RE_5_INVALID_ACCELERATION;

    if (!isnan (acceleration))
    {
        clip (&acceleration, RE_5_ACC_MIN, RE_5_ACC_MAX);
        coded_acceleration = (uint16_t) roundf (acceleration * RE_5_ACC_RATIO);
    }

    acceleration_slot[0] = (coded_acceleration >> RE_5_BYTE_1_SHIFT);
    acceleration_slot[1] = (coded_acceleration & RE_5_BYTE_MASK);
}

static void re_5_encode_set_address (uint8_t * const buffer, const re_5_data_t * data)
{
    // Address is 64 bits, skip 2 first bytes
    uint8_t addr_offset = RE_5_OFFSET_ADDR_MSB;
    uint64_t mac = data->address;

    if ( (RE_5_MAC_MAX < data->address) || (RE_5_MAC_MIN > data->address))
    {
        mac = RE_5_INVALID_MAC;
    }

    buffer[addr_offset] = (mac >> RE_5_BYTE_5_SHIFT) & RE_5_BYTE_MASK;
    addr_offset++;
    buffer[addr_offset] = (mac >> RE_5_BYTE_4_SHIFT) & RE_5_BYTE_MASK;
    addr_offset++;
    buffer[addr_offset] = (mac >> RE_5_BYTE_3_SHIFT) & RE_5_BYTE_MASK;
    addr_offset++;
    buffer[addr_offset] = (mac >> RE_5_BYTE_2_SHIFT) & RE_5_BYTE_MASK;
    addr_offset++;
    buffer[addr_offset] = (mac >> RE_5_BYTE_1_SHIFT) & RE_5_BYTE_MASK;
    addr_offset++;
    buffer[addr_offset] = (mac >> 0) & RE_5_BYTE_MASK;
}

static void re_5_encode_humidity (uint8_t * const buffer, const re_5_data_t * data)
{
    uint16_t coded_humidity = RE_5_INVALID_HUMIDITY;
    float humidity = data->humidity_rh;

    if (!isnan (humidity))
    {
        clip (&humidity, RE_5_HUMI_MIN, RE_5_HUMI_MAX);
        coded_humidity = (uint16_t) roundf (humidity * RE_5_HUMI_RATIO);
    }

    buffer[RE_5_OFFSET_HUMI_MSB] = coded_humidity >> RE_5_BYTE_1_SHIFT;
    buffer[RE_5_OFFSET_HUMI_LSB] = coded_humidity & RE_5_BYTE_MASK;
}

static void re_5_encode_temperature (uint8_t * const buffer, const re_5_data_t * data)
{
    uint16_t coded_temperature = RE_5_INVALID_TEMPERATURE;
    float temperature = data->temperature_c;

    if (!isnan (temperature))
    {
        clip (&temperature, RE_5_TEMP_MIN, RE_5_TEMP_MAX);
        coded_temperature = (uint16_t) roundf (temperature * RE_5_TEMP_RATIO);
    }

    buffer[RE_5_OFFSET_TEMP_MSB] = coded_temperature >> RE_5_BYTE_1_SHIFT;
    buffer[RE_5_OFFSET_TEMP_LSB] = coded_temperature & RE_5_BYTE_MASK;
}

static void re_5_encode_pressure (uint8_t * const buffer, const re_5_data_t * data)
{
    uint16_t coded_pressure = RE_5_INVALID_PRESSURE;
    float pressure = data->pressure_pa;

    if (!isnan (pressure))
    {
        clip (&pressure, RE_5_PRES_MIN, RE_5_PRES_MAX);
        pressure += RE_5_PRES_OFFSET;
        coded_pressure = (uint16_t) roundf (pressure * RE_5_PRES_RATIO);
    }

    buffer[RE_5_OFFSET_PRES_MSB] = coded_pressure >> RE_5_BYTE_1_SHIFT;
    buffer[RE_5_OFFSET_PRES_LSB] = coded_pressure & RE_5_BYTE_MASK;
}

static void re_5_encode_pwr (uint8_t * const buffer, const re_5_data_t * data)
{
    uint16_t coded_voltage = RE_5_INVALID_VOLTAGE;
    float voltage = data->battery_v;
    uint16_t coded_tx_power = RE_5_INVALID_POWER;
    float tx_power = (float) data->tx_power;

    if (!isnan (voltage))
    {
        clip (&voltage, RE_5_VOLTAGE_MIN, RE_5_VOLTAGE_MAX);
        coded_voltage = (uint16_t) roundf ( (voltage * RE_5_BATT_RATIO)
                                            - RE_5_BATT_OFFSET);
    }

    // Check against original int value
    if (RE_5_INVALID_POWER != data->tx_power)
    {
        clip (&tx_power, RE_5_TXPWR_MIN, RE_5_TXPWR_MAX);
        coded_tx_power = (uint16_t) roundf ( (tx_power
                                              + RE_5_TXPWR_OFFSET)
                                             / RE_5_TXPWR_RATIO);
    }

    uint16_t power_info = ((uint16_t) (coded_voltage << RE_5_BYTE_VOLTAGE_OFFSET))
                          + coded_tx_power;
    buffer[RE_5_OFFSET_POWER_MSB] = (power_info >> RE_5_BYTE_1_SHIFT);
    buffer[RE_5_OFFSET_POWER_LSB] = (power_info & RE_5_BYTE_MASK);
}

static void re_5_encode_movement (uint8_t * const buffer, const re_5_data_t * data)
{
    uint8_t movement_count = RE_5_INVALID_MOVEMENT;

    if (RE_5_MVTCTR_MAX >= data->movement_count)
    {
        movement_count = data->movement_count;
    }

    buffer[RE_5_OFFSET_MVTCTR] = movement_count;
}

static void re_5_encode_sequence (uint8_t * const buffer, const re_5_data_t * data)
{
    uint16_t measurement_seq = RE_5_INVALID_SEQUENCE;

    if (RE_5_SEQCTR_MAX >= data->measurement_count)
    {
        measurement_seq = data->measurement_count;
    }

    buffer[RE_5_OFFSET_SEQCTR_MSB] = (measurement_seq >> RE_5_BYTE_1_SHIFT);
    buffer[RE_5_OFFSET_SEQCTR_LSB] = (measurement_seq & RE_5_BYTE_MASK);
}

re_status_t re_5_encode (uint8_t * const buffer, const re_5_data_t * data)
{
    re_status_t result = RE_SUCCESS;

    if ( (NULL == buffer) || (NULL == data))
    {
        result |= RE_ERROR_NULL;
    }
    else
    {
        buffer[RE_5_OFFSET_HEADER] = RE_5_DESTINATION;
        re_5_encode_humidity (buffer, data);
        re_5_encode_temperature (buffer, data);
        re_5_encode_pressure (buffer, data);
        re_5_encode_acceleration (&buffer[RE_5_OFFSET_ACCX_MSB], data->accelerationx_g);
        re_5_encode_acceleration (&buffer[RE_5_OFFSET_ACCY_MSB], data->accelerationy_g);
        re_5_encode_acceleration (&buffer[RE_5_OFFSET_ACCZ_MSB], data->accelerationz_g);
        re_5_encode_movement (buffer, data);
        re_5_encode_sequence (buffer, data);
        re_5_encode_pwr (buffer, data);
        re_5_encode_set_address (buffer, data);
    }

    return result;
}
