#include "ruuvi_endpoints.h"
#include <stdlib.h>
#include <math.h>

#if RE_8_ENABLED
// CRC8 calculation https://oshgarage.com/the-crc8-checksum/

static const uint8_t CRC_8_TABLE[256] =
{
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
    0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
    0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
    0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
    0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
    0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
    0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
    0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
    0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
    0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
    0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
    0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
    0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
    0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
    0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
    0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
    0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};//!< CRC 8 lookup table - https://chromium.googlesource.com/chromium/src/+/HEAD/rlz/lib/crc8.cc

uint8_t re_calc_crc8 (const uint8_t * DataArray, const uint16_t Length)
{
    uint16_t i;
    uint8_t CRC;
    CRC = 0;

    for (i = 0; i < Length; i++)
    { CRC = CRC_8_TABLE[CRC ^ DataArray[i]]; }

    return CRC;
}

#endif

/**
 * @brief Get current time for log read command to compensate timestamps.
 *
 * @param[in] raw_message 11-byte payload from central.
 * @return u32 timestamp, 0 if raw_message was NULL.
 */
uint32_t re_std_log_current_time (const uint8_t * const raw_message)
{
    uint32_t time = 0;

    if (NULL != raw_message)
    {
        time += (raw_message[RE_LOG_READ_CURRENT_MSB_IDX] << 24U)
                + (raw_message[RE_LOG_READ_CURRENT_B2_IDX] << 16U)
                + (raw_message[RE_LOG_READ_CURRENT_B3_IDX] << 8U)
                + (raw_message[RE_LOG_READ_CURRENT_LSB_IDX]);
    }

    return time;
}

/**
 * @brief Get start time for log read command.
 *
 * @param[in] raw_message 11-byte payload from central.
 * @return u32 timestamp, 0 if raw_message was NULL.
 */
uint32_t re_std_log_start_time (const uint8_t * const raw_message)
{
    uint32_t time = 0;

    if (NULL != raw_message)
    {
        time += (raw_message[RE_LOG_READ_START_MSB_IDX] << 24U)
                + (raw_message[RE_LOG_READ_START_B2_IDX] << 16U)
                + (raw_message[RE_LOG_READ_START_B3_IDX] << 8U)
                + (raw_message[RE_LOG_READ_START_LSB_IDX]);
    }

    return time;
}

re_status_t re_log_write_header (uint8_t * const buffer, const uint8_t source)
{
    re_status_t err_code = RE_SUCCESS;

    if (NULL != buffer)
    {
        buffer[RE_STANDARD_SOURCE_INDEX] = source;
        buffer[RE_STANDARD_OPERATION_INDEX] = RE_STANDARD_LOG_VALUE_WRITE;
    }
    else
    {
        err_code |= RE_ERROR_NULL;
    }

    return err_code;
}

/**
 * @brief write a log element timestamp to the given buffer.
 *
 * @param[out] 11-byte buffer to write data to.
 * @param[in]  timestamp_ms Timestamp as it will be sent to remote.
 * @retval RE_SUCCESS encoding was successful.
 * @retval RE_ERROR_NULL buffer was NULL
 * @retval RE_ERROR_INVALID_PARAM timestamp cannot be encoded as 32-bit second value.
 */
re_status_t re_log_write_timestamp (uint8_t * const buffer, const uint64_t timestamp_ms)
{
    re_status_t err_code = RE_SUCCESS;
    const uint64_t timestamp_s = timestamp_ms / 1000U;

    if (NULL == buffer)
    {
        err_code |= RE_ERROR_NULL;
    }
    else if (timestamp_s > ( (uint32_t) 0xFFFFFFFFU))
    {
        err_code |= RE_ERROR_INVALID_PARAM;
    }
    else
    {
        buffer[RE_LOG_WRITE_TS_MSB_IDX] = (uint8_t) ( (timestamp_s >> 24) & 0xFFU);
        buffer[RE_LOG_WRITE_TS_B2_IDX] = (uint8_t) ( (timestamp_s >> 16) & 0xFFU);
        buffer[RE_LOG_WRITE_TS_B3_IDX] = (uint8_t) ( (timestamp_s >> 8) & 0xFFU);
        buffer[RE_LOG_WRITE_TS_LSB_IDX] = (uint8_t) (timestamp_s & 0xFFU);
    }

    return err_code;
}

static int32_t f2i (re_float value)
{
    int32_t rvalue = 0x80000000;
    value = (value) >= 0 ? (value) + 0.5 : (value) - 0.5;

    if (value >= INT32_MAX)
    {
        rvalue = INT32_MAX;
    }
    else if (value <= (INT32_MIN + 1))
    {
        rvalue = INT32_MIN + 1;
    }
    else
    {
        rvalue = (int32_t) value;
    }

    return rvalue;
}

/**
 * @brief Encode given float to the given buffer.
 *
 * @param[out] buffer 11-byte buffer to which data is encoded.
 * @param[in]  data Float value to encode.
 * @param[in]  source Ruuvi Endpoint data source, e.g. RE_STANDARD_DESTINATION_ACCELERATION_X.
 *
 * @retval RE_SUCCESS Data was encoded successfully.
 * @retval RE_ERROR_NULL Buffer was NULL.
 * @retval RE_ERROR_INVALID_PARAM if data is NAN or inf.
 * @retval RE_NOT_IMPLEMENTED if there's no encoding for given data source.
 *
 * @warning if data is outside representable range resulting encoding is undefined.
 */
re_status_t re_log_write_data (uint8_t * const buffer, const re_float data,
                               const uint8_t source)
{
    re_status_t err_code = RE_SUCCESS;
    int32_t discrete_value = 0xFFFFFFFF;
    re_float scaled_value = 0;

    if (NULL == buffer)
    {
        err_code |= RE_ERROR_NULL;
    }
    else if (isnan (data) || isinf (data))
    {
        err_code |= RE_ERROR_INVALID_PARAM;
    }
    else
    {
        switch (source)
        {
            case RE_STANDARD_DESTINATION_ACCELERATION_X:
            case RE_STANDARD_DESTINATION_ACCELERATION_Y:
            case RE_STANDARD_DESTINATION_ACCELERATION_Z:
                scaled_value = (data * RE_STANDARD_ACCELERATION_SF);
                break;

            case RE_STANDARD_DESTINATION_GYRATION_X:
            case RE_STANDARD_DESTINATION_GYRATION_Y:
            case RE_STANDARD_DESTINATION_GYRATION_Z:
                scaled_value = (data * RE_STANDARD_GYRATION_SF);
                break;

            case RE_STANDARD_DESTINATION_HUMIDITY:
                scaled_value = (data * RE_STANDARD_HUMIDITY_SF);
                break;

            case RE_STANDARD_DESTINATION_PRESSURE:
                scaled_value = (data * RE_STANDARD_PRESSURE_SF);
                break;

            case RE_STANDARD_DESTINATION_TEMPERATURE:
                scaled_value = (data * RE_STANDARD_TEMPERATURE_SF);
                break;

            default:
                err_code |= RE_ERROR_NOT_IMPLEMENTED;
                break;
        }

        scaled_value = roundf (scaled_value);
        discrete_value = f2i (scaled_value);
    }

    // These shifts do not rely on the value of leftmost bit if original
    // value is negative, so this is safe way to encode bytes.
    buffer[RE_LOG_WRITE_VALUE_MSB_IDX] = (uint8_t) ( (discrete_value >> 24U) & 0xFFU);
    buffer[RE_LOG_WRITE_VALUE_B2_IDX] = (uint8_t) ( (discrete_value >> 16U) & 0xFFU);
    buffer[RE_LOG_WRITE_VALUE_B3_IDX] = (uint8_t) ( (discrete_value >> 8U) & 0xFFU);
    buffer[RE_LOG_WRITE_VALUE_LSB_IDX] = (uint8_t) (discrete_value & 0xFFU);
    return err_code;
}

void re_clip (float * const value, const float min, const float max)
{
    if (*value > max)
    {
        *value = max;
    }

    if (*value < min)
    {
        *value = min;
    }
}