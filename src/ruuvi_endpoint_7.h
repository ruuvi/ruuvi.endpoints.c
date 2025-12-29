/**
 * Ruuvi Endpoint 7 helper.
 * Defines necessary data for creating a Ruuvi data format 7 broadcast.
 * Format 7 is designed for motion/presence sensors with environmental data.
 *
 * License: BSD-3-Clause
 * Author: Otso Jousimaa <otso@ojousima.net>
 */

#ifndef RUUVI_ENDPOINT_7_H
#define RUUVI_ENDPOINT_7_H
#include "ruuvi_endpoints.h"
#include <stdbool.h>

#define RE_7_DESTINATION          (0x07U)
#define RE_7_INVALID_SEQUENCE     (0xFFU)
#define RE_7_INVALID_FLAGS        (0xFFU)
#define RE_7_INVALID_TEMPERATURE  (0x8000U)
#define RE_7_INVALID_HUMIDITY     (0xFFFFU)
#define RE_7_INVALID_PRESSURE     (0xFFFFU)
#define RE_7_INVALID_TILT         (0x7FU)
#define RE_7_INVALID_LUMINOSITY   (0xFFFFU)
#define RE_7_INVALID_COLOR_TEMP   (0xFFU)
#define RE_7_INVALID_BATTERY      (0x0FU)
#define RE_7_INVALID_MOTION_INT   (0x0FU)
#define RE_7_INVALID_MOTION_COUNT (0xFFU)
#define RE_7_INVALID_MAC          (0xFFFFFFU)
#define RE_7_DATA_LENGTH          (20U)

/** Temperature: int16, 0.005°C resolution */
#define RE_7_TEMP_MAX (163.835f)
#define RE_7_TEMP_MIN (-163.835f)

/** Humidity: uint16, 0.0025% resolution */
#define RE_7_HUMI_MAX (163.835f)
#define RE_7_HUMI_MIN (0.0f)

/** Pressure: uint16, 1 Pa resolution, offset 50000 Pa */
#define RE_7_PRES_MAX (115534.0f)
#define RE_7_PRES_MIN (50000.0f)

/** Tilt: int8, ±90° range, ~0.71° resolution
 *  Encoding: coded = tilt * (126/90), value 127 reserved for invalid */
#define RE_7_TILT_MAX (90.0f)
#define RE_7_TILT_MIN (-90.0f)

/** Luminosity: uint16, 1 lux resolution */
#define RE_7_LUMI_MAX (65534.0f)
#define RE_7_LUMI_MIN (0.0f)

/** Color temperature: uint8, 26K steps, 1000-7650K range */
#define RE_7_COLOR_TEMP_MAX (7650U)
#define RE_7_COLOR_TEMP_MIN (1000U)

/** Battery: 4-bit, 1.8-3.6V range (~128mV steps)
 *  Encoding: coded = (voltage - 1.8) * (14/1.8), value 15 reserved for invalid */
#define RE_7_VOLTAGE_MAX (3.6f)
#define RE_7_VOLTAGE_MIN (1.8f)

/** Motion intensity: 4-bit, 0-15 scale */
#define RE_7_MOTION_INT_MAX (15U)
#define RE_7_MOTION_INT_MIN (0U)

/** Motion count: 8-bit, 0-254 range */
#define RE_7_MOTION_CNT_MAX (254U)
#define RE_7_MOTION_CNT_MIN (0U)

/** Sequence counter: 8-bit, 0-254 range */
#define RE_7_SEQCTR_MAX (254U)
#define RE_7_SEQCTR_MIN (0U)

/** Payload offset in raw BLE advertisement (after company ID and header) */
#define RE_7_OFFSET_PAYLOAD (7U)

/** Payload offsets within manufacturer data (22 bytes total) */
#define RE_7_OFFSET_HEADER      (0U)  /**< Format header 0x07 */
#define RE_7_OFFSET_SEQ         (1U)  /**< Message counter */
#define RE_7_OFFSET_FLAGS       (2U)  /**< State flags */
#define RE_7_OFFSET_TEMP_MSB    (3U)  /**< Temperature MSB */
#define RE_7_OFFSET_TEMP_LSB    (4U)  /**< Temperature LSB */
#define RE_7_OFFSET_HUMI_MSB    (5U)  /**< Humidity MSB */
#define RE_7_OFFSET_HUMI_LSB    (6U)  /**< Humidity LSB */
#define RE_7_OFFSET_PRES_MSB    (7U)  /**< Pressure MSB */
#define RE_7_OFFSET_PRES_LSB    (8U)  /**< Pressure LSB */
#define RE_7_OFFSET_TILT_X      (9U)  /**< Tilt X (pitch) */
#define RE_7_OFFSET_TILT_Y      (10U) /**< Tilt Y (roll) */
#define RE_7_OFFSET_LUMI_MSB    (11U) /**< Luminosity MSB */
#define RE_7_OFFSET_LUMI_LSB    (12U) /**< Luminosity LSB */
#define RE_7_OFFSET_COLOR_TEMP  (13U) /**< Color temperature */
#define RE_7_OFFSET_BATT_MOTION (14U) /**< Battery (4-bit) + Motion intensity (4-bit) */
#define RE_7_OFFSET_MOTION_CNT  (15U) /**< Motion count */
#define RE_7_OFFSET_CRC         (16U) /**< CRC8 over bytes 0-15 */
#define RE_7_OFFSET_MAC_0       (17U) /**< MAC address byte 0 (MSB of 3 LSB) */
#define RE_7_OFFSET_MAC_1       (18U) /**< MAC address byte 1 */
#define RE_7_OFFSET_MAC_2       (19U) /**< MAC address byte 2 (LSB) */

/** State flags bit positions */
#define RE_7_FLAG_MOTION_NOW (0U) /**< Bit 0: Motion currently detected */
#define RE_7_FLAG_PRESENCE   (1U) /**< Bit 1: Presence/occupancy detected */

/** @brief All data required for Ruuvi dataformat 7 package. */
typedef struct
{
    re_float temperature_c;
    //!< Temperature in celsius.
    re_float humidity_rh;
    //!< Humidity in relative humidity percentage.
    re_float pressure_pa;
    //!< Pressure in pascals.
    re_float acceleration_x_g;
    //!< Acceleration X in G. Used by encode to calculate tilt: tilt_x = asin(acc_x / |acc|).
    re_float acceleration_y_g;
    //!< Acceleration Y in G. Used by encode to calculate tilt: tilt_y = asin(acc_y / |acc|).
    re_float acceleration_z_g;
    //!< Acceleration Z in G. Used for normalization when calculating tilt.
    re_float tilt_x_deg;
    //!< Tilt X (pitch) in degrees, -90 to +90. Output by decode.
    re_float tilt_y_deg;
    //!< Tilt Y (roll) in degrees, -90 to +90. Output by decode.
    re_float luminosity_lux;
    //!< Luminosity in lux.
    uint16_t color_temp_k;
    //!< Color temperature in Kelvin, 1000-7650K.
    re_float battery_v;
    //!< Battery voltage, 1.8-3.6V.
    uint8_t motion_intensity;
    //!< Motion intensity, 0-15 scale.
    uint8_t motion_count;
    //!< Motion event counter, 0-254.
    uint8_t sequence_counter;
    //!< Message sequence counter, 0-254.
    bool motion_detected;
    //!< True if motion currently detected.
    bool presence_detected;
    //!< True if presence/occupancy detected.
    uint64_t address;
    //!< BLE MAC address (only 3 LSB used in payload).
} re_7_data_t;

/**
 * @brief Encode given data to given buffer in Ruuvi DF7.
 *
 * NAN can be used as a placeholder for invalid / not available values.
 * CRC8 is automatically calculated and appended.
 *
 * @param[in] buffer uint8_t array with length of 22 bytes.
 * @param[in] data Struct containing all necessary information
 *            for encoding the data into buffer.
 * @retval RE_SUCCESS if data was encoded successfully.
 * @retval RE_ERROR_NULL if buffer or data is NULL.
 */
re_status_t
re_7_encode (uint8_t * const buffer, const re_7_data_t * data);

/**
 * @brief Checks if the provided buffer conforms to the Ruuvi DF7 format.
 *
 * This function examines the input buffer to determine if its content
 * represents data in the Ruuvi DF7 format.
 *
 * @param[in] p_buffer Pointer to a uint8_t input array with a length of 31 bytes to be checked.
 * @return Returns 'true' if the buffer format is Ruuvi DF7, 'false' otherwise.
 */
bool
re_7_check_format (const uint8_t * const p_buffer);

/**
 * @brief Decodes a given buffer using the Ruuvi DF7 format.
 *
 * Ruuvi DF7 is a data format for motion/presence sensors with environmental data.
 *
 * @param[in] p_buffer Pointer to a uint8_t input array with a length of 31 bytes
 *  representing a Bluetooth frame with Ruuvi DF7 formatted payload.
 * @param[out] p_data Pointer to a re_7_data_t struct.
 *  After the function executes, this struct will contain the data decoded from the input buffer.
 * @return Returns RE_SUCCESS if the data was decoded successfully.
 * @retval RE_ERROR_NULL if buffer or data is NULL.
 * @retval RE_ERROR_INVALID_PARAM if format header is wrong.
 * @retval RE_ERROR_DECODING_CRC if CRC check fails.
 */
re_status_t
re_7_decode (const uint8_t * const p_buffer, re_7_data_t * const p_data);

#endif
