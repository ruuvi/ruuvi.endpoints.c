#ifndef RUUVI_ENDPOINT_8_H
#define RUUVI_ENDPOINT_8_H

/**
 * Ruuvi Endpoint 0x08 helper.
 * Encrypted data format for RuuviTags.
 *
 * @file ruuvi_endpoint_08.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2021-08-25
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "ruuvi_endpoints.h"
#include <stddef.h>
#include <stdint.h>

#define RE_8_DESTINATION          (0x08U)
#define RE_8_INVALID_TEMPERATURE  (0x8000U)
#define RE_8_INVALID_HUMIDITY     (0xFFFFU)
#define RE_8_INVALID_PRESSURE     (0xFFFFU)
#define RE_8_INVALID_SEQUENCE     (0xFFFFU)
#define RE_8_INVALID_MOVEMENT     (0xFFFFU)
#define RE_8_INVALID_VOLTAGE      (0x07FFU)
#define RE_8_INVALID_POWER        (0x1FU)
#define RE_8_INVALID_MAC          (0xFFFFFFFFFFFFU)
#define RE_8_DATA_LENGTH          (24U)
#define RE_8_CIPHERTEXT_LENGTH    (16U)

#define RE_8_TEMP_MAX          (163.835f)
#define RE_8_TEMP_MIN          (-163.835f)
#define RE_8_HUMI_MAX          (163.835f)
#define RE_8_HUMI_MIN          (0.0f)
#define RE_8_PRES_MAX          (115534.0f)
#define RE_8_PRES_MIN          (50000.0f)
#define RE_8_ACC_MAX           (32.767f)
#define RE_8_ACC_MIN           (-32.767f)
#define RE_8_VOLTAGE_MAX       (3.646f)
#define RE_8_VOLTAGE_MIN       (1.6f)
#define RE_8_TXPWR_MAX         (20)
#define RE_8_TXPWR_MIN         (-40)

#define RE_8_OFFSET_HEADER     (0U)
#define RE_8_OFFSET_CIPHER     (1U)
#define RE_8_OFFSET_TEMP_MSB   (1U)
#define RE_8_OFFSET_TEMP_LSB   (2U)
#define RE_8_OFFSET_HUMI_MSB   (3U)
#define RE_8_OFFSET_HUMI_LSB   (4U)
#define RE_8_OFFSET_PRES_MSB   (5U)
#define RE_8_OFFSET_PRES_LSB   (6U)
#define RE_8_OFFSET_POWER_MSB  (7U)
#define RE_8_OFFSET_POWER_LSB  (8U)
#define RE_8_OFFSET_MVTCTR_MSB (9U)
#define RE_8_OFFSET_MVTCTR_LSB (10U)
#define RE_8_OFFSET_SEQCTR_MSB (11U)
#define RE_8_OFFSET_SEQCTR_LSB (12U)
#define RE_8_RESERVED_BYTES    (4U)
#define RE_8_OFFSET_RESERVED   (13U)
#define RE_8_OFFSET_CRC8       (17U)
#define RE_8_OFFSET_ADDR_MSB   (18U)
#define RE_8_OFFSET_ADDR_LSB   (RE_8_OFFSET_ADDR_MSB + 5U)

/** @brief All data required for Ruuvi dataformat 08 package. */
typedef struct
{
    float humidity_rh;
    //!< Humidity in relative humidity percentage.
    float pressure_pa;
    //!< Pressure in pascals.
    float temperature_c;
    //!< Temperature in celcius.
    float battery_v;
    //!< Battery voltage, preferably under load such as radio TX.
    uint16_t movement_count;
    //!< Number of detected movements.
    uint16_t message_counter;
    //!< Counter for message, used for preventing replay.
    int8_t tx_power;
    //!< Transmission power of radio, in dBm.
    uint64_t address;
    //!< BLE address of device, most significant byte first.
} re_8_data_t;

/**
 * @brief Encryption function for data format.
 *
 * @param[in]  cleartext Data to encrypt
 * @param[out] ciphertext Encrypted data
 * @param[in]  data_size Size of input/output buffers
 * @param[in]  key Data to encrypt data
 * @param[in]  key_size Length of key in bytes
 * @return 0 on success, non-zero error code otherwise.
 */
typedef uint32_t (*re_8_encrypt_fp) (const uint8_t * const cleartext,
                                     uint8_t * const ciphertext,
                                     const size_t data_size,
                                     const uint8_t * const key,
                                     const size_t key_size);

/**
 * @brief Encode data to Ruuvi Format 8.
 *
 * @param[out] buffer 24-byte buffer into which data is encoded.
 * @param[in]  data Data to encode. Use NAN for values not available.
 * @param[in]  cipher Pointer to encryption function.
 * @param[in]  key Encryption key for data format.
 * @param[in]  key_size Encryption key length in bytes.
 * @retval RE_SUCCESS if data was encoded successfully.
 */
re_status_t re_8_encode (uint8_t * const buffer,
                         const re_8_data_t * const data,
                         re_8_encrypt_fp cipher,
                         const uint8_t * const key,
                         const size_t key_size);

#endif // RUUVI_ENDPOINT_8_H
