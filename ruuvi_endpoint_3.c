#include "ruuvi_endpoint_3.h"
#include "ruuvi_endpoints.h"
#include <stddef.h>
#include <stdbool.h>
#include <math.h>

static void ruuvi_endpoint_3_encode_acceleration(uint8_t* const buffer, const float acceleration, const float invalid)
{
  if(invalid != acceleration)
  {
    int16_t decimal = (int16_t) (acceleration*1000);
    buffer[0] = decimal >> 8;
    buffer[1] = decimal & 0xFF;
  }
  else
  {
    buffer[0] = RUUVI_ENDPOINT_3_INVALID_DATA;
    buffer[1] = RUUVI_ENDPOINT_3_INVALID_DATA;
  }
}

ruuvi_endpoint_status_t ruuvi_endpoint_3_encode(uint8_t* const buffer, const ruuvi_endpoint_3_data_t* data, const float invalid)
{
  if(NULL == buffer  || NULL == data) { return RUUVI_ENDPOINT_ERROR_NULL; }

  buffer[RUUVI_ENDPOINT_3_OFFSET_HEADER] = RUUVI_ENDPOINT_3_DESTINATION;

  // HUMIDITY
  if(invalid != data->humidity_rh)
  {
    //Humidity (one lsb is 0.5%, e.g. 128 is 64%). Round the value
    buffer[RUUVI_ENDPOINT_3_OFFSET_HUMIDITY] = (uint8_t)((data->humidity_rh*2) + 0.5);
  }
  else
  {
    buffer[RUUVI_ENDPOINT_3_OFFSET_HUMIDITY] = RUUVI_ENDPOINT_3_INVALID_DATA;
  }

  // Temperature
  if(invalid != data->temperature_c)
  {
    //Temperature (MSB is sign, next 7 bits are decimal value)
    float temperature = data->temperature_c;
    bool sign = (temperature < 0) ? 1 : 0;
    // abs value
    if(0 > temperature) { temperature = 0 - temperature; }
    // cap the temperature
    if(127 < temperature) {temperature = 127; }

    buffer[RUUVI_ENDPOINT_3_OFFSET_TEMPERATURE_DECIMAL] = (uint8_t)temperature | (sign<<7);
    uint8_t temperature_fraction = (uint8_t)((temperature - floor(temperature)) * 100);
    buffer[RUUVI_ENDPOINT_3_OFFSET_TEMPERATURE_FRACTION] = temperature_fraction;
  }
  else
  {
    buffer[RUUVI_ENDPOINT_3_OFFSET_TEMPERATURE_DECIMAL]  = RUUVI_ENDPOINT_3_INVALID_DATA;
    buffer[RUUVI_ENDPOINT_3_OFFSET_TEMPERATURE_FRACTION] = RUUVI_ENDPOINT_3_INVALID_DATA;
  }

  // Pressure
  if(invalid != data->pressure_pa)
  {
    uint32_t pressure = data->pressure_pa;
    pressure -= 50000;
    buffer[RUUVI_ENDPOINT_3_OFFSET_PRESSURE_MSB] = pressure>>8;
    buffer[RUUVI_ENDPOINT_3_OFFSET_PRESSURE_LSB] = pressure&0xFF;
  }
  else
  {
    buffer[RUUVI_ENDPOINT_3_OFFSET_PRESSURE_MSB] = RUUVI_ENDPOINT_3_INVALID_DATA;
    buffer[RUUVI_ENDPOINT_3_OFFSET_PRESSURE_LSB] = RUUVI_ENDPOINT_3_INVALID_DATA;
  }

  // acceleration
  ruuvi_endpoint_3_encode_acceleration(&buffer[RUUVI_ENDPOINT_3_OFFSET_ACCELERATIONX_MSB], data->accelerationx_g, invalid);
  ruuvi_endpoint_3_encode_acceleration(&buffer[RUUVI_ENDPOINT_3_OFFSET_ACCELERATIONY_MSB], data->accelerationy_g, invalid);
  ruuvi_endpoint_3_encode_acceleration(&buffer[RUUVI_ENDPOINT_3_OFFSET_ACCELERATIONZ_MSB], data->accelerationz_g, invalid);

  // voltage
  if(invalid != data->battery_v)
  {
    uint32_t voltage = data->battery_v*1000;
    buffer[RUUVI_ENDPOINT_3_OFFSET_VOLTAGE_MSB] = voltage>>8;
    buffer[RUUVI_ENDPOINT_3_OFFSET_VOLTAGE_LSB] = voltage&0xFF;
  }
  else
  {
    buffer[RUUVI_ENDPOINT_3_OFFSET_VOLTAGE_MSB] = RUUVI_ENDPOINT_3_INVALID_DATA;
    buffer[RUUVI_ENDPOINT_3_OFFSET_VOLTAGE_LSB] = RUUVI_ENDPOINT_3_INVALID_DATA;
  }

  return RUUVI_ENDPOINT_SUCCESS;
}