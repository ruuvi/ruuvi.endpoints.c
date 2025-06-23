/**
 * Ruuvi Endpoint 6 helper.
 * Defines necessary data for creating a Ruuvi data format 6 broadcast.
 *
 * License: BSD-3
 * Author: TheSomeMan
 */

#ifndef RUUVI_ENDPOINT_6_H
#define RUUVI_ENDPOINT_6_H
#include "ruuvi_endpoints.h"
#include <stdbool.h>

#define RE_6_DESTINATION (0x06U)

#define RE_6_INVALID_TEMPERATURE (0x8000)
#define RE_6_INVALID_HUMIDITY    (0xFFFFU)
#define RE_6_INVALID_PRESSURE    (0xFFFFU)
#define RE_6_INVALID_PM          (0xFFFFU)
#define RE_6_INVALID_CO2         (0xFFFFU)
#define RE_6_INVALID_VOC         (0x1FFU)
#define RE_6_INVALID_NOX         (0x1FFU)
#define RE_6_INVALID_LUMINOSITY  (0xFFU)
#define RE_6_INVALID_SOUND       (0x1FFU)
#define RE_6_INVALID_MAC         (0xFFFFFFU)

#define RE_6_DATA_LENGTH (20U)

#define RE_6_TEMPERATURE_MAX   (163.835f)
#define RE_6_TEMPERATURE_RATIO (200.0f)

#define RE_6_HUMIDITY_MIN   (0.0f)
#define RE_6_HUMIDITY_MAX   (100.0f)
#define RE_6_HUMIDITY_RATIO (400.0f)

#define RE_6_PRESSURE_MIN   (50000.0f)
#define RE_6_PRESSURE_MAX   (115534.0f)
#define RE_6_PRESSURE_RATIO (1.0f)

#define RE_6_PM_MIN   (0.0f)
#define RE_6_PM_MAX   (1000.0f)
#define RE_6_PM_RATIO (10.0f)

#define RE_6_CO2_MIN   (0.0f)
#define RE_6_CO2_MAX   (40000.0f)
#define RE_6_CO2_RATIO (1.0f)

#define RE_6_VOC_MIN   (0.0f)
#define RE_6_VOC_MAX   (500.0f)
#define RE_6_VOC_RATIO (1.0f)

#define RE_6_NOX_MIN   (0.0f)
#define RE_6_NOX_MAX   (500.0f)
#define RE_6_NOX_RATIO (1.0f)

#define RE_6_LUMINOSITY_MIN   (0.0f)
#define RE_6_LUMINOSITY_MAX   (65535.0f)
#define RE_6_LUMINOSITY_RATIO (254.0f / logf(RE_6_LUMINOSITY_MAX + 1))

#define RE_6_SOUND_DBA_AVG_MIN   (27.0f)
#define RE_6_SOUND_DBA_AVG_MAX   (129.0f)
#define RE_6_SOUND_DBA_AVG_RATIO (5.0f)

#define RE_6_MAC_MIN (0UL)
#define RE_6_MAC_MAX (0xFFFFFFUL)

#define RE_6_FLAG_CALIBRATION_IN_PROGRESS_BIT_OFFSET (0U)
#define RE_6_FLAG_BUTTON_PRESSED_BIT_OFFSET          (1U)
#define RE_6_FLAG_RTC_RUNNING_ON_BOOT_BIT_OFFSET     (2U)
#define RE_6_SOUND_DBA_AVG_BIT9_OFFSET               (4U)
#define RE_6_VOC_BIT9_OFFSET                         (6U)
#define RE_6_NOX_BIT9_OFFSET                         (7U)

#define RE_6_FLAGS_CALIBRATION_IN_PROGRESS (1U << RE_6_FLAG_CALIBRATION_IN_PROGRESS_BIT_OFFSET)
#define RE_6_FLAGS_BUTTON_PRESSED          (1U << RE_6_FLAG_BUTTON_PRESSED_BIT_OFFSET)
#define RE_6_FLAGS_RTC_RUNNING_ON_BOOT     (1U << RE_6_FLAG_RTC_RUNNING_ON_BOOT_BIT_OFFSET)
#define RE_6_FLAGS_SOUND_DBA_AVG_B9        (1U << RE_6_SOUND_DBA_AVG_BIT9_OFFSET)
#define RE_6_FLAGS_VOC_B9                  (1U << RE_6_VOC_BIT9_OFFSET)
#define RE_6_FLAGS_NOX_B9                  (1U << RE_6_NOX_BIT9_OFFSET)

#define RE_6_OFFSET_PAYLOAD (11U)

#define RE_6_OFFSET_HEADER (0U)

#define RE_6_OFFSET_TEMPERATURE_MSB (1U)
#define RE_6_OFFSET_TEMPERATURE_LSB (2U)

#define RE_6_OFFSET_HUMIDITY_MSB (3U)
#define RE_6_OFFSET_HUMIDITY_LSB (4U)

#define RE_6_OFFSET_PRESSURE_MSB (5U)
#define RE_6_OFFSET_PRESSURE_LSB (6U)

#define RE_6_OFFSET_PM_2_5_MSB (7U)
#define RE_6_OFFSET_PM_2_5_LSB (8U)

#define RE_6_OFFSET_CO2_MSB (9U)
#define RE_6_OFFSET_CO2_LSB (10U)

#define RE_6_OFFSET_VOC (11U)
#define RE_6_OFFSET_NOX (12U)

#define RE_6_OFFSET_LUMINOSITY (13U)

#define RE_6_OFFSET_SOUND_DBA_AVG (14U)

#define RE_6_OFFSET_SEQ_CNT2 (15U)

#define RE_6_OFFSET_FLAGS (16U)

#define RE_6_OFFSET_ADDR_3 (17U)
#define RE_6_OFFSET_ADDR_4 (18U)
#define RE_6_OFFSET_ADDR_5 (19U)

/** @brief Description of flags field */
typedef struct re_6_flags_t
{
    bool flag_calibration_in_progress : 1; //!< Flag: Calibration in progress
    bool flag_button_pressed : 1;          //!< Flag: Button pressed
    bool flag_rtc_running_on_boot : 1;     //!< Flag: RTC was running on boot
} re_6_flags_t;

/** @brief Least 24-bits of BLE MAC address. */
typedef struct re_6_mac_addr_24_t
{
    uint8_t byte3; //!< Most significant byte of the least 24-bits of BLE MAC address.
    uint8_t byte4; //!< Second most significant byte of the least 24-bits of BLE MAC address.
    uint8_t byte5; //!< Least significant byte of the least 24-bits of BLE MAC address.
} re_6_mac_addr_24_t;

/** @brief All data required for Ruuvi dataformat 6 package. */
typedef struct
{
    re_float temperature_c;           //!< Temperature in degrees Celsius.
    re_float humidity_rh;             //!< Humidity in relative humidity percentage.
    re_float pressure_pa;             //!< Humidity in relative humidity percentage.
    re_float pm2p5_ppm;               /*!< Particulate matter mass concentration PM2.5.
                                        The value is in micrograms per cubic meter (µg/m³). */
    re_float           co2;           //!< CO2 concentration in ppm
    re_float           voc;           //!< VOC index points.
    re_float           nox;           //!< NOx index points.
    re_float           luminosity;    //!< Luminosity.
    re_float           sound_dba_avg; //!< Sound dBA avg.
    uint8_t            seq_cnt2;      //!< Running counter of measurement.
    re_6_flags_t       flags;         //!< Flags for additional information.
    re_6_mac_addr_24_t mac_addr_24;   //!< Least 24-bits of BLE MAC address of device.
} re_6_data_t;

/**
 * @brief Encode given data to given buffer in Ruuvi DF6.
 *
 * NAN can be used as a placeholder for invalid / not available values.
 *
 * @param[out] buffer uint8_t array with length of 24 bytes.
 * @param[in] data Struct containing all necessary information
 *            for encoding the data into buffer.
 * @retval RE_SUCCESS if data was encoded successfully.
 */
re_status_t
re_6_encode (uint8_t * const buffer, const re_6_data_t * data);

/**
 * @brief Checks if the provided buffer conforms to the Ruuvi DF6 format.
 *
 * This function examines the input buffer to determine if its content
 * represents data in the Ruuvi DF6 format.
 *
 * @param[in] p_buffer Pointer to a uint8_t input array with a length of 31 bytes to be checked.
 * @return Returns 'true' if the buffer format is Ruuvi DF6, 'false' otherwise.
 */
bool
re_6_check_format (const uint8_t * const p_buffer);

/**
 * @brief Decodes a given buffer using the Ruuvi DF6 format.
 *
 * Ruuvi DF6 is a data format used by the Ruuvi AirQ environmental sensor.
 *
 * @param[in] p_buffer Pointer to a uint8_t input array with a length of 31 bytes
 *  representing a Bluetooth frame with Ruuvi DF6 formatted payload.
 * @param[out] p_data Pointer to a re_6_data_t struct.
 *  After the function executes, this struct will contain the data decoded from the input buffer.
 * @return Returns RE_SUCCESS if the data was decoded successfully,
 *  i.e., if the input buffer was a valid Ruuvi DF6 buffer and `p_data` now points to a valid `re_6_data_t` object.
 *  If the decoding fails, the function returns a code indicating the type of error occurred.
 */
re_status_t
re_6_decode (const uint8_t * const p_buffer, re_6_data_t * const p_data);

/**
 * @brief Create invalid Ruuvi DF6 data.
 * @param measurement_cnt Running counter of measurement.
 * @param radio_mac BLE address of device.
 * @return /ref re_6_data_t with all values set to NAN.
 */
re_6_data_t
re_6_data_invalid (const uint16_t measurement_cnt, const uint64_t radio_mac);

#endif
