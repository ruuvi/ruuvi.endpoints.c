#ifndef RUUVI_ENDPOINTS_H
#define RUUVI_ENDPOINTS_H

#ifdef APPLICATION_ENDPOINTS_CONFIGURED
#   include "app_config.h"
#else
#if !defined(RE_3_ENABLED)
#   define RE_3_ENABLED  (1U)
#endif
#if !defined(RE_5_ENABLED)
#   define RE_5_ENABLED  (1U)
#endif
#if !defined(RE_6_ENABLED)
#   define RE_6_ENABLED  (1U)
#endif
#if !defined(RE_8_ENABLED)
#   define RE_8_ENABLED  (1U)
#endif
#if !defined(RE_C5_ENABLED)
#   define RE_C5_ENABLED (1U)
#endif
#if !defined(RE_CA_ENABLED)
#   define RE_CA_ENABLED (1U)
#endif
#if !defined(RE_F0_ENABLED)
#   define RE_F0_ENABLED (1U)
#endif
#if !defined(RE_FA_ENABLED)
#   define RE_FA_ENABLED (1U)
#endif
#if !defined(RE_E0_ENABLED)
#   define RE_E0_ENABLED (1U)
#endif
#if !defined(RE_E1_ENABLED)
#   define RE_E1_ENABLED (1U)
#endif
#if !defined(RE_IBEACON_ENABLED)
#   define RE_IBEACON_ENABLED (1U)
#endif
#endif

#include <stdint.h>

#define RUUVI_ENDPOINTS_SEMVER "4.1.0"          //!< SEMVER of endpoints.

#define RE_SUCCESS                  (0U)        //!< Encoded successfully.
#define RE_ERROR_DATA_SIZE          (1U << 3U)  //!< Data size too large/small.
#define RE_ERROR_INVALID_PARAM      (1U << 4U)  //!< Invalid Parameter.
#define RE_ERROR_NULL               (1U << 11U) //!< Null Pointer.
#define RE_ERROR_ENCODING           (1U << 12U) //!< Data encoding failed.
#define RE_ERROR_DECODING           (1U << 13U) //!< Data decoding failed.
#define RE_ERROR_DECODING_LEN       (1U << 14U) //!< Data decoding len failed.
#define RE_ERROR_DECODING_DELIMITER (1U << 15U) //!< Data decoding delimiter failed.
#define RE_ERROR_DECODING_STX       (1U << 16U) //!< Data decoding stx failed.
#define RE_ERROR_DECODING_ETX       (1U << 17U) //!< Data decoding etx failed.
#define RE_ERROR_DECODING_CRC       (1U << 18U) //!< Data decoding crc failed.
#define RE_ERROR_DECODING_CMD       (1U << 19U) //!< Data decoding cmd failed.
#define RE_ERROR_NOT_IMPLEMENTED    (1U << 24U) //!< Not implemented yet.

#define RE_BYTE_0_SHIFT (0U)
#define RE_BYTE_1_SHIFT (8U)
#define RE_BYTE_2_SHIFT (16U)
#define RE_BYTE_3_SHIFT (24U)
#define RE_BYTE_4_SHIFT (32U)
#define RE_BYTE_5_SHIFT (40U)
#define RE_BYTE_MASK    (0xFFU)
#define RE_BIT1_MASK    (0x01U)

#define RE_BIT_1_SHIFT (1U)

typedef uint32_t re_status_t;                   //!< Status code
typedef float    re_float;                      //!< Ruuvi endpoint float type

typedef uint8_t re_bit_offset_t;                //!< Bit offset type, used for bit fields.


/**
 *  Ruuvi Standard Message consists of 11 bytes: 3 are a header, 8 are payload.
 *  Header has destination, source and type.
 *  Payload format is determined by the type of a message.
 */
#define RE_STANDARD_DESTINATION_INDEX   (0U)
#define RE_STANDARD_SOURCE_INDEX        (1U)
#define RE_STANDARD_OPERATION_INDEX     (2U)
#define RE_STANDARD_PAYLOAD_START_INDEX (3U)
#define RE_STANDARD_HEADER_LENGTH       (3U)
#define RE_STANDARD_PAYLOAD_LENGTH      (8U)
#define RE_STANDARD_MESSAGE_LENGTH      (RE_STANDARD_HEADER_LENGTH + \
                                            RE_STANDARD_PAYLOAD_LENGTH)

/**
 * @brief If endpoint is even, type of a message is considered a write.
 *
 * If it is odd, type is considered a read.
 */
#define RE_STANDARD_OP_READ_BIT                (1U << 0U)
#define RE_STANDARD_SENSOR_CONFIGURATION_WRITE (0x02U)
#define RE_STANDARD_SENSOR_CONFIGURATION_READ  (RE_STANDARD_SENSOR_CONFIGURATION_WRITE | \
                                                    RE_STANDARD_OP_READ_BIT)
#define RE_STANDARD_SENSOR_OFFSET_WRITE        (0x04U)
#define RE_STANDARD_SENSOR_OFFSET_READ         (RE_STANDARD_SENSOR_OFFSET_WRITE | \
                                                    RE_STANDARD_OP_READ_BIT)
#define RE_STANDARD_LOG_CONFIGURATION_WRITE    (0x06U)
#define RE_STANDARD_LOG_CONFIGURATION_READ     (RE_STANDARD_LOG_CONFIGURATION_WRITE | \
                                                    RE_STANDARD_OP_READ_BIT)
#define RE_STANDARD_VALUE_WRITE                (0x08U)
//!< Sensor reads are acknowledged by setting type as value write
#define RE_STANDARD_VALUE_READ                 (RE_STANDARD_VALUE_WRITE | \
                                                    RE_STANDARD_OP_READ_BIT)
#define RE_STANDARD_LOG_VALUE_WRITE            (0x10U)
#define RE_STANDARD_LOG_VALUE_READ             (RE_STANDARD_LOG_VALUE_WRITE | \
                                                    RE_STANDARD_OP_READ_BIT)

#define RE_STANDARD_LOG_MULTI_WRITE            (0x20U)
#define RE_STANDARD_LOG_MULTI_READ             (RE_STANDARD_LOG_MULTI_WRITE | \
                                                    RE_STANDARD_OP_READ_BIT)

#define RE_SYS_CONFIG_WRITE_HEARTBEAT          (0xF2U)
#define RE_SYS_CONFIG_READ_HEARTBEAT           (RE_SYS_CONFIG_WRITE_HEARTBEAT | \
                                                    RE_STANDARD_OP_READ_BIT)

#define RE_STANDARD_OP_TIMEOUT                 (0xE0)  //!< Internal timeout, aborting operation.
#define RE_STANDARD_OP_UNAUTHORIZED            (0xEAU) //!< Operation was unauthorized
#define RE_STANDARD_OP_ERROR                   (0xEEU) //!< internal error has occured

#define RE_LOG_READ_CURRENT_MSB_IDX            (3U)    //!< MSB of current time.
#define RE_LOG_READ_CURRENT_B2_IDX             (4U)    //!< B2 of current time.
#define RE_LOG_READ_CURRENT_B3_IDX             (5U)    //!< B3 of current time.
#define RE_LOG_READ_CURRENT_LSB_IDX            (6U)    //!< LSB of current time.
#define RE_LOG_READ_START_MSB_IDX              (7U)    //!< MSB of first time to read.
#define RE_LOG_READ_START_B2_IDX               (8U)    //!< B2 of first time to read.
#define RE_LOG_READ_START_B3_IDX               (9U)    //!< B3 of first time to read.
#define RE_LOG_READ_START_LSB_IDX              (10U)   //!< LSB of first time to read.
#define RE_LOG_WRITE_TS_MSB_IDX                (3U)    //!< MSB of timestamp.
#define RE_LOG_WRITE_TS_B2_IDX                 (4U)    //!< B2 of timestamp.
#define RE_LOG_WRITE_TS_B3_IDX                 (5U)    //!< B3 of timestamp.
#define RE_LOG_WRITE_TS_LSB_IDX                (6U)    //!< LSB of timestamp.
#define RE_LOG_WRITE_VALUE_MSB_IDX             (7U)    //!< MSB of value.
#define RE_LOG_WRITE_VALUE_B2_IDX              (8U)    //!< B2 of value.
#define RE_LOG_WRITE_VALUE_B3_IDX              (9U)    //!< B3 of value.
#define RE_LOG_WRITE_VALUE_LSB_IDX             (10U)   //!< LSB of value.

#define RE_LOG_WRITE_MULTI_NUM_RECORDS_IDX     (3U)    //!< Number of records.
#define RE_LOG_WRITE_MULTI_RECORD_LEN_IDX      (4U)    //!< Length of record.
#define RE_LOG_WRITE_MULTI_PAYLOAD_IDX         (5U)    //!< Start of payload.

#define RE_LOG_WRITE_AIRQ_TIMESTAMP_MSB_OFS     (0U)    //!< MSB offset of timestamp.
#define RE_LOG_WRITE_AIRQ_PAYLOAD_OFS           (4U)    //!< Offset of payload.
#define RE_LOG_WRITE_AIRQ_DATA_FORMAT_OFS       (4U)    //!< Offset of data format.
#define RE_LOG_WRITE_AIRQ_TEMPERATURE_MSB_OFS   (5U)    //!< MSB offset of temperature.
#define RE_LOG_WRITE_AIRQ_HUMIDITY_MSB_OFS      (7U)    //!< MSB offset of humidity.
#define RE_LOG_WRITE_AIRQ_PRESSURE_MSB_OFS      (9U)    //!< MSB offset of pressure.
#define RE_LOG_WRITE_AIRQ_PM1P0_MSB_OFS         (11U)   //!< MSB offset of pm1p0.
#define RE_LOG_WRITE_AIRQ_PM2P5_MSB_OFS         (13U)   //!< MSB offset of pm2p5.
#define RE_LOG_WRITE_AIRQ_PM4P0_MSB_OFS         (15U)   //!< MSB offset of pm4p0.
#define RE_LOG_WRITE_AIRQ_PM10P0_MSB_OFS        (17U)   //!< MSB offset of pm10p0.
#define RE_LOG_WRITE_AIRQ_CO2_MSB_OFS           (19U)   //!< MSB offset of CO2.
#define RE_LOG_WRITE_AIRQ_VOC_MSB_OFS           (21U)   //!< Offset of VOC index.
#define RE_LOG_WRITE_AIRQ_NOX_MSB_OFS           (22U)   //!< Offset of NOx index.
#define RE_LOG_WRITE_AIRQ_LUMINOSITY_MSB_OFS    (23U)   //!< MSB offset of luminosity.
#define RE_LOG_WRITE_AIRQ_SOUND_INST_DBA_OFS    (26U)   //!< Offset of sound_inst_dba.
#define RE_LOG_WRITE_AIRQ_SOUND_AVG_DBA_OFS     (27U)   //!< Offset of sound_avg_dba.
#define RE_LOG_WRITE_AIRQ_SOUND_PEAK_SPL_DB_OFS (28U)   //!< Offset of sound_peak_spl_db.
#define RE_LOG_WRITE_AIRQ_SEQ_CNT_MSB_OFS       (29U)   //!< Offset of measurement sequence counter.
#define RE_LOG_WRITE_AIRQ_FLAGS_OFS             (32U)   //!< Offset of flags.
#define RE_LOG_WRITE_AIRQ_FW_VER_OFS            (33U)   //!< Offset of fw version.
#define RE_LOG_WRITE_AIRQ_RESERVED1_OFS         (34U)   //!< Reserved for future use.
#define RE_LOG_WRITE_AIRQ_RESERVED2_OFS         (32U)   //!< Reserved for future use.
#define RE_LOG_WRITE_AIRQ_RESERVED3_OFS         (33U)   //!< Reserved for future use.
#define RE_LOG_WRITE_AIRQ_RESERVED4_OFS         (33U)   //!< Reserved for future use.
#define RE_LOG_WRITE_AIRQ_RECORD_LEN            (38U)   //!< Length of record.


#define RE_STANDARD_DESTINATION_ACCELERATION   (0x4AU) //!< XYZ acceleration combined
#define RE_STANDARD_DESTINATION_ACCELERATION_X (0x40U) //!< X acceleration
#define RE_STANDARD_DESTINATION_ACCELERATION_Y (0x41U) //!< Y acceleration
#define RE_STANDARD_DESTINATION_ACCELERATION_Z (0x42U) //!< Z acceleration
#define RE_STANDARD_DESTINATION_GYRATION       (0x4BU) //!< XYZ gyration combined
#define RE_STANDARD_DESTINATION_GYRATION_X     (0x43U) //!< X gyration
#define RE_STANDARD_DESTINATION_GYRATION_Y     (0x44U) //!< Y gyration
#define RE_STANDARD_DESTINATION_GYRATION_Z     (0x45U) //!< Z gyration
#define RE_STANDARD_DESTINATION_TEMPERATURE    (0x30U) //!< Temperature
#define RE_STANDARD_DESTINATION_HUMIDITY       (0x31U) //!< Humidity
#define RE_STANDARD_DESTINATION_PRESSURE       (0x32U) //!< Pressure
#define RE_STANDARD_DESTINATION_ENVIRONMENTAL  (0x3AU) //!< Temp Humi Pres combined.
#define RE_STANDARD_DESTINATION_AIRQ           (0x3BU) //!< All AIRQ measurements combined.
#define RE_STANDARD_DESTINATION_ADC_BATTERY    (0x20U) //!< ADC battery vs GND
#define RE_STANDARD_DESTINATION_RTC            (0x21U) //!< RTC value
#define RE_STANDARD_DESTINATION_PASSWORD       (0x2AU) //!< Password endpoint.
#define RE_STANDARD_DESTINATION_SYS_CONFIG     (0x22U) //!< Configuration of system settings

typedef enum
{
    RE_ACC_XYZ  = RE_STANDARD_DESTINATION_ACCELERATION,
    RE_ACC_X    = RE_STANDARD_DESTINATION_ACCELERATION_X,
    RE_ACC_Y    = RE_STANDARD_DESTINATION_ACCELERATION_Y,
    RE_ACC_Z    = RE_STANDARD_DESTINATION_ACCELERATION_Z,
    RE_GYR_XYZ  = RE_STANDARD_DESTINATION_GYRATION,
    RE_GYR_X    = RE_STANDARD_DESTINATION_GYRATION_X,
    RE_GYR_Y    = RE_STANDARD_DESTINATION_GYRATION_Y,
    RE_GYR_Z    = RE_STANDARD_DESTINATION_GYRATION_Z,
    RE_ENV_ALL  = RE_STANDARD_DESTINATION_ENVIRONMENTAL,
    RE_ENV_AIRQ = RE_STANDARD_DESTINATION_AIRQ,
    RE_ENV_TEMP = RE_STANDARD_DESTINATION_TEMPERATURE,
    RE_ENV_HUMI = RE_STANDARD_DESTINATION_HUMIDITY,
    RE_ENV_PRES = RE_STANDARD_DESTINATION_PRESSURE,
    RE_SEC_PASS = RE_STANDARD_DESTINATION_PASSWORD
} re_type_t;

// Scaling factors float -> i32.
#define RE_STANDARD_ACCELERATION_SF            (1000.0F)    //!< milli-mg.
#define RE_STANDARD_GYRATION_SF                (1000.0F)    //!< milli-dps.
#define RE_STANDARD_VOLTAGE_SF                 (1000.0F)    //!< milli-V.
#define RE_STANDARD_TEMPERATURE_SF             (100.0F)     //!< centi-C.
#define RE_STANDARD_HUMIDITY_SF                (100.0F)     //!< centi-RH%.
#define RE_STANDARD_PRESSURE_SF                (1.0F)       //!< Pa.
#define RE_STANDARD_INVALID_I32                (0xFFFFFFFF) //!< Error value.

typedef enum
{
    RE_SENSOR_CONFIG_W = RE_STANDARD_SENSOR_CONFIGURATION_WRITE,
    RE_SENSOR_CONFIG_R = RE_STANDARD_SENSOR_CONFIGURATION_READ,
    RE_LOG_W = RE_STANDARD_LOG_VALUE_WRITE,
    RE_LOG_R = RE_STANDARD_LOG_VALUE_READ,
    RE_LOG_W_MULTI = RE_STANDARD_LOG_MULTI_WRITE,
    RE_LOG_R_MULTI = RE_STANDARD_LOG_MULTI_READ,
} re_op_t;

/**
 * @brief Get current time for log read command to compensate timestamps.
 *
 * @param[in] raw_message 11-byte payload from central.
 * @return u32 timestamp, 0 if raw_message was NULL.
 */
uint32_t re_std_log_current_time (const uint8_t * const raw_message);

/**
 * @brief Get start time for log read command.
 *
 * @param[in] raw_message 11-byte payload from central.
 * @return u32 timestamp, 0 if raw_message was NULL.
 */
uint32_t re_std_log_start_time (const uint8_t * const raw_message);

/**
 * @brief Write a log write header to given buffer.
 *
 * @param[out] buffer 11-byte buffer to which header will be written.
 * @param[in]  source Source endpoint of data.
 * @retval RE_SUCCESS Header was written successfully.
 * @retval RE_ERROR_NULL Buffer was NULL.
 */
re_status_t re_log_write_header (uint8_t * const buffer, const uint8_t source);

/**
 * @brief write a log element timestamp to the given buffer.
 *
 * @param[out] 11-byte buffer to write data to.
 * @param[in]  timestamp_ms Timestamp as it will be sent to remote.
 * @retval RE_SUCCESS encoding was successful.
 * @retval RE_ERROR_NULL buffer was NULL
 * @retval RE_ERROR_INVALID_PARAM timestamp cannot be encoded as 32-bit second value.
 */
re_status_t re_log_write_timestamp (uint8_t * const buffer, const uint64_t timestamp_ms);

/**
 * @brief Encode given float to the given buffer.
 *
 * @param[out] buffer 11-byte buffer to which data is encoded. 0xFFFFFFFF on error.
 * @param[in]  data Float value to encode.
 * @param[in]  source Ruuvi Endpoint data source, e.g. RE_STANDARD_DESTINATION_ACCELERATION_X.
 *
 * @retval RE_SUCCESS Data was encoded successfully.
 * @retval RE_ERROR_NULL Buffer was NULL.
 * @retval RE_ERROR_INVALID_PARAMETER if data is NAN or inf.
 * @retval RE_NOT_IMPLEMENTED if there's no encoding for given data source.
 */
re_status_t re_log_write_data (uint8_t * const buffer, const re_float data,
                               const uint8_t source);

/**
 * @brief Clip given float to the given range.
 *
 * @param[out] value Value to clip.
 * @param[in]  min Minimum value. Value pointer will be >= min.
 * @param[in]  max Maximum value. Value pointer will be <= max.
 */
void re_clip (float * const value, const float min, const float max);

static inline int16_t re_clip_int16_t (const int16_t value, const int16_t min,
                                       const int16_t max)
{
    if (value > max)
    {
        return max;
    }

    if (value < min)
    {
        return min;
    }

    return value;
}

static inline uint16_t re_clip_uint16_t (const uint16_t value, const uint16_t min,
        const uint16_t max)
{
    if (value > max)
    {
        return max;
    }

    if (value < min)
    {
        return min;
    }

    return value;
}

static inline re_float re_clip_float (const re_float value, const re_float min,
                                      const re_float max)
{
    if (value > max)
    {
        return max;
    }

    if (value < min)
    {
        return min;
    }

    return value;
}

#define RE_CLIP(value, min, max) \
  _Generic((value), /* type = result of (value) + (min) + (max) */ \
    int16_t:  re_clip_int16_t,   \
    uint16_t: re_clip_uint16_t,  \
    float:    re_clip_float      \
  )( (value), (min), (max) )

/**
 * @brief Calculate CRC8 checksum of a data array. CRC polynomial is 0x07
 *
 * @param[in] DataArray Array to checksum
 * @param[in] Length Length of data to checksum
 * @return CRC8 Checksum
 */
uint8_t re_calc_crc8 (const uint8_t * DataArray, const uint16_t Length);

#endif
