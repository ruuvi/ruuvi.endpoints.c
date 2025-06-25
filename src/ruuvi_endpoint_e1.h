/**
 * Ruuvi Endpoint 0xE1 helper.
 * Defines necessary data for creating a Ruuvi data format 0xE1 broadcast.
 *
 * License: BSD-3
 * Author: TheSomeMan
 */

#ifndef RUUVI_ENDPOINT_E1_H
#define RUUVI_ENDPOINT_E1_H

#include "ruuvi_endpoints.h"
#include <stdbool.h>

#define RE_E1_DESTINATION (0xE1U)

#define RE_E1_INVALID_TEMPERATURE (0x8000)
#define RE_E1_INVALID_HUMIDITY    (0xFFFFU)
#define RE_E1_INVALID_PRESSURE    (0xFFFFU)
#define RE_E1_INVALID_PM          (0xFFFFU)
#define RE_E1_INVALID_CO2         (0xFFFFU)
#define RE_E1_INVALID_VOC         (0x1FFU)
#define RE_E1_INVALID_NOX         (0x1FFU)
#define RE_E1_INVALID_LUMINOSITY  (0xFFFFFFU)
#define RE_E1_INVALID_SOUND_DBA   (0x1FFU)
#define RE_E1_INVALID_SEQUENCE    (0xFFFFFFU)
#define RE_E1_INVALID_MAC         (0xFFFFFFFFFFFFU)

#define RE_E1_DATA_LENGTH (40U)

#define RE_E1_TEMPERATURE_MAX   (163.835f)
#define RE_E1_TEMPERATURE_RATIO (200.0f)

#define RE_E1_HUMIDITY_MIN   (0.0f)
#define RE_E1_HUMIDITY_MAX   (100.0f)
#define RE_E1_HUMIDITY_RATIO (400.0f)

#define RE_E1_PRESSURE_MIN   (50000.0f)
#define RE_E1_PRESSURE_MAX   (115534.0f)
#define RE_E1_PRESSURE_RATIO (1.0f)

#define RE_E1_PM_MIN   (0.0f)
#define RE_E1_PM_MAX   (1000.0f)
#define RE_E1_PM_RATIO (10.0f)

#define RE_E1_CO2_MIN   (0.0f)
#define RE_E1_CO2_MAX   (40000.0f)
#define RE_E1_CO2_RATIO (1.0f)

#define RE_E1_VOC_MIN   (0.0f)
#define RE_E1_VOC_MAX   (500.0f)
#define RE_E1_VOC_RATIO (1.0f)

#define RE_E1_NOX_MIN   (0.0f)
#define RE_E1_NOX_MAX   (500.0f)
#define RE_E1_NOX_RATIO (1.0f)

#define RE_E1_LUMINOSITY_MIN   (0.0f)
#define RE_E1_LUMINOSITY_MAX   (144284.0f)
#define RE_E1_LUMINOSITY_RATIO (100.0f)

#define RE_E1_SOUND_DBA_MIN   (18.0f)
#define RE_E1_SOUND_DBA_MAX   (120.0f)
#define RE_E1_SOUND_DBA_RATIO (5.0f)

#define RE_E1_FLAG_CALIBRATION_IN_PROGRESS_BIT_OFFSET (0U)
#define RE_E1_FLAG_BUTTON_PRESSED_BIT_OFFSET          (1U)
#define RE_E1_FLAG_RTC_RUNNING_ON_BOOT_BIT_OFFSET     (2U)
#define RE_E1_SOUND_DBA_INST_BIT9_OFFSET              (3U)
#define RE_E1_SOUND_DBA_AVG_BIT9_OFFSET               (4U)
#define RE_E1_SOUND_DBA_PEAK_BIT9_OFFSET              (5U)
#define RE_E1_VOC_BIT9_OFFSET                         (6U)
#define RE_E1_NOX_BIT9_OFFSET                         (7U)

#define RE_E1_FLAGS_CALIBRATION_IN_PROGRESS (1U << RE_E1_FLAG_CALIBRATION_IN_PROGRESS_BIT_OFFSET)
#define RE_E1_FLAGS_BUTTON_PRESSED          (1U << RE_E1_FLAG_BUTTON_PRESSED_BIT_OFFSET)
#define RE_E1_FLAGS_RTC_RUNNING_ON_BOOT     (1U << RE_E1_FLAG_RTC_RUNNING_ON_BOOT_BIT_OFFSET)
#define RE_E1_FLAGS_SOUND_DBA_INST_B9       (1U << RE_E1_SOUND_DBA_INST_BIT9_OFFSET)
#define RE_E1_FLAGS_SOUND_DBA_AVG_B9        (1U << RE_E1_SOUND_DBA_AVG_BIT9_OFFSET)
#define RE_E1_FLAGS_SOUND_DBA_PEAK_B9       (1U << RE_E1_SOUND_DBA_PEAK_BIT9_OFFSET)
#define RE_E1_FLAGS_VOC_B9                  (1U << RE_E1_VOC_BIT9_OFFSET)
#define RE_E1_FLAGS_NOX_B9                  (1U << RE_E1_NOX_BIT9_OFFSET)

#define RE_E1_OFFSET_PAYLOAD (4U)

#define RE_E1_RAW_BUF_SIZE (RE_E1_OFFSET_PAYLOAD + RE_E1_DATA_LENGTH + 4U)

#define RE_E1_OFFSET_HEADER (0U)

#define RE_E1_OFFSET_TEMPERATURE_MSB (1U)
#define RE_E1_OFFSET_TEMPERATURE_LSB (2U)

#define RE_E1_OFFSET_HUMIDITY_MSB (3U)
#define RE_E1_OFFSET_HUMIDITY_LSB (4U)

#define RE_E1_OFFSET_PRESSURE_MSB (5U)
#define RE_E1_OFFSET_PRESSURE_LSB (6U)

#define RE_E1_OFFSET_PM_1_0_MSB (7U)
#define RE_E1_OFFSET_PM_1_0_LSB (8U)

#define RE_E1_OFFSET_PM_2_5_MSB (9U)
#define RE_E1_OFFSET_PM_2_5_LSB (10U)

#define RE_E1_OFFSET_PM_4_0_MSB (11U)
#define RE_E1_OFFSET_PM_4_0_LSB (12U)

#define RE_E1_OFFSET_PM_10_0_MSB (13U)
#define RE_E1_OFFSET_PM_10_0_LSB (14U)

#define RE_E1_OFFSET_CO2_MSB (15U)
#define RE_E1_OFFSET_CO2_LSB (16U)

#define RE_E1_OFFSET_VOC (17U)

#define RE_E1_OFFSET_NOX (18U)

#define RE_E1_OFFSET_LUMINOSITY_MSB (19U)
#define RE_E1_OFFSET_LUMINOSITY_MID (20U)
#define RE_E1_OFFSET_LUMINOSITY_LSB (21U)

#define RE_E1_OFFSET_SOUND_DBA_INST (22U)
#define RE_E1_OFFSET_SOUND_DBA_AVG  (23U)
#define RE_E1_OFFSET_SOUND_DBA_PEAK (24U)

#define RE_E1_OFFSET_SEQ_CNT_MSB (25U)
#define RE_E1_OFFSET_SEQ_CNT_MID (26U)
#define RE_E1_OFFSET_SEQ_CNT_LSB (27U)

#define RE_E1_OFFSET_FLAGS (28U)

#define RE_E1_OFFSET_RESERVED (29U)

#define RE_E1_OFFSET_ADDR_MSB (34U)

/** @brief Description of flags field */
typedef struct re_e1_flags_t
{
    bool flag_calibration_in_progress : 1; //!< Flag: Calibration in progress
    bool flag_button_pressed : 1;          //!< Flag: Button pressed
    bool flag_rtc_running_on_boot : 1;     //!< Flag: RTC was running on boot
} re_e1_flags_t;

/** @brief All data required for Ruuvi dataformat 0xE1 package. */
typedef struct
{
    re_float temperature_c;       //!< Temperature in degrees Celsius.
    re_float humidity_rh;         //!< Humidity in relative humidity percentage.
    re_float pressure_pa;         //!< Humidity in relative humidity percentage.
    re_float pm1p0_ppm;           /*!< Particulate matter mass concentration PM1.0
                                    in micrograms/m3. */
    re_float pm2p5_ppm;           /*!< Particulate matter mass concentration PM2.5
                                    in micrograms/m3. */
    re_float pm4p0_ppm;           /*!< Particulate matter mass concentration PM4.0
                                    in micrograms/m3. */
    re_float pm10p0_ppm;          /*!< Particulate matter mass concentration PM10.0
                                    in micrograms/m3. */
    re_float      co2;            //!< CO2 concentration in ppm
    re_float      voc;            //!< VOC index points.
    re_float      nox;            //!< NOx index points.
    re_float      luminosity;     //!< Luminosity.
    re_float      sound_dba_inst; //!< Sound dBA instant.
    re_float      sound_dba_avg;  //!< Sound dBA average.
    re_float      sound_dba_peak; //!< Sound dBA peak.
    uint32_t      seq_cnt;        //!< Running counter of measurement.
    re_e1_flags_t flags;          //!< Flags for additional information.
    uint64_t      address;        //!< BLE address of device, most significant byte first.
} re_e1_data_t;

/**
 * @brief Encode given data to given buffer in Ruuvi DFxE1.
 *
 * NAN can be used as a placeholder for invalid / not available values.
 *
 * @param[out] buffer uint8_t array with length of 24 bytes.
 * @param[in] data Struct containing all necessary information
 *            for encoding the data into buffer.
 * @retval RE_SUCCESS if data was encoded successfully.
 */
re_status_t
re_e1_encode (uint8_t * const buffer, const re_e1_data_t * data);

/**
 * @brief Checks if the provided buffer conforms to the Ruuvi DFxE1 format.
 *
 * This function examines the input buffer to determine if its content
 * represents data in the Ruuvi DFxE1 format.
 *
 * @param[in] p_buffer Pointer to a uint8_t input array with a length of 31 bytes to be checked.
 * @return Returns 'true' if the buffer format is Ruuvi DFxE1, 'false' otherwise.
 */
bool
re_e1_check_format (const uint8_t * const p_buffer);

/**
 * @brief Decodes a given buffer using the Ruuvi DFxE1 format.
 *
 * Ruuvi DFxE1 is a data format used by the Ruuvi AirQ environmental sensor.
 *
 * @param[in] p_buffer Pointer to a uint8_t input array with a length of 31 bytes
 *  representing a Bluetooth frame with Ruuvi DFxE1 formatted payload.
 * @param[out] p_data Pointer to a re_e1_data_t struct.
 *  After the function executes, this struct will contain the data decoded from the input buffer.
 * @return Returns RE_SUCCESS if the data was decoded successfully,
 *  i.e., if the input buffer was a valid Ruuvi DFxE1 buffer and `p_data` now points to a valid `re_e1_data_t` object.
 *  If the decoding fails, the function returns a code indicating the type of error occurred.
 */
re_status_t
re_e1_decode (const uint8_t * const p_buffer, re_e1_data_t * const p_data);

/**
 * @brief Create invalid Ruuvi DFxE1 data.
 * @param measurement_cnt Running counter of measurement.
 * @param radio_mac BLE address of device.
 * @return /ref re_e1_data_t with all values set to NAN.
 */
re_e1_data_t
re_e1_data_invalid (const uint16_t measurement_cnt, const uint64_t radio_mac);

#endif
