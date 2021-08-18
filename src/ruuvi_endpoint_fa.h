#ifndef RUUVI_ENDPOINT_FA_H
#define RUUVI_ENDPOINT_FA_H

/**
 * Ruuvi Endpoint 0xFA helper.
 * Deprecated, encrypted data format 3.
 *
 * @file ruuvi_endpoint_fa.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2002-08-17
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "ruuvi_endpoints.h"
#include <stdint.h>
#include <stddef.h>

#define RE_FA_DESTINATION       (0xFAU)
#define RE_FA_INVALID_DATA      (0U)
#define RE_FA_DATA_LENGTH       (23U)
#define RE_FA_CIPHERTEXT_LENGTH (16U)

#define RE_FA_OFFSET_HEADER               (0U)
#define RE_FA_OFFSET_HUMIDITY             (1U)
#define RE_FA_OFFSET_TEMPERATURE_DECIMAL  (2U)
#define RE_FA_OFFSET_TEMPERATURE_FRACTION (3U)
#define RE_FA_OFFSET_PRESSURE_MSB         (4U)
#define RE_FA_OFFSET_PRESSURE_LSB         (5U)
#define RE_FA_OFFSET_ACCELERATIONX_MSB    (6U)
#define RE_FA_OFFSET_ACCELERATIONX_LSB    (7U)
#define RE_FA_OFFSET_ACCELERATIONY_MSB    (8U)
#define RE_FA_OFFSET_ACCELERATIONY_LSB    (9U)
#define RE_FA_OFFSET_ACCELERATIONZ_MSB    (10U)
#define RE_FA_OFFSET_ACCELERATIONZ_LSB    (11U)
#define RE_FA_OFFSET_VOLTAGE_MSB          (12U)
#define RE_FA_OFFSET_VOLTAGE_LSB          (13U)
#define RE_FA_OFFSET_COUNTER              (14U)
#define RE_FA_OFFSET_TRAILING_NULL_1      (15U)
#define RE_FA_OFFSET_TRAILING_NULL_2      (16U)
#define RE_FA_OFFSET_ADDRESS_MSB          (17U)
#define RE_FA_OFFSET_ADDRESS_LSB          (RE_FA_OFFSET_ADDRESS_MSB + 5U)

/** @brief All data required for Ruuvi dataformat fa package. */
typedef struct
{
    float humidity_rh;
    //!< Humidity in relative humidity percentage.
    float pressure_pa;
    //!< Pressure in pascals.
    float temperature_c;
    //!< Temperature in celcius.
    float accelerationx_g;
    //!< Acceleration along X-axis, can be RMS over some period.
    float accelerationy_g;
    //!< Acceleration along Y-axis, can be RMS over some period.
    float accelerationz_g;
    //!< Acceleration along Z-axis, can be RMS over some period.
    float battery_v;
    //!< Battery voltage, preferably under load such as radio TX.
    uint8_t message_counter;
    //!< Counter for message, used for preventing replay.
    uint64_t address;
    //!< BLE address of device, most significant byte first.
} re_fa_data_t;

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
typedef uint32_t (*re_fa_encrypt_fp) (const uint8_t * const cleartext,
                                      uint8_t * const ciphertext,
                                      const size_t data_size,
                                      const uint8_t * const key,
                                      const size_t key_size);

/**
 * @brief Encode data to Ruuvi Format FA.
 *
 * @param[out] buffer 23-byte buffer into which data is encoded.
 * @param[in]  data Data to encode.
 * @param[in]  invalid re_float value treated as not available. NAN recommended.
 * @param[in]  Encryption key for data format.
 * @param[in]
 * @retval RE_SUCCESS if data was encoded successfully.
 */
re_status_t re_fa_encode (uint8_t * const buffer,
                          const re_fa_data_t * const data,
                          const re_float invalid,
                          re_fa_encrypt_fp cipher,
                          const uint8_t * const key,
                          const size_t key_size);

#endif // RUUVI_ENDPOINT_FA_H
