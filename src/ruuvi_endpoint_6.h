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

#define RE_6_DESTINATION          (0x06U)
#define RE_6_INVALID_MAC          (0xFFFFFFFFFFFFU)
#define RE_6_INVALID_PM           (0xFFFFU)
#define RE_6_INVALID_CO2          (0xFFFFU)
#define RE_6_INVALID_HUMIDITY     (1023)
#define RE_6_INVALID_VOC_INDEX    (511)
#define RE_6_INVALID_NOX_INDEX    (511)
#define RE_6_INVALID_TEMPERATURE  (0x400U)
#define RE_6_INVALID_SEQUENCE     (0xFFFFU)

#define RE_6_DATA_LENGTH          (24U)


#define RE_6_PM_MAX               (1000.0f)
#define RE_6_PM_MIN               (0.0f)

#define RE_6_CO2_MAX              (40000.0f)
#define RE_6_CO2_MIN              (0.0f)

#define RE_6_HUMIDITY_MAX         (100.0f)
#define RE_6_HUMIDITY_MIN         (0.0f)

#define RE_6_VOC_INDEX_MAX        (500.0f)
#define RE_6_VOC_INDEX_MIN        (1.0f)

#define RE_6_NOX_INDEX_MAX        (500.0f)
#define RE_6_NOX_INDEX_MIN        (1.0f)

#define RE_6_TEMPERATURE_MAX      (102.3f)
#define RE_6_TEMPERATURE_MIN      (-102.3f)

#define RE_6_SEQ_CTR_MAX          (65534)
#define RE_6_SEQ_CTR_MIN          (0)

#define RE_6_OFFSET_PAYLOAD          (7U)

#define RE_6_OFFSET_HEADER           (0U)

#define RE_6_OFFSET_PM_1_0_MSB       (1U)
#define RE_6_OFFSET_PM_1_0_LSB       (2U)
#define RE_6_OFFSET_PM_1_0_OFS       (0U)
#define RE_6_OFFSET_PM_MASK          (0xFFFFU)

#define RE_6_OFFSET_PM_2_5_MSB       (3U)
#define RE_6_OFFSET_PM_2_5_LSB       (4U)
#define RE_6_OFFSET_PM_2_5_OFS       (0U)
#define RE_6_OFFSET_PM_2_5_MASK      (0xFFFFU)

#define RE_6_OFFSET_PM_4_0_MSB       (5U)
#define RE_6_OFFSET_PM_4_0_LSB       (6U)
#define RE_6_OFFSET_PM_4_0_OFS       (0U)
#define RE_6_OFFSET_PM_4_0_MASK      (0xFFFFU)

#define RE_6_OFFSET_PM_10_0_MSB      (7U)
#define RE_6_OFFSET_PM_10_0_LSB      (8U)
#define RE_6_OFFSET_PM_10_0_OFS      (0U)
#define RE_6_OFFSET_PM_10_0_MASK     (0xFFFFU)

#define RE_6_OFFSET_CO2_MSB          (9U)
#define RE_6_OFFSET_CO2_LSB          (10U)
#define RE_6_OFFSET_CO2_OFS          (0U)
#define RE_6_OFFSET_CO2_MASK         (0xFFFFU)

#define RE_6_OFFSET_HUMIDITY_MSB     (11U)
#define RE_6_OFFSET_HUMIDITY_LSB     (12U)
#define RE_6_OFFSET_HUMIDITY_OFS     (6U)
#define RE_6_OFFSET_HUMIDITY_MASK    (0x3FFU)

#define RE_6_OFFSET_VOC_INDEX_MSB    (12U)
#define RE_6_OFFSET_VOC_INDEX_LSB    (13U)
#define RE_6_OFFSET_VOC_INDEX_OFS    (5U)
#define RE_6_OFFSET_VOC_INDEX_MASK   (0x1FFU)

#define RE_6_OFFSET_NOX_INDEX_MSB    (13U)
#define RE_6_OFFSET_NOX_INDEX_LSB    (14U)
#define RE_6_OFFSET_NOX_INDEX_OFS    (4U)
#define RE_6_OFFSET_NOX_INDEX_MASK   (0x1FFU)

#define RE_6_OFFSET_TEMPERATURE_MSB  (14U)
#define RE_6_OFFSET_TEMPERATURE_LSB  (15U)
#define RE_6_OFFSET_TEMPERATURE_OFS  (0U)
#define RE_6_OFFSET_TEMPERATURE_MASK (0x7FFU)

#define RE_6_OFFSET_SEQ_CTR_MSB      (16U)
#define RE_6_OFFSET_SEQ_CTR_LSB      (17U)

#define RE_6_OFFSET_ADDR_MSB         (18U)

/** @brief All data required for Ruuvi dataformat 6 package. */
typedef struct
{
    re_float pm1p0_ppm;
    //!< Particulate matter mass concentration PM1.0 in micrograms/m3.
    re_float pm2p5_ppm;
    //!< Particulate matter mass concentration PM2.5 in micrograms/m3.
    re_float pm4p0_ppm;
    //!< Particulate matter mass concentration PM4.0 in micrograms/m3.
    re_float pm10p0_ppm;
    //!< Particulate matter mass concentration PM10.0 in micrograms/m3.
    re_float co2;
    //!< CO2 concentration in ppm
    re_float humidity_rh;
    //!< Humidity in relative humidity percentage.
    re_float voc_index;
    //!< VOC index points.
    re_float nox_index;
    //!< NOx index points.
    re_float temperature_c;
    //!< Temperature in degrees Celsius.
    uint16_t measurement_count;
    //!< Running counter of measurement.
    uint64_t address;
    //!< BLE address of device, most significant byte first.
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
re_status_t re_6_encode (uint8_t * const buffer, const re_6_data_t * data);


/**
 * @brief Checks if the provided buffer conforms to the Ruuvi DF6 format.
 *
 * This function examines the input buffer to determine if its content
 * represents data in the Ruuvi DF6 format.
 *
 * @param[in] p_buffer Pointer to a uint8_t input array with a length of 31 bytes to be checked.
 * @return Returns 'true' if the buffer format is Ruuvi DF6, 'false' otherwise.
 */
bool re_6_check_format (const uint8_t * const p_buffer);


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
re_status_t re_6_decode (const uint8_t * const p_buffer, re_6_data_t * const p_data);

/**
 * @brief Create invalid Ruuvi DF6 data.
 * @param measurement_cnt Running counter of measurement.
 * @param radio_mac BLE address of device.
 * @return /ref re_6_data_t with all values set to NAN.
 */
re_6_data_t re_6_data_invalid (const uint16_t measurement_cnt, const uint64_t radio_mac);

#endif
