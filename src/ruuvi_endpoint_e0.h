/**
 * Ruuvi Endpoint 0xE0 helper.
 * Defines necessary data for creating a Ruuvi data format 0xE0 broadcast.
 * @note Deprecated, replaced by Ruuvi Endpoint E1.
 *
 * License: BSD-3
 * Author: TheSomeMan
 */

#ifndef RUUVI_ENDPOINT_E0_H
#define RUUVI_ENDPOINT_E0_H

#include "ruuvi_endpoints.h"
#include <stdbool.h>

#define RE_E0_DESTINATION         (0xE0U)

#define RE_E0_INVALID_TEMPERATURE (0x8000U)
#define RE_E0_INVALID_HUMIDITY    (0xFFFFU)
#define RE_E0_INVALID_PRESSURE    (0xFFFFU)
#define RE_E0_INVALID_PM          (0xFFFFU)
#define RE_E0_INVALID_CO2         (0xFFFFU)
#define RE_E0_INVALID_VOC_INDEX   (511)
#define RE_E0_INVALID_NOX_INDEX   (511)
#define RE_E0_INVALID_LUMINOSITY  (0xFFFFU)
#define RE_E0_INVALID_SOUND_DBA   (0xFFU)
#define RE_E0_INVALID_VOLTAGE     (0xFFU)
#define RE_E0_INVALID_SEQUENCE    (0xFFFFU)
#define RE_E0_INVALID_MAC         (0xFFFFFFFFFFFFU)

#define RE_E0_FLAGS_USB_ON                  (0x01U)
#define RE_E0_FLAGS_LOW_BATTERY             (0x02U)
#define RE_E0_FLAGS_CALIBRATION_IN_PROGRESS (0x04U)
#define RE_E0_FLAGS_BOOST_MODE              (0x08U)

#define RE_E0_DATA_LENGTH (40U)

#define RE_E0_TEMPERATURE_MIN   (-126.0f)
#define RE_E0_TEMPERATURE_MAX   (126.0f)
#define RE_E0_TEMPERATURE_RATIO (200.0f)

#define RE_E0_HUMIDITY_MIN   (0.0f)
#define RE_E0_HUMIDITY_MAX   (100.0f)
#define RE_E0_HUMIDITY_RATIO (400.0f)

#define RE_E0_PRESSURE_MIN (50000.0f)
#define RE_E0_PRESSURE_MAX (115534.0f)

#define RE_E0_PM_MIN   (0.0f)
#define RE_E0_PM_MAX   (1000.0f)
#define RE_E0_PM_RATIO (10.0f)

#define RE_E0_CO2_MIN   (0.0f)
#define RE_E0_CO2_MAX   (40000.0f)
#define RE_E0_CO2_RATIO (1.0f)

#define RE_E0_VOC_INDEX_MIN (1.0f)
#define RE_E0_VOC_INDEX_MAX (500.0f)

#define RE_E0_NOX_INDEX_MIN (1.0f)
#define RE_E0_NOX_INDEX_MAX (500.0f)

#define RE_E0_LUMINOSITY_MIN (0.0f)
#define RE_E0_LUMINOSITY_MAX (65534.0f)

#define RE_E0_SOUND_DBA_MIN   (0.0f)
#define RE_E0_SOUND_DBA_MAX   (127.0f)
#define RE_E0_SOUND_DBA_RATIO (2.0f)

#define RE_E0_VOLTAGE_MIN   (0.0f)
#define RE_E0_VOLTAGE_MAX   (7.62f)
#define RE_E0_VOLTAGE_RATIO (100.0f / 3.0f)

#define RE_E0_OFFSET_PAYLOAD          (4U)

#define RE_E0_OFFSET_HEADER (0U)

#define RE_E0_OFFSET_TEMPERATURE_MSB (1U)
#define RE_E0_OFFSET_TEMPERATURE_LSB (2U)

#define RE_E0_OFFSET_HUMIDITY_MSB (3U)
#define RE_E0_OFFSET_HUMIDITY_LSB (4U)

#define RE_E0_OFFSET_PRESSURE_MSB (5U)
#define RE_E0_OFFSET_PRESSURE_LSB (6U)

#define RE_E0_OFFSET_PM_1_0_MSB (7U)
#define RE_E0_OFFSET_PM_1_0_LSB (8U)

#define RE_E0_OFFSET_PM_2_5_MSB (9U)
#define RE_E0_OFFSET_PM_2_5_LSB (10U)

#define RE_E0_OFFSET_PM_4_0_MSB (11U)
#define RE_E0_OFFSET_PM_4_0_LSB (12U)

#define RE_E0_OFFSET_PM_10_0_MSB (13U)
#define RE_E0_OFFSET_PM_10_0_LSB (14U)

#define RE_E0_OFFSET_CO2_MSB (15U)
#define RE_E0_OFFSET_CO2_LSB (16U)

#define RE_E0_OFFSET_VOC_INDEX_MSB (17U)
#define RE_E0_OFFSET_VOC_INDEX_LSB (18U)

#define RE_E0_OFFSET_NOX_INDEX_MSB (19U)
#define RE_E0_OFFSET_NOX_INDEX_LSB (20U)

#define RE_E0_OFFSET_LUMINOSITY_MSB (21U)
#define RE_E0_OFFSET_LUMINOSITY_LSB (22U)

#define RE_E0_OFFSET_SOUND_DBA_AVG  (23U)
#define RE_E0_OFFSET_SOUND_DBA_PEAK (24U)

#define RE_E0_OFFSET_SEQ_CTR_MSB (25U)
#define RE_E0_OFFSET_SEQ_CTR_LSB (26U)

#define RE_E0_OFFSET_VOLTAGE (27U)

#define RE_E0_OFFSET_FLAGS (28U)

#define RE_E0_OFFSET_RESERVED (29U)

#define RE_E0_OFFSET_ADDR_MSB (34U)

/** @brief All data required for Ruuvi dataformat 0xE0 package. */
typedef struct
{
    re_float temperature_c;     //!< Temperature in degrees Celsius.
    re_float humidity_rh;       //!< Humidity in relative humidity percentage.
    re_float pressure_pa;       //!< Humidity in relative humidity percentage.
    re_float pm1p0_ppm;         //!< Particulate matter mass concentration PM1.0 in micrograms/m3.
    re_float pm2p5_ppm;         //!< Particulate matter mass concentration PM2.5 in micrograms/m3.
    re_float pm4p0_ppm;         //!< Particulate matter mass concentration PM4.0 in micrograms/m3.
    re_float pm10p0_ppm;        //!< Particulate matter mass concentration PM10.0 in micrograms/m3.
    re_float co2;               //!< CO2 concentration in ppm
    re_float voc_index;         //!< VOC index points.
    re_float nox_index;         //!< NOx index points.
    re_float luminosity;        //!< Luminosity.
    re_float sound_dba_avg;     //!< Sound dBA avg.
    re_float sound_dba_peak;    //!< Sound dBA peak.
    uint16_t measurement_count; //!< Running counter of measurement.
    re_float voltage;           //!< Voltage in volts.

    bool  flag_usb_on : 1;                  //!< Flag: USB ON
    bool  flag_low_battery : 1;             //!< Flag: LOW BATTERY
    bool  flag_calibration_in_progress : 1; //!< Flag: Calibration in progress
    bool  flag_boost_mode : 1;              //!< Flag: Boost mode

    uint64_t address; //!< BLE address of device, most significant byte first.
} re_e0_data_t;

/**
 * @brief Encode given data to given buffer in Ruuvi DFxE0.
 *
 * NAN can be used as a placeholder for invalid / not available values.
 *
 * @param[out] buffer uint8_t array with length of 24 bytes.
 * @param[in] data Struct containing all necessary information
 *            for encoding the data into buffer.
 * @retval RE_SUCCESS if data was encoded successfully.
 */
re_status_t
re_e0_encode (uint8_t * const buffer, const re_e0_data_t * data);

/**
 * @brief Checks if the provided buffer conforms to the Ruuvi DFxE0 format.
 *
 * This function examines the input buffer to determine if its content
 * represents data in the Ruuvi DFxE0 format.
 *
 * @param[in] p_buffer Pointer to a uint8_t input array with a length of 31 bytes to be checked.
 * @return Returns 'true' if the buffer format is Ruuvi DFxE0, 'false' otherwise.
 */
bool
re_e0_check_format (const uint8_t * const p_buffer);

/**
 * @brief Decodes a given buffer using the Ruuvi DFxE0 format.
 *
 * Ruuvi DFxE0 is a data format used by the Ruuvi AirQ environmental sensor.
 *
 * @param[in] p_buffer Pointer to a uint8_t input array with a length of 31 bytes
 *  representing a Bluetooth frame with Ruuvi DFxE0 formatted payload.
 * @param[out] p_data Pointer to a re_e0_data_t struct.
 *  After the function executes, this struct will contain the data decoded from the input buffer.
 * @return Returns RE_SUCCESS if the data was decoded successfully,
 *  i.e., if the input buffer was a valid Ruuvi DFxE0 buffer and `p_data` now points to a valid `re_e0_data_t` object.
 *  If the decoding fails, the function returns a code indicating the type of error occurred.
 */
re_status_t
re_e0_decode (const uint8_t * const p_buffer, re_e0_data_t * const p_data);

/**
 * @brief Create invalid Ruuvi DFxE0 data.
 * @param measurement_cnt Running counter of measurement.
 * @param radio_mac BLE address of device.
 * @return /ref re_e0_data_t with all values set to NAN.
 */
re_e0_data_t
re_e0_data_invalid (const uint16_t measurement_cnt, const uint64_t radio_mac);

#endif
