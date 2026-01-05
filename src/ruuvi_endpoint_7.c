#include "ruuvi_endpoint_7.h"
#include "ruuvi_endpoints.h"
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#if RE_7_ENABLED

#define RE_7_TEMP_RATIO        (200.0f)
#define RE_7_HUMI_RATIO        (400.0f)
#define RE_7_PRES_RATIO        (1.0f)
#define RE_7_PRES_OFFSET       (-50000.0f)
#define RE_7_TILT_RATIO        (126.0f / 90.0f) /* Map ±90° to ±126, 127 reserved for invalid */
#define RE_7_RAD_TO_DEG        (180.0f / 3.14159265358979323846f)
#define RE_7_COLOR_TEMP_OFFSET (1000U)
#define RE_7_COLOR_TEMP_STEP   (26U)
#define RE_7_BATT_RATIO        (14.0f / 1.8f) /* Map 1.8-3.6V to 0-14, 15 reserved for invalid */
#define RE_7_BATT_OFFSET       (1.8f)

#define RE_7_NIBBLE_HIGH_SHIFT (4U)
#define RE_7_NIBBLE_MASK       (0x0FU)

#define RE_7_RAW_PACKET_ADV_DATA_TYPE_LEN_OFFSET   (0U)
#define RE_7_RAW_PACKET_ADV_DATA_TYPE_LEN_VAL      (2U)
#define RE_7_RAW_PACKET_ADV_DATA_TYPE_FLAG1_OFFSET (1U)
#define RE_7_RAW_PACKET_ADV_DATA_TYPE_FLAG1_VAL    (1U)
#define RE_7_RAW_PACKET_ADV_DATA_TYPE_FLAG2_OFFSET (2U)

#define RE_7_RAW_PACKET_LENGTH_OFFSET             (3U)
#define RE_7_RAW_PACKET_LENGTH_VAL                (23U) /* 22 bytes payload + type byte */
#define RE_7_RAW_PACKET_TYPE_OFFSET               (4U)
#define RE_7_RAW_PACKET_TYPE_VAL                  (0xFFU)
#define RE_7_RAW_PACKET_MANUFACTURER_ID_OFFSET_LO (5U)
#define RE_7_RAW_PACKET_MANUFACTURER_ID_OFFSET_HI (6U)
#define RE_7_RAW_PACKET_MANUFACTURER_ID_VAL       (0x499U)

#define RE_7_CRC_DATA_LEN (16U) /* Bytes 0-15 for CRC calculation, includes header */

static void
re_7_encode_sequence (uint8_t * const buffer, const re_7_data_t * data)
{
    uint8_t seq = RE_7_INVALID_SEQUENCE;

    if (data->sequence_counter <= RE_7_SEQCTR_MAX)
    {
        seq = data->sequence_counter;
    }

    buffer[RE_7_OFFSET_SEQ] = seq;
}

static uint8_t
re_7_decode_sequence (const uint8_t * const buffer)
{
    return buffer[RE_7_OFFSET_SEQ];
}

static void
re_7_encode_flags (uint8_t * const buffer, const re_7_data_t * data)
{
    uint8_t flags = 0;

    if (data->motion_detected)
    {
        flags |= (1U << RE_7_FLAG_MOTION_NOW);
    }

    if (data->presence_detected)
    {
        flags |= (1U << RE_7_FLAG_PRESENCE);
    }

    buffer[RE_7_OFFSET_FLAGS] = flags;
}

static void
re_7_decode_flags (const uint8_t * const buffer, re_7_data_t * const p_data)
{
    const uint8_t flags       = buffer[RE_7_OFFSET_FLAGS];
    p_data->motion_detected   = (flags & (1U << RE_7_FLAG_MOTION_NOW)) != 0;
    p_data->presence_detected = (flags & (1U << RE_7_FLAG_PRESENCE)) != 0;
}

static void
re_7_encode_temperature (uint8_t * const buffer, const re_7_data_t * data)
{
    uint16_t coded_temperature = RE_7_INVALID_TEMPERATURE;
    re_float temperature       = data->temperature_c;

    if (!isnan (temperature))
    {
        re_clip (&temperature, RE_7_TEMP_MIN, RE_7_TEMP_MAX);
        int16_t rounded_temperature = (int16_t) lrintf (temperature * RE_7_TEMP_RATIO);
        coded_temperature           = (uint16_t) rounded_temperature;
    }

    buffer[RE_7_OFFSET_TEMP_MSB] = coded_temperature >> RE_BYTE_1_SHIFT;
    buffer[RE_7_OFFSET_TEMP_LSB] = coded_temperature & RE_BYTE_MASK;
}

static re_float
re_7_decode_temperature (const uint8_t * const buffer)
{
    uint16_t coded_temperature = 0;
    coded_temperature |= buffer[RE_7_OFFSET_TEMP_LSB] & RE_BYTE_MASK;
    coded_temperature |= ( (uint16_t) buffer[RE_7_OFFSET_TEMP_MSB] & RE_BYTE_MASK) <<
                         RE_BYTE_1_SHIFT;

    if (RE_7_INVALID_TEMPERATURE == coded_temperature)
    {
        return NAN;
    }

    return ( (re_float) ( (int16_t) coded_temperature)) / RE_7_TEMP_RATIO;
}

static void
re_7_encode_humidity (uint8_t * const buffer, const re_7_data_t * data)
{
    uint16_t coded_humidity = RE_7_INVALID_HUMIDITY;
    re_float humidity       = data->humidity_rh;

    if (!isnan (humidity))
    {
        re_clip (&humidity, RE_7_HUMI_MIN, RE_7_HUMI_MAX);
        coded_humidity = (uint16_t) lrintf (humidity * RE_7_HUMI_RATIO);
    }

    buffer[RE_7_OFFSET_HUMI_MSB] = coded_humidity >> RE_BYTE_1_SHIFT;
    buffer[RE_7_OFFSET_HUMI_LSB] = coded_humidity & RE_BYTE_MASK;
}

static re_float
re_7_decode_humidity (const uint8_t * const buffer)
{
    uint16_t coded_humidity = 0;
    coded_humidity |= buffer[RE_7_OFFSET_HUMI_LSB] & RE_BYTE_MASK;
    coded_humidity |= ( (uint16_t) buffer[RE_7_OFFSET_HUMI_MSB] & RE_BYTE_MASK) <<
                      RE_BYTE_1_SHIFT;

    if (RE_7_INVALID_HUMIDITY == coded_humidity)
    {
        return NAN;
    }

    return ( (re_float) coded_humidity) / RE_7_HUMI_RATIO;
}

static void
re_7_encode_pressure (uint8_t * const buffer, const re_7_data_t * data)
{
    uint16_t coded_pressure = RE_7_INVALID_PRESSURE;
    re_float pressure       = data->pressure_pa;

    if (!isnan (pressure))
    {
        re_clip (&pressure, RE_7_PRES_MIN, RE_7_PRES_MAX);
        pressure += RE_7_PRES_OFFSET;
        coded_pressure = (uint16_t) lrintf (pressure * RE_7_PRES_RATIO);
    }

    buffer[RE_7_OFFSET_PRES_MSB] = coded_pressure >> RE_BYTE_1_SHIFT;
    buffer[RE_7_OFFSET_PRES_LSB] = coded_pressure & RE_BYTE_MASK;
}

static re_float
re_7_decode_pressure (const uint8_t * const buffer)
{
    uint16_t coded_pressure = 0;
    coded_pressure |= buffer[RE_7_OFFSET_PRES_LSB] & RE_BYTE_MASK;
    coded_pressure |= ( (uint16_t) buffer[RE_7_OFFSET_PRES_MSB] & RE_BYTE_MASK) <<
                      RE_BYTE_1_SHIFT;

    if (RE_7_INVALID_PRESSURE == coded_pressure)
    {
        return NAN;
    }

    return ( ( (re_float) coded_pressure) - RE_7_PRES_OFFSET) / RE_7_PRES_RATIO;
}

static void
re_7_encode_tilt (uint8_t * const buffer, const re_7_data_t * data)
{
    int8_t         coded_tilt_x = RE_7_INVALID_TILT;
    int8_t         coded_tilt_y = RE_7_INVALID_TILT;
    const re_float acc_x        = data->acceleration_x_g;
    const re_float acc_y        = data->acceleration_y_g;
    const re_float acc_z        = data->acceleration_z_g;

    /* Check if any acceleration component is valid */
    if ( (!isnan (acc_x)) && (!isnan (acc_y)) && (!isnan (acc_z)))
    {
        /* Calculate magnitude of acceleration vector */
        const re_float magnitude = sqrtf (acc_x * acc_x + acc_y * acc_y + acc_z * acc_z);

        /* Avoid division by zero */
        if (magnitude > 0.001f)
        {
            /* Normalize and calculate tilt angles using asin */
            /* tilt = asin(acc_component / magnitude) converted to degrees */
            re_float norm_x = acc_x / magnitude;
            re_float norm_y = acc_y / magnitude;

            /* Clamp to valid asin range [-1, 1] to handle floating point errors */
            if (norm_x > 1.0f)
            {
                norm_x = 1.0f;
            }

            if (norm_x < -1.0f)
            {
                norm_x = -1.0f;
            }

            if (norm_y > 1.0f)
            {
                norm_y = 1.0f;
            }

            if (norm_y < -1.0f)
            {
                norm_y = -1.0f;
            }

            re_float tilt_x = asinf (norm_x) * RE_7_RAD_TO_DEG;
            re_float tilt_y = asinf (norm_y) * RE_7_RAD_TO_DEG;
            re_clip (&tilt_x, RE_7_TILT_MIN, RE_7_TILT_MAX);
            re_clip (&tilt_y, RE_7_TILT_MIN, RE_7_TILT_MAX);
            coded_tilt_x = (int8_t) lrintf (tilt_x * RE_7_TILT_RATIO);
            coded_tilt_y = (int8_t) lrintf (tilt_y * RE_7_TILT_RATIO);
        }
    }

    buffer[RE_7_OFFSET_TILT_X] = (uint8_t) coded_tilt_x;
    buffer[RE_7_OFFSET_TILT_Y] = (uint8_t) coded_tilt_y;
}

static void
re_7_decode_tilt (const uint8_t * const buffer, re_float * tilt_x, re_float * tilt_y)
{
    const int8_t coded_tilt_x = (int8_t) buffer[RE_7_OFFSET_TILT_X];
    const int8_t coded_tilt_y = (int8_t) buffer[RE_7_OFFSET_TILT_Y];

    if (RE_7_INVALID_TILT == coded_tilt_x)
    {
        *tilt_x = NAN;
    }
    else
    {
        *tilt_x = ( (re_float) coded_tilt_x) / RE_7_TILT_RATIO;
    }

    if (RE_7_INVALID_TILT == coded_tilt_y)
    {
        *tilt_y = NAN;
    }
    else
    {
        *tilt_y = ( (re_float) coded_tilt_y) / RE_7_TILT_RATIO;
    }
}

static void
re_7_encode_luminosity (uint8_t * const buffer, const re_7_data_t * data)
{
    uint16_t coded_luminosity = RE_7_INVALID_LUMINOSITY;
    re_float luminosity       = data->luminosity_lux;

    if (!isnan (luminosity))
    {
        re_clip (&luminosity, RE_7_LUMI_MIN, RE_7_LUMI_MAX);
        coded_luminosity = (uint16_t) lrintf (luminosity);
    }

    buffer[RE_7_OFFSET_LUMI_MSB] = coded_luminosity >> RE_BYTE_1_SHIFT;
    buffer[RE_7_OFFSET_LUMI_LSB] = coded_luminosity & RE_BYTE_MASK;
}

static re_float
re_7_decode_luminosity (const uint8_t * const buffer)
{
    uint16_t coded_luminosity = 0;
    coded_luminosity |= buffer[RE_7_OFFSET_LUMI_LSB] & RE_BYTE_MASK;
    coded_luminosity |= ( (uint16_t) buffer[RE_7_OFFSET_LUMI_MSB] & RE_BYTE_MASK) <<
                        RE_BYTE_1_SHIFT;

    if (RE_7_INVALID_LUMINOSITY == coded_luminosity)
    {
        return NAN;
    }

    return (re_float) coded_luminosity;
}

static void
re_7_encode_color_temp (uint8_t * const buffer, const re_7_data_t * data)
{
    uint8_t coded_color_temp = RE_7_INVALID_COLOR_TEMP;

    if ( (data->color_temp_k >= RE_7_COLOR_TEMP_MIN)
            && (data->color_temp_k <= RE_7_COLOR_TEMP_MAX))
    {
        coded_color_temp = (uint8_t) ( (data->color_temp_k - RE_7_COLOR_TEMP_OFFSET) /
                                       RE_7_COLOR_TEMP_STEP);
    }

    buffer[RE_7_OFFSET_COLOR_TEMP] = coded_color_temp;
}

static uint16_t
re_7_decode_color_temp (const uint8_t * const buffer)
{
    const uint8_t coded_color_temp = buffer[RE_7_OFFSET_COLOR_TEMP];

    if (RE_7_INVALID_COLOR_TEMP == coded_color_temp)
    {
        return 0; /* Invalid */
    }

    return (uint16_t) (coded_color_temp * RE_7_COLOR_TEMP_STEP + RE_7_COLOR_TEMP_OFFSET);
}

static void
re_7_encode_batt_motion (uint8_t * const buffer, const re_7_data_t * data)
{
    uint8_t  coded_battery = RE_7_INVALID_BATTERY;
    uint8_t  coded_motion  = RE_7_INVALID_MOTION_INT;
    re_float voltage       = data->battery_v;

    if (!isnan (voltage))
    {
        re_clip (&voltage, RE_7_VOLTAGE_MIN, RE_7_VOLTAGE_MAX);
        coded_battery = (uint8_t) lrintf ( (voltage - RE_7_BATT_OFFSET) * RE_7_BATT_RATIO);

        if (coded_battery > RE_7_NIBBLE_MASK)
        {
            coded_battery = RE_7_NIBBLE_MASK;
        }
    }

    if (data->motion_intensity <= RE_7_MOTION_INT_MAX)
    {
        coded_motion = data->motion_intensity;
    }

    buffer[RE_7_OFFSET_BATT_MOTION] = (uint8_t) ( ( (uint8_t) (coded_battery <<
                                      RE_7_NIBBLE_HIGH_SHIFT))
                                      | (coded_motion & RE_7_NIBBLE_MASK));
}

static void
re_7_decode_batt_motion (const uint8_t * const buffer, re_float * battery_v,
                         uint8_t * motion_intensity)
{
    const uint8_t packed        = buffer[RE_7_OFFSET_BATT_MOTION];
    const uint8_t coded_battery = (packed >> RE_7_NIBBLE_HIGH_SHIFT) & RE_7_NIBBLE_MASK;
    const uint8_t coded_motion  = packed & RE_7_NIBBLE_MASK;

    if (RE_7_INVALID_BATTERY == coded_battery)
    {
        *battery_v = NAN;
    }
    else
    {
        *battery_v = RE_7_BATT_OFFSET + ( ( (re_float) coded_battery) / RE_7_BATT_RATIO);
    }

    *motion_intensity = coded_motion;
}

static void
re_7_encode_motion_count (uint8_t * const buffer, const re_7_data_t * data)
{
    uint8_t motion_count = RE_7_INVALID_MOTION_COUNT;

    if (data->motion_count <= RE_7_MOTION_CNT_MAX)
    {
        motion_count = data->motion_count;
    }

    buffer[RE_7_OFFSET_MOTION_CNT] = motion_count;
}

static uint8_t
re_7_decode_motion_count (const uint8_t * const buffer)
{
    return buffer[RE_7_OFFSET_MOTION_CNT];
}

static void
re_7_encode_mac (uint8_t * const buffer, const re_7_data_t * data)
{
    uint64_t mac = data->address;
    /* Store 3 LSB of MAC address (offset 17-19) */
    buffer[RE_7_OFFSET_MAC_0] = (mac >> RE_BYTE_2_SHIFT) & RE_BYTE_MASK;
    buffer[RE_7_OFFSET_MAC_1] = (mac >> RE_BYTE_1_SHIFT) & RE_BYTE_MASK;
    buffer[RE_7_OFFSET_MAC_2] = (mac >> RE_BYTE_0_SHIFT) & RE_BYTE_MASK;
}

static uint64_t
re_7_decode_mac (const uint8_t * const buffer)
{
    uint64_t mac = 0;
    mac |= ( (uint64_t) buffer[RE_7_OFFSET_MAC_0]) << RE_BYTE_2_SHIFT;
    mac |= ( (uint64_t) buffer[RE_7_OFFSET_MAC_1]) << RE_BYTE_1_SHIFT;
    mac |= ( (uint64_t) buffer[RE_7_OFFSET_MAC_2]) << RE_BYTE_0_SHIFT;
    return mac;
}

re_status_t
re_7_encode (uint8_t * const buffer, const re_7_data_t * data)
{
    re_status_t result = RE_SUCCESS;

    if ( (NULL == buffer) || (NULL == data))
    {
        result |= RE_ERROR_NULL;
    }
    else
    {
        buffer[RE_7_OFFSET_HEADER] = RE_7_DESTINATION;
        re_7_encode_sequence (buffer, data);
        re_7_encode_flags (buffer, data);
        re_7_encode_temperature (buffer, data);
        re_7_encode_humidity (buffer, data);
        re_7_encode_pressure (buffer, data);
        re_7_encode_tilt (buffer, data);
        re_7_encode_luminosity (buffer, data);
        re_7_encode_color_temp (buffer, data);
        re_7_encode_batt_motion (buffer, data);
        re_7_encode_motion_count (buffer, data);
        /* Calculate CRC8 over bytes 0-15 */
        buffer[RE_7_OFFSET_CRC] = re_calc_crc8 (buffer, RE_7_CRC_DATA_LEN);
        re_7_encode_mac (buffer, data);
    }

    return result;
}

bool
re_7_check_format (const uint8_t * const p_buffer)
{
    if (NULL == p_buffer)
    {
        return false;
    }

    if (RE_7_RAW_PACKET_ADV_DATA_TYPE_LEN_VAL !=
            p_buffer[RE_7_RAW_PACKET_ADV_DATA_TYPE_LEN_OFFSET])
    {
        return false;
    }

    if (RE_7_RAW_PACKET_ADV_DATA_TYPE_FLAG1_VAL !=
            p_buffer[RE_7_RAW_PACKET_ADV_DATA_TYPE_FLAG1_OFFSET])
    {
        return false;
    }

    if (RE_7_RAW_PACKET_LENGTH_VAL != p_buffer[RE_7_RAW_PACKET_LENGTH_OFFSET])
    {
        return false;
    }

    if (RE_7_RAW_PACKET_TYPE_VAL != p_buffer[RE_7_RAW_PACKET_TYPE_OFFSET])
    {
        return false;
    }

    const uint16_t manufacturer_id = (uint16_t) ( (uint16_t)
                                     p_buffer[RE_7_RAW_PACKET_MANUFACTURER_ID_OFFSET_HI]
                                     << RE_BYTE_1_SHIFT)
                                     + p_buffer[RE_7_RAW_PACKET_MANUFACTURER_ID_OFFSET_LO];

    if (RE_7_RAW_PACKET_MANUFACTURER_ID_VAL != manufacturer_id)
    {
        return false;
    }

    if (RE_7_DESTINATION != p_buffer[RE_7_OFFSET_PAYLOAD + RE_7_OFFSET_HEADER])
    {
        return false;
    }

    return true;
}

re_status_t
re_7_decode (const uint8_t * const p_buffer, re_7_data_t * const p_data)
{
    const uint8_t * const p_payload = &p_buffer[RE_7_OFFSET_PAYLOAD];
    re_status_t          result    = RE_SUCCESS;

    if ( (NULL == p_buffer) || (NULL == p_data))
    {
        return RE_ERROR_NULL;
    }

    memset (p_data, 0, sizeof (*p_data));

    if (RE_7_DESTINATION != p_payload[RE_7_OFFSET_HEADER])
    {
        return RE_ERROR_INVALID_PARAM;
    }

    /* Verify CRC */
    const uint8_t calculated_crc = re_calc_crc8 (p_payload, RE_7_CRC_DATA_LEN);

    if (calculated_crc != p_payload[RE_7_OFFSET_CRC])
    {
        return RE_ERROR_DECODING_CRC;
    }

    p_data->sequence_counter = re_7_decode_sequence (p_payload);
    re_7_decode_flags (p_payload, p_data);
    p_data->temperature_c = re_7_decode_temperature (p_payload);
    p_data->humidity_rh   = re_7_decode_humidity (p_payload);
    p_data->pressure_pa   = re_7_decode_pressure (p_payload);
    re_7_decode_tilt (p_payload, &p_data->tilt_x_deg, &p_data->tilt_y_deg);
    p_data->luminosity_lux = re_7_decode_luminosity (p_payload);
    p_data->color_temp_k   = re_7_decode_color_temp (p_payload);
    re_7_decode_batt_motion (p_payload, &p_data->battery_v, &p_data->motion_intensity);
    p_data->motion_count = re_7_decode_motion_count (p_payload);
    p_data->address      = re_7_decode_mac (p_payload);
    return result;
}

#endif
