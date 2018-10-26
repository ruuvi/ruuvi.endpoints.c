/**
 * Ruuvi Endpoint 5 helper. Defines necessary data for creating a Ruuvi data format 5 broadcast.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 */

#ifndef RUUVI_ENDPOINT_5_H
#define RUUVI_ENDPOINT_5_H
#include "ruuvi_endpoints.h"

#define RUUVI_ENDPOINT_5_DESTINATION          0x05
#define RUUVI_ENDPOINT_5_INVALID_TEMPERATURE  0x8000
#define RUUVI_ENDPOINT_5_INVALID_HUMIDITY     0xFFFF
#define RUUVI_ENDPOINT_5_INVALID_PRESSURE     0xFFFF
#define RUUVI_ENDPOINT_5_INVALID_ACCELERATION 0x8000
#define RUUVI_ENDPOINT_5_INVALID_SEQUENCE     0xFFFF
#define RUUVI_ENDPOINT_5_INVALID_MOVEMENT     0xFF
#define RUUVI_ENDPOINT_5_INVALID_VOLTAGE      0x07FF
#define RUUVI_ENDPOINT_5_INVALID_POWER        0x1F
#define RUUVI_ENDPOINT_5_DATA_LENGTH          24

#define RUUVI_ENDPOINT_5_OFFSET_HEADER               0
#define RUUVI_ENDPOINT_5_OFFSET_TEMPERATURE_MSB      1
#define RUUVI_ENDPOINT_5_OFFSET_TEMPERATURE_LSB      2
#define RUUVI_ENDPOINT_5_OFFSET_HUMIDITY_MSB         3
#define RUUVI_ENDPOINT_5_OFFSET_HUMIDITY_LSB         4
#define RUUVI_ENDPOINT_5_OFFSET_PRESSURE_MSB         5
#define RUUVI_ENDPOINT_5_OFFSET_PRESSURE_LSB         6
#define RUUVI_ENDPOINT_5_OFFSET_ACCELERATIONX_MSB    7
#define RUUVI_ENDPOINT_5_OFFSET_ACCELERATIONX_LSB    8
#define RUUVI_ENDPOINT_5_OFFSET_ACCELERATIONY_MSB    9
#define RUUVI_ENDPOINT_5_OFFSET_ACCELERATIONY_LSB    10
#define RUUVI_ENDPOINT_5_OFFSET_ACCELERATIONZ_MSB    11
#define RUUVI_ENDPOINT_5_OFFSET_ACCELERATIONZ_LSB    12
#define RUUVI_ENDPOINT_5_OFFSET_POWER_MSB            13
#define RUUVI_ENDPOINT_5_OFFSET_POWER_LSB            14
#define RUUVI_ENDPOINT_5_OFFSET_MOVEMENT_COUNTER     15
#define RUUVI_ENDPOINT_5_OFFSET_SEQUENCE_COUNTER_MSB 16
#define RUUVI_ENDPOINT_5_OFFSET_SEQUENCE_COUNTER_LSB 17
#define RUUVI_ENDPOINT_5_OFFSET_ADDRESS_MSB          18


typedef struct{
  float humidity_rh;
  float pressure_pa;
  float temperature_c;
  float accelerationx_g;
  float accelerationy_g;
  float accelerationz_g;
  float battery_v;
  uint16_t measurement_count;
  uint8_t movement_count;
  uint64_t address;
  int8_t tx_power;
}ruuvi_endpoint_5_data_t;

/**
 * Encode given data to given buffer in Ruuvi DF5.
 *
 * parameter data: uint8_t array with length of 24 bytes.
 * parameter data: Struct containing all necessary information for encoding the data into buffer
 * parameter invalid: A float which signals that given data point is invalid.
 */
ruuvi_endpoint_status_t ruuvi_endpoint_5_encode(uint8_t* const buffer, const ruuvi_endpoint_5_data_t* data, const float invalid);


#endif