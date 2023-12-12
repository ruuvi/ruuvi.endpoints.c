/**
 * Ruuvi Endpoint 5 helper.
 * Defines necessary data for creating a Ruuvi data format 5 broadcast.
 *
 * License: BSD-3
 * Author: Otso Jousimaa <otso@ojousima.net>
 */

#ifndef RUUVI_ENDPOINT_5_H
#define RUUVI_ENDPOINT_5_H
#include "ruuvi_endpoints.h"

#define RE_5_DESTINATION          (0xC5U)
#define RE_5_INVALID_TEMPERATURE  (0x8000U)
#define RE_5_INVALID_HUMIDITY     (0xFFFFU)
#define RE_5_INVALID_PRESSURE     (0xFFFFU)
#define RE_5_INVALID_ACCELERATION (0x8000U)
#define RE_5_INVALID_SEQUENCE     (0xFFFFU)
#define RE_5_INVALID_MOVEMENT     (0xFFU)
#define RE_5_INVALID_VOLTAGE      (0x07FFU)
#define RE_5_INVALID_POWER        (0x1FU)
#define RE_5_INVALID_MAC          (0xFFFFFFFFFFFFU)
#define RE_5_DATA_LENGTH          (18U)

#define RE_5_TEMP_MAX          (163.835f)
#define RE_5_TEMP_MIN          (-163.835f)
#define RE_5_HUMI_MAX          (163.835f)
#define RE_5_HUMI_MIN          (0.0f)
#define RE_5_PRES_MAX          (115534.0f)
#define RE_5_PRES_MIN          (50000.0f)
#define RE_5_ACC_MAX           (32.767f)
#define RE_5_ACC_MIN           (-32.767f)
#define RE_5_VOLTAGE_MAX       (3.646f)
#define RE_5_VOLTAGE_MIN       (1.6f)
#define RE_5_TXPWR_MAX         (20)
#define RE_5_TXPWR_MIN         (-40)
#define RE_5_MVTCTR_MAX        (254)
#define RE_5_MVTCTR_MIN        (0)
#define RE_5_SEQCTR_MAX        (65534)
#define RE_5_SEQCTR_MIN        (0)

#define RE_5_OFFSET_HEADER     (0U)
#define RE_5_OFFSET_TEMP_MSB   (1U)
#define RE_5_OFFSET_TEMP_LSB   (2U)
#define RE_5_OFFSET_HUMI_MSB   (3U)
#define RE_5_OFFSET_HUMI_LSB   (4U)
#define RE_5_OFFSET_PRES_MSB   (5U)
#define RE_5_OFFSET_PRES_LSB   (6U)
/*
#define RE_5_OFFSET_ACCX_MSB   (7U)
#define RE_5_OFFSET_ACCX_LSB   (8U)
#define RE_5_OFFSET_ACCY_MSB   (9U)
#define RE_5_OFFSET_ACCY_LSB   (10U)
#define RE_5_OFFSET_ACCZ_MSB   (11U)
#define RE_5_OFFSET_ACCZ_LSB   (12U)
*/
#define RE_5_OFFSET_POWER_MSB  (7U)
#define RE_5_OFFSET_POWER_LSB  (8U)
#define RE_5_OFFSET_MVTCTR     (9U)
#define RE_5_OFFSET_SEQCTR_MSB (10U)
#define RE_5_OFFSET_SEQCTR_LSB (11U)
#define RE_5_OFFSET_ADDR_MSB   (12U)

/** @brief All data required for Ruuvi dataformat 5 package. */
typedef struct
{
    re_float humidity_rh;
    //!< Humidity in relative humidity percentage.
    re_float pressure_pa;
    //!< Pressure in pascals.
    re_float temperature_c;
    //!< Temperature in celcius.
    re_float accelerationx_g;
    //!< Acceleration along X-axis, can be RMS over some period.
    re_float accelerationy_g;
    //!< Acceleration along Y-axis, can be RMS over some period.
    re_float accelerationz_g;
    //!< Acceleration along Z-axis, can be RMS over some period.
    re_float battery_v;
    //!< Battery voltage, preferably under load such as radio TX.
    uint16_t measurement_count;
    //!< Running counter of measurement.
    uint8_t movement_count;
    //!< Number of detected movements.
    uint64_t address;
    //!< BLE address of device, most significant byte first.
    int8_t tx_power;
    //!< Transmission power of radio, in dBm.
} re_5_data_t;

/**
 * @brief Encode given data to given buffer in Ruuvi DF5.
 *
 * NAN can be used as a placeholder for invalid / not available values.
 *
 * @param[in] buffer uint8_t array with length of 24 bytes.
 * @param[in] data Struct containing all necessary information
 *            for encoding the data into buffer.
 * @retval RE_SUCCESS if data was encoded successfully.
 */
re_status_t re_5_encode (uint8_t * const buffer, const re_5_data_t * data);


#endif
