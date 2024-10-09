#include "ruuvi_endpoint_f0.h"
#include "ruuvi_endpoints.h"
#include <stddef.h>
#include <string.h>
#include <math.h>

#define RE_F0_ENABLED 1

#if RE_F0_ENABLED

#define RE_F0_MAC_MAX (0xFFFFFFFFFFFFU)
#define RE_F0_MAC_MIN (0)

#define RE_F0_BYTE_0_SHIFT (0U)
#define RE_F0_BYTE_1_SHIFT (8U)
#define RE_F0_BYTE_2_SHIFT (16U)
#define RE_F0_BYTE_3_SHIFT (24U)
#define RE_F0_BYTE_4_SHIFT (32U)
#define RE_F0_BYTE_5_SHIFT (40U)
#define RE_F0_BYTE_MASK    (0xFFU)

#define RE_F0_RAW_PACKET_ADV_DATA_FLAGS_LEN_OFFSET       (0U)
#define RE_F0_RAW_PACKET_ADV_DATA_FLAGS_LEN_VAL          (2U)
#define RE_F0_RAW_PACKET_ADV_DATA_FLAGS_DATA_TYPE_OFFSET (1U)
#define RE_F0_RAW_PACKET_ADV_DATA_FLAGS_DATA_TYPE_VAL    (1U) /* BT_DATA_FLAGS - AD flags */

#define RE_F0_RAW_PACKET_ADV_DATA_UUID16_LEN_OFFSET       (3U)
#define RE_F0_RAW_PACKET_ADV_DATA_UUID16_LEN_VAL          (3U)
#define RE_F0_RAW_PACKET_ADV_DATA_UUID16_DATA_TYPE_OFFSET (4U)
#define RE_F0_RAW_PACKET_ADV_DATA_UUID16_DATA_TYPE_VAL    (3U) /* BT_DATA_UUID16_ALL - 16-bit UUID, all listed */
#define RE_F0_RAW_PACKET_ADV_DATA_UUID16_BYTE1_OFFSET     (5U)
#define RE_F0_RAW_PACKET_ADV_DATA_UUID16_BYTE2_OFFSET     (6U)
#define RE_F0_RAW_PACKET_ADV_DATA_UUID16_VAL              (0xFC98U)

#define RE_F0_RAW_PACKET_MANUFACTURER_DATA_LENGTH_OFFSET (7U)
#define RE_F0_RAW_PACKET_MANUFACTURER_DATA_LENGTH_VAL    (23U)
#define RE_F0_RAW_PACKET_MANUFACTURER_DATA_TYPE_OFFSET   (8U)
#define RE_F0_RAW_PACKET_MANUFACTURER_DATA_TYPE_VAL      (0xFFU) /* BT_DATA_MANUFACTURER_DATA */
#define RE_F0_RAW_PACKET_MANUFACTURER_ID_OFFSET_LO       (9U)
#define RE_F0_RAW_PACKET_MANUFACTURER_ID_OFFSET_HI       (10U)
#define RE_F0_RAW_PACKET_MANUFACTURER_ID_VAL             (0x499U)

// Avoid mocking simple function
#ifdef TEST
void
re_clip (re_float * const value, const re_float min, const re_float max)
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
#endif

static void
re_f0_encode_temperature (uint8_t * const p_slot, re_float val)
{
    uint8_t coded_val = RE_F0_INVALID_TEMPERATURE;

    if (!isnan (val))
    {
        re_clip (&val, RE_F0_TEMPERATURE_MIN, RE_F0_TEMPERATURE_MAX);
        const int8_t signed_coded_val = (int8_t) lrintf (val * RE_F0_TEMPERATURE_SCALE_FACTOR);
        coded_val = (uint8_t) signed_coded_val;
    }

    *p_slot = coded_val;
}

static re_float
re_f0_decode_temperature (const uint8_t * const p_slot)
{
    uint8_t coded_val = * (uint8_t *) p_slot;

    if (RE_F0_INVALID_TEMPERATURE == coded_val)
    {
        return NAN;
    }

    const int8_t signed_coded_val = (int8_t) coded_val;
    return (re_float) signed_coded_val / RE_F0_TEMPERATURE_SCALE_FACTOR;
}

static void
re_f0_encode_humidity (uint8_t * const p_slot, re_float val)
{
    uint8_t coded_val = RE_F0_INVALID_HUMIDITY;

    if (!isnan (val))
    {
        re_clip (&val, RE_F0_HUMIDITY_MIN, RE_F0_HUMIDITY_MAX);
        coded_val = (uint8_t) lrintf (val * RE_F0_HUMIDITY_SCALE_FACTOR);
    }

    *p_slot = coded_val;
}

static re_float
re_f0_decode_humidity (const uint8_t * const p_slot)
{
    const uint8_t coded_val = *p_slot;

    if (RE_F0_INVALID_HUMIDITY == coded_val)
    {
        return NAN;
    }

    return (re_float) coded_val / RE_F0_HUMIDITY_SCALE_FACTOR;
}

static void
re_f0_encode_pressure (uint8_t * const p_slot, re_float val)
{
    uint8_t coded_val = RE_F0_INVALID_PRESSURE;

    if (!isnan (val))
    {
        re_clip (&val, RE_F0_PRESSURE_MIN, RE_F0_PRESSURE_MAX);
        coded_val = (uint8_t) lrintf ( ( (val - RE_F0_PRESSURE_MIN) /
                                         RE_F0_PRESSURE_SCALE_FACTOR));
    }

    *p_slot = coded_val;
}

static re_float
re_f0_decode_pressure (const uint8_t * const p_slot)
{
    const uint8_t coded_val = *p_slot;

    if (RE_F0_INVALID_PRESSURE == coded_val)
    {
        return NAN;
    }

    return ( (re_float) coded_val * RE_F0_PRESSURE_SCALE_FACTOR) + RE_F0_PRESSURE_MIN;
}

static void
re_f0_encode_pm (uint8_t * const p_slot, re_float val)
{
    uint8_t coded_val = RE_F0_INVALID_PM;

    if (!isnan (val))
    {
        re_clip (&val, RE_F0_PM_MIN, RE_F0_PM_MAX);
        coded_val = (uint8_t) (int) lrintf (logf (val + 1) * RE_F0_PM_SCALE_FACTOR);
    }

    *p_slot = coded_val;
}

static re_float
re_f0_decode_pm (const uint8_t * const p_slot)
{
    const uint8_t coded_val = *p_slot;

    if (RE_F0_INVALID_PM == coded_val)
    {
        return NAN;
    }

    return (re_float) (expf ( ( (re_float) coded_val / RE_F0_PM_SCALE_FACTOR)) - 1);
}

static void
re_f0_encode_co2 (uint8_t * const p_slot, re_float val)
{
    uint8_t coded_val = RE_F0_INVALID_CO2;

    if (!isnan (val))
    {
        re_clip (&val, RE_F0_CO2_MIN, RE_F0_CO2_MAX);
        coded_val = (uint8_t) (int) lrintf (logf (val + 1) * RE_F0_CO2_SCALE_FACTOR);
    }

    *p_slot = coded_val;
}

static re_float
re_f0_decode_co2 (const uint8_t * const p_slot)
{
    const uint8_t coded_val = *p_slot;

    if (RE_F0_INVALID_CO2 == coded_val)
    {
        return NAN;
    }

    return (re_float) (expf ( ( (re_float) coded_val / RE_F0_CO2_SCALE_FACTOR)) - 1);
}

static void
re_f0_encode_voc (uint8_t * const p_slot, re_float val)
{
    uint8_t coded_val = RE_F0_INVALID_VOC_INDEX;

    if ( (!isnan (val)) && (val >= RE_F0_VOC_INDEX_MIN) && (val <= RE_F0_VOC_INDEX_MAX))
    {
        coded_val = (uint8_t) (int) lrintf (logf (val) * RE_F0_VOC_SCALE_FACTOR);
    }

    *p_slot = coded_val;
}

static re_float
re_f0_decode_voc (const uint8_t * const p_slot)
{
    uint8_t coded_val = *p_slot;

    if (RE_F0_INVALID_VOC_INDEX == coded_val)
    {
        return NAN;
    }

    return (re_float) (expf ( ( (re_float) coded_val / RE_F0_VOC_SCALE_FACTOR)));
}

static void
re_f0_encode_nox (uint8_t * const p_slot, re_float val)
{
    uint8_t coded_val = RE_F0_INVALID_NOX_INDEX;

    if ( (!isnan (val)) && (val >= RE_F0_NOX_INDEX_MIN) && (val <= RE_F0_NOX_INDEX_MAX))
    {
        coded_val = (uint8_t) (int) lrintf (logf (val) * RE_F0_NOX_SCALE_FACTOR);
    }

    *p_slot = coded_val;
}

static re_float
re_f0_decode_nox (const uint8_t * const p_slot)
{
    uint8_t coded_val = *p_slot;

    if (RE_F0_INVALID_NOX_INDEX == coded_val)
    {
        return NAN;
    }

    return (re_float) (expf ( ( (re_float) coded_val / RE_F0_NOX_SCALE_FACTOR)));
}

static void
re_f0_encode_luminosity (uint8_t * const p_slot, re_float val)
{
    uint8_t coded_val = RE_F0_INVALID_LUMINOSITY;

    if (!isnan (val))
    {
        re_clip (&val, RE_F0_LUMINOSITY_MIN, RE_F0_LUMINOSITY_MAX);
        coded_val = (uint8_t) (int) lrintf (logf (val + 1) * RE_F0_LUMINOSITY_SCALE_FACTOR);
    }

    *p_slot = coded_val;
}

static re_float
re_f0_decode_luminosity (const uint8_t * const p_slot)
{
    uint8_t coded_val = *p_slot;

    if (RE_F0_INVALID_LUMINOSITY == coded_val)
    {
        return NAN;
    }

    return (re_float) (expf ( ( (re_float) coded_val / RE_F0_LUMINOSITY_SCALE_FACTOR)) - 1);
}

static void
re_f0_encode_sound (uint8_t * const p_slot, re_float val)
{
    uint8_t coded_val = RE_F0_INVALID_SOUND;

    if (!isnan (val))
    {
        re_clip (&val, RE_F0_SOUND_MIN, RE_F0_SOUND_MAX);
        coded_val = (uint8_t) (int) lrintf (val * RE_F0_SOUND_SCALE_FACTOR);
    }

    *p_slot = coded_val;
}

static re_float
re_f0_decode_sound (const uint8_t * const p_slot)
{
    uint8_t coded_val = *p_slot;

    if (RE_F0_INVALID_SOUND == coded_val)
    {
        return NAN;
    }

    return (re_float) ( (re_float) coded_val / RE_F0_SOUND_SCALE_FACTOR);
}

static void
re_f0_encode_flags (uint8_t * const p_slot, const re_f0_data_t * p_data)
{
    uint8_t flags = ( (p_data->flag_seq_cnt & RE_F0_FLAGS_SEQ_MASK) <<
                      RE_F0_FLAGS_SEQ_OFFSET);
    flags |= (p_data->flag_usb_on ? RE_F0_FLAGS_USB_ON : 0U);
    flags |= (p_data->flag_low_battery ? RE_F0_FLAGS_LOW_BATTERY : 0U);
    flags |= (p_data->flag_calibration_in_progress ? RE_F0_FLAGS_CALIBRATION_IN_PROGRESS :
              0U);
    flags |= (p_data->flag_boost_mode ? RE_F0_FLAGS_BOOST_MODE : 0U);
    *p_slot = flags;
}

static void
re_f0_decode_flags (const uint8_t * const p_slot, re_f0_data_t * const p_data)
{
    const uint8_t flags = *p_slot;
    p_data->flag_seq_cnt = (flags >> RE_F0_FLAGS_SEQ_OFFSET) & RE_F0_FLAGS_SEQ_MASK;
    p_data->flag_usb_on = (flags & RE_F0_FLAGS_USB_ON) ? true : false;
    p_data->flag_low_battery = (flags & RE_F0_FLAGS_LOW_BATTERY) ? true : false;
    p_data->flag_calibration_in_progress = (flags & RE_F0_FLAGS_CALIBRATION_IN_PROGRESS) ?
                                           true : false;
    p_data->flag_boost_mode = (flags & RE_F0_FLAGS_BOOST_MODE) ? true : false;
}

static void
re_f0_encode_set_address (uint8_t * const p_buffer, const re_f0_data_t * p_data)
{
    // Address is 64 bits, skip 2 first bytes
    uint8_t  addr_offset = RE_F0_OFFSET_ADDR_MSB;
    uint64_t mac         = p_data->address;

    if (RE_F0_MAC_MAX < p_data->address)
    {
        mac = RE_F0_INVALID_MAC;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"

    if (RE_F0_MAC_MIN > p_data->address)
    {
        mac = RE_F0_INVALID_MAC;
    }

#pragma GCC diagnostic pop
    p_buffer[addr_offset] = (mac >> RE_F0_BYTE_5_SHIFT) & RE_F0_BYTE_MASK;
    addr_offset++;
    p_buffer[addr_offset] = (mac >> RE_F0_BYTE_4_SHIFT) & RE_F0_BYTE_MASK;
    addr_offset++;
    p_buffer[addr_offset] = (mac >> RE_F0_BYTE_3_SHIFT) & RE_F0_BYTE_MASK;
    addr_offset++;
    p_buffer[addr_offset] = (mac >> RE_F0_BYTE_2_SHIFT) & RE_F0_BYTE_MASK;
    addr_offset++;
    p_buffer[addr_offset] = (mac >> RE_F0_BYTE_1_SHIFT) & RE_F0_BYTE_MASK;
    addr_offset++;
    p_buffer[addr_offset] = (mac >> RE_F0_BYTE_0_SHIFT) & RE_F0_BYTE_MASK;
}

static uint64_t
re_f0_decode_address (const uint8_t * const p_buffer)
{
    // Address is 64 bits, skip 2 first bytes
    uint8_t  addr_offset = 0;
    uint64_t mac         = 0;
    mac |= p_buffer[addr_offset];
    mac <<= RE_F0_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    mac <<= RE_F0_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    mac <<= RE_F0_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    mac <<= RE_F0_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    mac <<= RE_F0_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    return mac;
}

re_status_t
re_f0_encode (uint8_t * const p_buffer, const re_f0_data_t * const p_data)
{
    re_status_t result = RE_SUCCESS;

    if ( (NULL == p_buffer) || (NULL == p_data))
    {
        result |= RE_ERROR_NULL;
    }
    else
    {
        memset (p_buffer, 0, RE_F0_DATA_LENGTH);
        p_buffer[RE_F0_OFFSET_HEADER] = RE_F0_DESTINATION;
        re_f0_encode_temperature (&p_buffer[RE_F0_OFFSET_TEMPERATURE], p_data->temperature_c);
        re_f0_encode_humidity (&p_buffer[RE_F0_OFFSET_HUMIDITY], p_data->humidity_rh);
        re_f0_encode_pressure (&p_buffer[RE_F0_OFFSET_PRESSURE], p_data->pressure_pa);
        re_f0_encode_pm (&p_buffer[RE_F0_OFFSET_PM_1_0], p_data->pm1p0_ppm);
        re_f0_encode_pm (&p_buffer[RE_F0_OFFSET_PM_2_5], p_data->pm2p5_ppm);
        re_f0_encode_pm (&p_buffer[RE_F0_OFFSET_PM_4_0], p_data->pm4p0_ppm);
        re_f0_encode_pm (&p_buffer[RE_F0_OFFSET_PM_10_0], p_data->pm10p0_ppm);
        re_f0_encode_co2 (&p_buffer[RE_F0_OFFSET_CO2], p_data->co2);
        re_f0_encode_voc (&p_buffer[RE_F0_OFFSET_VOC_INDEX], p_data->voc_index);
        re_f0_encode_nox (&p_buffer[RE_F0_OFFSET_NOX_INDEX], p_data->nox_index);
        re_f0_encode_luminosity (&p_buffer[RE_F0_OFFSET_LUMINOSITY], p_data->luminosity);
        re_f0_encode_sound (&p_buffer[RE_F0_OFFSET_SOUND], p_data->sound_dba_avg);
        re_f0_encode_flags (&p_buffer[RE_F0_OFFSET_FLAGS], p_data);
        re_f0_encode_set_address (p_buffer, p_data);
    }

    return result;
}

bool re_f0_check_format (const uint8_t * const p_buffer)
{
    if (NULL == p_buffer)
    {
        return false;
    }

    if (RE_F0_RAW_PACKET_ADV_DATA_FLAGS_LEN_VAL !=
            p_buffer[RE_F0_RAW_PACKET_ADV_DATA_FLAGS_LEN_OFFSET])
    {
        return false;
    }

    if (RE_F0_RAW_PACKET_ADV_DATA_FLAGS_DATA_TYPE_VAL !=
            p_buffer[RE_F0_RAW_PACKET_ADV_DATA_FLAGS_DATA_TYPE_OFFSET])
    {
        return false;
    }

    if (RE_F0_RAW_PACKET_ADV_DATA_UUID16_LEN_VAL !=
            p_buffer[RE_F0_RAW_PACKET_ADV_DATA_UUID16_LEN_OFFSET])
    {
        return false;
    }

    if (RE_F0_RAW_PACKET_ADV_DATA_UUID16_DATA_TYPE_VAL !=
            p_buffer[RE_F0_RAW_PACKET_ADV_DATA_UUID16_DATA_TYPE_OFFSET])
    {
        return false;
    }

    const uint16_t service_uuid = (uint16_t) ( (uint16_t)
                                  p_buffer[RE_F0_RAW_PACKET_ADV_DATA_UUID16_BYTE2_OFFSET] << RE_F0_BYTE_1_SHIFT)
                                  + p_buffer[RE_F0_RAW_PACKET_ADV_DATA_UUID16_BYTE1_OFFSET];

    if (RE_F0_RAW_PACKET_ADV_DATA_UUID16_VAL != service_uuid)
    {
        return false;
    }

    if (RE_F0_RAW_PACKET_MANUFACTURER_DATA_LENGTH_VAL !=
            p_buffer[RE_F0_RAW_PACKET_MANUFACTURER_DATA_LENGTH_OFFSET])
    {
        return false;
    }

    if (RE_F0_RAW_PACKET_MANUFACTURER_DATA_TYPE_VAL !=
            p_buffer[RE_F0_RAW_PACKET_MANUFACTURER_DATA_TYPE_OFFSET])
    {
        return false;
    }

    const uint16_t manufacturer_id = (uint16_t) ( (uint16_t)
                                     p_buffer[RE_F0_RAW_PACKET_MANUFACTURER_ID_OFFSET_HI] << RE_F0_BYTE_1_SHIFT)
                                     + p_buffer[RE_F0_RAW_PACKET_MANUFACTURER_ID_OFFSET_LO];

    if (RE_F0_RAW_PACKET_MANUFACTURER_ID_VAL != manufacturer_id)
    {
        return false;
    }

    if (RE_F0_DESTINATION != p_buffer[RE_F0_OFFSET_PAYLOAD + RE_F0_OFFSET_HEADER])
    {
        return false;
    }

    return true;
}

re_status_t re_f0_decode (const uint8_t * const p_buffer, re_f0_data_t * const p_data)
{
    const uint8_t * const p_payload = &p_buffer[RE_F0_OFFSET_PAYLOAD];
    re_status_t result = RE_SUCCESS;

    if ( (NULL == p_payload) || (NULL == p_data))
    {
        return RE_ERROR_NULL;
    }

    memset (p_data, 0, sizeof (*p_data));

    if (RE_F0_DESTINATION != p_payload[RE_F0_OFFSET_HEADER])
    {
        return RE_ERROR_INVALID_PARAM;
    }

    p_data->temperature_c = re_f0_decode_temperature (&p_payload[RE_F0_OFFSET_TEMPERATURE]);
    p_data->humidity_rh = re_f0_decode_humidity (&p_payload[RE_F0_OFFSET_HUMIDITY]);
    p_data->pressure_pa = re_f0_decode_pressure (&p_payload[RE_F0_OFFSET_PRESSURE]);
    p_data->pm1p0_ppm = re_f0_decode_pm (&p_payload[RE_F0_OFFSET_PM_1_0]);
    p_data->pm2p5_ppm = re_f0_decode_pm (&p_payload[RE_F0_OFFSET_PM_2_5]);
    p_data->pm4p0_ppm = re_f0_decode_pm (&p_payload[RE_F0_OFFSET_PM_4_0]);
    p_data->pm10p0_ppm = re_f0_decode_pm (&p_payload[RE_F0_OFFSET_PM_10_0]);
    p_data->co2 = re_f0_decode_co2 (&p_payload[RE_F0_OFFSET_CO2]);
    p_data->voc_index = re_f0_decode_voc (&p_payload[RE_F0_OFFSET_VOC_INDEX]);
    p_data->nox_index = re_f0_decode_nox (&p_payload[RE_F0_OFFSET_NOX_INDEX]);
    p_data->luminosity = re_f0_decode_luminosity (&p_payload[RE_F0_OFFSET_LUMINOSITY]);
    p_data->sound_dba_avg = re_f0_decode_sound (&p_payload[RE_F0_OFFSET_SOUND]);
    re_f0_decode_flags (&p_payload[RE_F0_OFFSET_FLAGS], p_data);
    p_data->address = re_f0_decode_address (&p_payload[RE_F0_OFFSET_ADDR_MSB]);
    return result;
}

re_f0_data_t
re_f0_data_invalid (const uint8_t measurement_cnt, const uint64_t radio_mac)
{
    const re_f0_data_t data =
    {
        .temperature_c = NAN,
        .humidity_rh   = NAN,
        .pressure_pa   = NAN,
        .pm1p0_ppm     = NAN,
        .pm2p5_ppm     = NAN,
        .pm4p0_ppm     = NAN,
        .pm10p0_ppm    = NAN,
        .co2           = NAN,
        .voc_index     = NAN,
        .nox_index     = NAN,
        .luminosity    = NAN,
        .sound_dba_avg = NAN,
        .flag_seq_cnt  = measurement_cnt & RE_F0_FLAGS_SEQ_MASK,
        .flag_usb_on   = false,
        .flag_low_battery = false,
        .flag_calibration_in_progress = false,
        .flag_boost_mode = false,
        .address       = radio_mac,
    };
    return data;
}

#endif
