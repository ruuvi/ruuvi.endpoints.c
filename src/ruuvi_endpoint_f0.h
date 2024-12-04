/**
 * Ruuvi Endpoint 0xF0 helper.
 * Defines necessary data for creating a Ruuvi data format 0xF0 broadcast.
 *
 * License: BSD-3
 * Author: TheSomeMan
 */

#ifndef RUUVI_ENDPOINT_F0_H
#define RUUVI_ENDPOINT_F0_H

#include "ruuvi_endpoints.h"
#include <stdbool.h>

#define RE_F0_DESTINATION         (0xF0U)

#define RE_F0_INVALID_TEMPERATURE (0x80U)
#define RE_F0_INVALID_HUMIDITY    (0xFFU)
#define RE_F0_INVALID_PRESSURE    (0xFFU)
#define RE_F0_INVALID_PM          (0xFFU)
#define RE_F0_INVALID_CO2         (0xFFU)
#define RE_F0_INVALID_VOC_INDEX   (0xFFU)
#define RE_F0_INVALID_NOX_INDEX   (0xFFU)
#define RE_F0_INVALID_LUMINOSITY  (0xFFU)
#define RE_F0_INVALID_SOUND       (0xFFU)
#define RE_F0_INVALID_MAC         (0xFFFFFFFFFFFFU)

#define RE_F0_FLAGS_SEQ_MASK                (0x0FU)
#define RE_F0_FLAGS_SEQ_OFFSET              (4U)
#define RE_F0_FLAGS_USB_ON                  (0x01U)
#define RE_F0_FLAGS_LOW_BATTERY             (0x02U)
#define RE_F0_FLAGS_CALIBRATION_IN_PROGRESS (0x04U)
#define RE_F0_FLAGS_BOOST_MODE              (0x08U)

#define RE_F0_DATA_LENGTH (20U)

#define RE_F0_TEMPERATURE_MIN          (-126.0f)
#define RE_F0_TEMPERATURE_MAX          (126.0f)
#define RE_F0_TEMPERATURE_SCALE_FACTOR (1)

#define RE_F0_HUMIDITY_MIN          (0.0f)
#define RE_F0_HUMIDITY_MAX          (100.0f)
#define RE_F0_HUMIDITY_SCALE_FACTOR (2)

#define RE_F0_PRESSURE_MIN          (90000.0f)
#define RE_F0_PRESSURE_MAX          (115400.0f)
#define RE_F0_PRESSURE_SCALE_FACTOR (100)

#define RE_F0_PM_MIN          (0.0f)
#define RE_F0_PM_MAX          (1000.0f)
#define RE_F0_PM_SCALE_FACTOR (254.0f / logf(RE_F0_PM_MAX + 1))

#define RE_F0_CO2_MIN          (0.0f)
#define RE_F0_CO2_MAX          (40000.0f)
#define RE_F0_CO2_SCALE_FACTOR (254.0f / logf(RE_F0_CO2_MAX + 1))

#define RE_F0_VOC_INDEX_MIN    (1.0f)
#define RE_F0_VOC_INDEX_MAX    (500.0f)
#define RE_F0_VOC_SCALE_FACTOR (254.0f / logf(RE_F0_VOC_INDEX_MAX))

#define RE_F0_NOX_INDEX_MIN    (1.0f)
#define RE_F0_NOX_INDEX_MAX    (500.0f)
#define RE_F0_NOX_SCALE_FACTOR (254.0f / logf(RE_F0_NOX_INDEX_MAX))

#define RE_F0_LUMINOSITY_MIN          (0.0f)
#define RE_F0_LUMINOSITY_MAX          (40000.0f)
#define RE_F0_LUMINOSITY_SCALE_FACTOR (254.0f / logf(RE_F0_LUMINOSITY_MAX + 1))

#define RE_F0_SOUND_MIN          (0.0f)
#define RE_F0_SOUND_MAX          (127.0f)
#define RE_F0_SOUND_SCALE_FACTOR (2.0f)

#define RE_F0_OFFSET_PAYLOAD (11U)

#define RE_F0_OFFSET_HEADER      (0U)
#define RE_F0_OFFSET_TEMPERATURE (1U)
#define RE_F0_OFFSET_HUMIDITY    (2U)
#define RE_F0_OFFSET_PRESSURE    (3U)
#define RE_F0_OFFSET_PM_1_0      (4U)
#define RE_F0_OFFSET_PM_2_5      (5U)
#define RE_F0_OFFSET_PM_4_0      (6U)
#define RE_F0_OFFSET_PM_10_0     (7U)
#define RE_F0_OFFSET_CO2         (8U)
#define RE_F0_OFFSET_VOC_INDEX   (9U)
#define RE_F0_OFFSET_NOX_INDEX   (10U)
#define RE_F0_OFFSET_LUMINOSITY  (11U)
#define RE_F0_OFFSET_SOUND       (12U)
#define RE_F0_OFFSET_FLAGS       (13U)
#define RE_F0_OFFSET_ADDR_MSB    (14U)

/** @brief All data required for Ruuvi dataformat 0xF0 package. */
typedef struct
{
    re_float temperature_c; //!< Temperature in degrees Celsius.
    re_float humidity_rh;   //!< Humidity in relative humidity percentage.
    re_float pressure_pa;   //!< Pressure in Pa.
    re_float pm1p0_ppm;     //!< Particulate matter mass concentration PM1.0 in micrograms/m3.
    re_float pm2p5_ppm;     //!< Particulate matter mass concentration PM2.5 in micrograms/m3.
    re_float pm4p0_ppm;     //!< Particulate matter mass concentration PM4.0 in micrograms/m3.
    re_float pm10p0_ppm;    //!< Particulate matter mass concentration PM10.0 in micrograms/m3.
    re_float co2;           //!< CO2 concentration in ppm
    re_float voc_index;     //!< VOC index points.
    re_float nox_index;     //!< NOx index points.
    re_float luminosity;    //!< Luminosity.
    re_float sound_dba_avg; //!< Sound dBA avg.

    bool  flag_usb_on : 1;                  //!< Flag: USB ON
    bool  flag_low_battery : 1;             //!< Flag: LOW BATTERY
    bool  flag_calibration_in_progress : 1; //!< Flag: Calibration in progress
    bool  flag_boost_mode : 1;              //!< Flag: Boost mode
    uint8_t  flag_seq_cnt : 4;              //!< Flag: Sequence counter

    uint64_t address; //!< BLE address of device, most significant byte first.
} re_f0_data_t;

/**
 * @brief Encode given data to given buffer in Ruuvi DFxF0.
 *
 * NAN can be used as a placeholder for invalid / not available values.
 *
 * @param[out] buffer uint8_t array with length of 24 bytes.
 * @param[in] data Struct containing all necessary information
 *            for encoding the data into buffer.
 * @retval RE_SUCCESS if data was encoded successfully.
 */
re_status_t
re_f0_encode (uint8_t * const buffer, const re_f0_data_t * data);

/**
 * @brief Checks if the provided buffer conforms to the Ruuvi DFxFE format.
 *
 * This function examines the input buffer to determine if its content
 * represents data in the Ruuvi DFxFE format.
 *
 * @param[in] p_buffer Pointer to a uint8_t input array with a length of 31 bytes to be checked.
 * @return Returns 'true' if the buffer format is Ruuvi DFxFE, 'false' otherwise.
 */
bool
re_f0_check_format (const uint8_t * const p_buffer);

/**
 * @brief Decodes a given buffer using the Ruuvi DFxFE format.
 *
 * Ruuvi DFxFE is a data format used by the Ruuvi AirQ environmental sensor.
 *
 * @param[in] p_buffer Pointer to a uint8_t input array with a length of 31 bytes
 *  representing a Bluetooth frame with Ruuvi DFxFE formatted payload.
 * @param[out] p_data Pointer to a re_f0_data_t struct.
 *  After the function executes, this struct will contain the data decoded from the input buffer.
 * @return Returns RE_SUCCESS if the data was decoded successfully,
 *  i.e., if the input buffer was a valid Ruuvi DFxFE buffer and `p_data` now points to a valid `re_f0_data_t` object.
 *  If the decoding fails, the function returns a code indicating the type of error occurred.
 */
re_status_t
re_f0_decode (const uint8_t * const p_buffer, re_f0_data_t * const p_data);

/**
 * @brief Create invalid Ruuvi DFxFE data.
 * @param measurement_cnt Running counter of measurement.
 * @param radio_mac BLE address of device.
 * @return /ref re_f0_data_t with all values set to NAN.
 */
re_f0_data_t
re_f0_data_invalid (const uint8_t measurement_cnt, const uint64_t radio_mac);

#endif
