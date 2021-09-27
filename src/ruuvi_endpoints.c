#include "ruuvi_endpoints.h"
#include <stdlib.h>
#include <math.h>

// CRC8 calculation https://oshgarage.com/the-crc8-checksum/

static const uint8_t CRC_8_TABLE[256] = 
{
      0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
    157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
     35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
    190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
     70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
    219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
    101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
    248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
    140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
     17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
    175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
     50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
    202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
     87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
    233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
    116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53
};//!< CRC 8 lookup table - OSH Garage

/**
 * @brief Calculate CRC8 checksum of a data array
 * 
 * @param[in] DataArray Array to checksum
 * @param[in] Length Length of data to checksum
 * @return CRC8 Checksum
 */
uint8_t re_calc_crc8(const uint8_t *DataArray, const uint16_t Length)
{
    uint16_t i;
    uint8_t CRC;

    CRC = 0;
    for (i=0; i<Length; i++)
        CRC = CRC_8_TABLE[CRC ^ DataArray[i]];

    return CRC;
}


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