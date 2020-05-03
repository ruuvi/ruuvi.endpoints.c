#include "ruuvi_endpoint_5.h"
#include "ruuvi_endpoints.h"
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

static void re_5_encode_acceleration (uint8_t * const buffer,
        const float acceleration)
{
    int16_t decimal = RE_5_INVALID_ACCELERATION;

    if (!isnan (acceleration))
    {
        // convert to mG
        decimal = (int16_t) (acceleration * 1000);
    }

    buffer[0] = (decimal >> 8);
    buffer[1] = decimal & 0xFF;
}

re_status_t re_5_encode (uint8_t * const buffer,
        const re_5_data_t * data)
{
    if (NULL == buffer  || NULL == data) { return RE_ERROR_NULL; }

    buffer[RE_5_OFFSET_HEADER] = RE_5_DESTINATION;
    uint16_t humidity = RE_5_INVALID_HUMIDITY;

    if (!isnan (data->humidity_rh) && 0 < data->humidity_rh)
    {
        // Humidity (16bit unsigned) in 0.0025% (0-163.83% range, though realistically 0-100%)
        humidity = (uint16_t) (data->humidity_rh * 400);
    }

    buffer[RE_5_OFFSET_HUMIDITY_MSB] = (humidity >> 8);
    buffer[RE_5_OFFSET_HUMIDITY_LSB] = humidity & 0xFF;
    // Temperature is in 0.005 degrees
    int16_t temperature = RE_5_INVALID_TEMPERATURE;

    if (!isnan (data->temperature_c))
    {
        //
        temperature = (int16_t) (data->temperature_c * 200);
    }

    buffer[RE_5_OFFSET_TEMPERATURE_MSB] = (temperature >> 8);
    buffer[RE_5_OFFSET_TEMPERATURE_LSB] = (temperature & 0xFF);
    // Pressure
    uint32_t pressure = RE_5_INVALID_PRESSURE;

    if (!isnan (data->pressure_pa) && 50000 < data->pressure_pa)
    {
        pressure = data->pressure_pa;
        pressure -= 50000;
    }

    buffer[RE_5_OFFSET_PRESSURE_MSB] = (pressure >> 8) & 0xFF;
    buffer[RE_5_OFFSET_PRESSURE_LSB] = (pressure  & 0xFF);
    // acceleration
    re_5_encode_acceleration (&buffer[RE_5_OFFSET_ACCELERATIONX_MSB],
                                          data->accelerationx_g);
    re_5_encode_acceleration (&buffer[RE_5_OFFSET_ACCELERATIONY_MSB],
                                          data->accelerationy_g);
    re_5_encode_acceleration (&buffer[RE_5_OFFSET_ACCELERATIONZ_MSB],
                                          data->accelerationz_g);
    // voltage, tx power
    uint16_t voltage = RE_5_INVALID_VOLTAGE;
    int8_t tx_power = RE_5_INVALID_POWER;

    if (!isnan (data->battery_v) && 1.6 < data->battery_v)
    {
        // first 11 bits unsigned is the battery voltage above 1.6V, in millivolts (1.6V to 3.647V range)
        voltage = (data->battery_v * 1000 - 1600);
    }

    if (RE_5_INVALID_POWER != data->tx_power && data->tx_power >= -40
            && data->tx_power <= 24)
    {
        // Last 5 bits unsigned is the TX power above -40dBm, in 2dBm steps. (-40dBm to +24dBm range)
        tx_power = (data->tx_power + 40) / 2;
    }

    uint16_t power_info = (voltage << 5) + tx_power;
    buffer[RE_5_OFFSET_POWER_MSB] = (power_info >> 8);
    buffer[RE_5_OFFSET_POWER_LSB] = (power_info & 0xFF);
    buffer[RE_5_OFFSET_MOVEMENT_COUNTER]     = data->movement_count;
    buffer[RE_5_OFFSET_SEQUENCE_COUNTER_MSB] = (data->measurement_count >> 8);
    buffer[RE_5_OFFSET_SEQUENCE_COUNTER_LSB] = (data->measurement_count & 0xFF);
    // Address is 64 bits, skip 2 first bytes
    buffer[RE_5_OFFSET_ADDRESS_MSB]     = (data->address >> 40) & 0xFF;
    buffer[RE_5_OFFSET_ADDRESS_MSB + 1] = (data->address >> 32) & 0xFF;
    buffer[RE_5_OFFSET_ADDRESS_MSB + 2] = (data->address >> 24) & 0xFF;
    buffer[RE_5_OFFSET_ADDRESS_MSB + 3] = (data->address >> 16) & 0xFF;
    buffer[RE_5_OFFSET_ADDRESS_MSB + 4] = (data->address >> 8) & 0xFF;
    buffer[RE_5_OFFSET_ADDRESS_MSB + 5] = (data->address >> 0) & 0xFF;
    return RE_SUCCESS;
}