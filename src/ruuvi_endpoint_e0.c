#include "ruuvi_endpoint_e0.h"
#include "ruuvi_endpoints.h"
#include <stddef.h>
#include <string.h>
#include <math.h>

#if RE_E0_ENABLED

#define RE_E0_MAC_MAX (0xFFFFFFFFFFFFU)
#define RE_E0_MAC_MIN (0)

#define RE_E0_BYTE_0_SHIFT (0U)
#define RE_E0_BYTE_1_SHIFT (8U)
#define RE_E0_BYTE_2_SHIFT (16U)
#define RE_E0_BYTE_3_SHIFT (24U)
#define RE_E0_BYTE_4_SHIFT (32U)
#define RE_E0_BYTE_5_SHIFT (40U)
#define RE_E0_BYTE_MASK    (0xFFU)

#define RE_E0_RAW_PACKET_LENGTH_OFFSET             (0U)
#define RE_E0_RAW_PACKET_LENGTH_VAL                (43U)
#define RE_E0_RAW_PACKET_TYPE_OFFSET               (1U)
#define RE_E0_RAW_PACKET_TYPE_VAL                  (0xFFU)
#define RE_E0_RAW_PACKET_MANUFACTURER_ID_OFFSET_LO (2U)
#define RE_E0_RAW_PACKET_MANUFACTURER_ID_OFFSET_HI (3U)
#define RE_E0_RAW_PACKET_MANUFACTURER_ID_VAL       (0x499U)

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
re_e0_encode_temperature (uint8_t * const p_slot, re_float val)
{
    int16_t val_i16 = RE_E0_INVALID_TEMPERATURE;

    if (!isnan (val))
    {
        re_clip (&val, RE_E0_TEMPERATURE_MIN, RE_E0_TEMPERATURE_MAX);
        val_i16 = (int16_t) lrintf (val * RE_E0_TEMPERATURE_RATIO);
    }

    const uint16_t coded_val = (uint16_t) val_i16;
    p_slot[0] |= ( (uint32_t) coded_val >> RE_E0_BYTE_1_SHIFT) & RE_E0_BYTE_MASK;
    p_slot[1] |= (coded_val & RE_E0_BYTE_MASK);
}

static re_float
re_e0_decode_temperature (const uint8_t * const p_slot)
{
    uint16_t coded_val = 0;
    coded_val |= p_slot[1] & RE_E0_BYTE_MASK;
    coded_val |= ( (uint16_t) p_slot[0] & RE_E0_BYTE_MASK) << RE_E0_BYTE_1_SHIFT;

    if (RE_E0_INVALID_TEMPERATURE == coded_val)
    {
        return NAN;
    }

    const int16_t val_i16 = (int16_t) coded_val;
    return (re_float) val_i16 / RE_E0_TEMPERATURE_RATIO;
}

static void
re_e0_encode_humidity (uint8_t * const p_slot, re_float val)
{
    uint16_t coded_val = RE_E0_INVALID_HUMIDITY;

    if (!isnan (val))
    {
        re_clip (&val, RE_E0_HUMIDITY_MIN, RE_E0_HUMIDITY_MAX);
        coded_val = (uint16_t) lrintf (val * RE_E0_HUMIDITY_RATIO);
    }

    p_slot[0] |= ( (uint32_t) coded_val >> RE_E0_BYTE_1_SHIFT) & RE_E0_BYTE_MASK;
    p_slot[1] |= (coded_val & RE_E0_BYTE_MASK);
}

static re_float
re_e0_decode_humidity (const uint8_t * const p_slot)
{
    uint16_t coded_val = 0;
    coded_val |= p_slot[1] & RE_E0_BYTE_MASK;
    coded_val |= ( (uint16_t) p_slot[0] & RE_E0_BYTE_MASK) << RE_E0_BYTE_1_SHIFT;

    if (RE_E0_INVALID_HUMIDITY == coded_val)
    {
        return NAN;
    }

    return (re_float) coded_val / RE_E0_HUMIDITY_RATIO;
}

static void
re_e0_encode_pressure (uint8_t * const p_slot, re_float val)
{
    uint16_t coded_val = RE_E0_INVALID_PRESSURE;

    if (!isnan (val))
    {
        re_clip (&val, RE_E0_PRESSURE_MIN, RE_E0_PRESSURE_MAX);
        coded_val = (uint16_t) lrintf (val - RE_E0_PRESSURE_MIN);
    }

    p_slot[0] |= ( (uint32_t) coded_val >> RE_E0_BYTE_1_SHIFT) & RE_E0_BYTE_MASK;
    p_slot[1] |= (coded_val & RE_E0_BYTE_MASK);
}

static re_float
re_e0_decode_pressure (const uint8_t * const p_slot)
{
    uint16_t coded_val = 0;
    coded_val |= p_slot[1] & RE_E0_BYTE_MASK;
    coded_val |= ( (uint16_t) p_slot[0] & RE_E0_BYTE_MASK) << RE_E0_BYTE_1_SHIFT;

    if (RE_E0_INVALID_PRESSURE == coded_val)
    {
        return NAN;
    }

    return (re_float) coded_val + RE_E0_PRESSURE_MIN;
}

static void
re_e0_encode_pm (uint8_t * const p_slot, re_float val)
{
    uint16_t coded_val = RE_E0_INVALID_PM;

    if (!isnan (val))
    {
        re_clip (&val, RE_E0_PM_MIN, RE_E0_PM_MAX);
        coded_val = (uint16_t) lrintf (val * RE_E0_PM_RATIO);
    }

    p_slot[0] |= ( ( (uint32_t) coded_val >> RE_E0_BYTE_1_SHIFT) & RE_E0_BYTE_MASK);
    p_slot[1] |= (coded_val & RE_E0_BYTE_MASK);
}

static re_float
re_e0_decode_pm (const uint8_t * const p_slot)
{
    uint16_t coded_val = 0;
    coded_val |= p_slot[1] & RE_E0_BYTE_MASK;
    coded_val |= ( (uint16_t) p_slot[0] & RE_E0_BYTE_MASK) << RE_E0_BYTE_1_SHIFT;

    if (RE_E0_INVALID_PM == coded_val)
    {
        return NAN;
    }

    return (re_float) coded_val / RE_E0_PM_RATIO;
}

static void
re_e0_encode_co2 (uint8_t * const p_slot, re_float val)
{
    uint16_t coded_val = RE_E0_INVALID_CO2;

    if (!isnan (val))
    {
        re_clip (&val, RE_E0_CO2_MIN, RE_E0_CO2_MAX);
        coded_val = (uint16_t) lrintf (val * RE_E0_CO2_RATIO);
    }

    p_slot[0] |= ( (uint32_t) coded_val >> RE_E0_BYTE_1_SHIFT) & RE_E0_BYTE_MASK;
    p_slot[1] |= (coded_val & RE_E0_BYTE_MASK);
}

static re_float
re_e0_decode_co2 (const uint8_t * const p_slot)
{
    uint16_t coded_val = 0;
    coded_val |= p_slot[1] & RE_E0_BYTE_MASK;
    coded_val |= ( (uint16_t) p_slot[0] & RE_E0_BYTE_MASK) << RE_E0_BYTE_1_SHIFT;

    if (RE_E0_INVALID_CO2 == coded_val)
    {
        return NAN;
    }

    return (re_float) coded_val / RE_E0_CO2_RATIO;
}

static void
re_e0_encode_voc (uint8_t * const p_slot, re_float val)
{
    uint16_t coded_val = RE_E0_INVALID_VOC_INDEX;

    if ( (!isnan (val)) && (val >= RE_E0_VOC_INDEX_MIN) && (val <= RE_E0_VOC_INDEX_MAX))
    {
        coded_val = (uint16_t) lrintf (val);
    }

    p_slot[0] |= ( (uint32_t) coded_val >> RE_E0_BYTE_1_SHIFT) & RE_E0_BYTE_MASK;
    p_slot[1] |= (coded_val & RE_E0_BYTE_MASK);
}

static re_float
re_e0_decode_voc (const uint8_t * const p_slot)
{
    uint16_t coded_val = 0;
    coded_val |= p_slot[1] & RE_E0_BYTE_MASK;
    coded_val |= ( (uint16_t) p_slot[0] & RE_E0_BYTE_MASK) << RE_E0_BYTE_1_SHIFT;

    if (RE_E0_INVALID_VOC_INDEX == coded_val)
    {
        return NAN;
    }

    return (re_float) coded_val;
}

static void
re_e0_encode_nox (uint8_t * const p_slot, re_float val)
{
    uint16_t coded_val = RE_E0_INVALID_NOX_INDEX;

    if ( (!isnan (val)) && (val >= RE_E0_NOX_INDEX_MIN) && (val <= RE_E0_NOX_INDEX_MAX))
    {
        coded_val = (uint16_t) lrintf (val);
    }

    p_slot[0] |= ( (uint32_t) coded_val >> RE_E0_BYTE_1_SHIFT) & RE_E0_BYTE_MASK;
    p_slot[1] |= (coded_val & RE_E0_BYTE_MASK);
}

static re_float
re_e0_decode_nox (const uint8_t * const p_slot)
{
    uint16_t coded_val = 0;
    coded_val |= p_slot[1] & RE_E0_BYTE_MASK;
    coded_val |= ( (uint16_t) p_slot[0] & RE_E0_BYTE_MASK) << RE_E0_BYTE_1_SHIFT;

    if (RE_E0_INVALID_NOX_INDEX == coded_val)
    {
        return NAN;
    }

    return (re_float) coded_val;
}

static void
re_e0_encode_luminosity (uint8_t * const p_slot, re_float val)
{
    uint16_t coded_val = RE_E0_INVALID_LUMINOSITY;

    if (!isnan (val))
    {
        re_clip (&val, RE_E0_LUMINOSITY_MIN, RE_E0_LUMINOSITY_MAX);
        coded_val = (uint16_t) lrintf (val);
    }

    p_slot[0] |= ( (uint32_t) coded_val >> RE_E0_BYTE_1_SHIFT) & RE_E0_BYTE_MASK;
    p_slot[1] |= (coded_val & RE_E0_BYTE_MASK);
}

static re_float
re_e0_decode_luminosity (const uint8_t * const p_slot)
{
    uint16_t coded_val = 0;
    coded_val |= p_slot[1] & RE_E0_BYTE_MASK;
    coded_val |= ( (uint16_t) p_slot[0] & RE_E0_BYTE_MASK) << RE_E0_BYTE_1_SHIFT;

    if (RE_E0_INVALID_LUMINOSITY == coded_val)
    {
        return NAN;
    }

    return (re_float) coded_val;
}

static void
re_e0_encode_sound (uint8_t * const p_slot, re_float val)
{
    uint8_t coded_val = RE_E0_INVALID_SOUND;

    if (!isnan (val))
    {
        re_clip (&val, RE_E0_SOUND_MIN, RE_E0_SOUND_MAX);
        coded_val = (uint16_t) lrintf (val * RE_E0_SOUND_RATIO);
    }

    p_slot[0] |= coded_val;
}

static re_float
re_e0_decode_sound (const uint8_t * const p_slot)
{
    uint8_t coded_val = p_slot[0];

    if (RE_E0_INVALID_SOUND == coded_val)
    {
        return NAN;
    }

    return (re_float) coded_val / RE_E0_SOUND_RATIO;
}

static void
re_e0_encode_sequence (uint8_t * const p_slot, const uint16_t measurement_seq)
{
    p_slot[0] = (uint8_t) ( (measurement_seq >> RE_E0_BYTE_1_SHIFT) & RE_E0_BYTE_MASK);
    p_slot[1] = (uint8_t) (measurement_seq & RE_E0_BYTE_MASK);
}

static uint16_t
re_e0_decode_sequence (const uint8_t * const p_buffer)
{
    uint16_t measurement_seq = 0;
    measurement_seq |= p_buffer[1] & RE_E0_BYTE_MASK;
    measurement_seq |= (p_buffer[0] & RE_E0_BYTE_MASK) << RE_E0_BYTE_1_SHIFT;
    return measurement_seq;
}

static void
re_e0_encode_voltage (uint8_t * const p_slot, re_float val)
{
    uint8_t coded_val = RE_E0_INVALID_VOLTAGE;

    if (!isnan (val))
    {
        re_clip (&val, RE_E0_VOLTAGE_MIN, RE_E0_VOLTAGE_MAX);
        coded_val = (uint16_t) lrintf (val * RE_E0_VOLTAGE_RATIO);
    }

    p_slot[0] |= coded_val;
}

static re_float
re_e0_decode_voltage (const uint8_t * const p_slot)
{
    uint8_t coded_val = p_slot[0];

    if (RE_E0_INVALID_VOLTAGE == coded_val)
    {
        return NAN;
    }

    return (re_float) coded_val / RE_E0_VOLTAGE_RATIO;
}

static void
re_e0_encode_flags (uint8_t * const p_slot, const re_e0_data_t * const p_data)
{
    uint8_t flags = 0;
    flags |= (p_data->flag_usb_on ? RE_E0_FLAGS_USB_ON : 0U);
    flags |= (p_data->flag_low_battery ? RE_E0_FLAGS_LOW_BATTERY : 0U);
    flags |= (p_data->flag_calibration_in_progress ? RE_E0_FLAGS_CALIBRATION_IN_PROGRESS :
              0U);
    flags |= (p_data->flag_boost_mode ? RE_E0_FLAGS_BOOST_MODE : 0U);
    *p_slot = flags;
}

static void
re_e0_decode_flags (const uint8_t * const p_slot, re_e0_data_t * const p_data)
{
    const uint8_t flags = *p_slot;
    p_data->flag_usb_on = (flags & RE_E0_FLAGS_USB_ON) ? true : false;
    p_data->flag_low_battery = (flags & RE_E0_FLAGS_LOW_BATTERY) ? true : false;
    p_data->flag_calibration_in_progress = (flags & RE_E0_FLAGS_CALIBRATION_IN_PROGRESS) ?
                                           true : false;
    p_data->flag_boost_mode = (flags & RE_E0_FLAGS_BOOST_MODE) ? true : false;
}

static void
re_e0_encode_set_address (uint8_t * const p_buffer, const re_e0_data_t * p_data)
{
    // Address is 64 bits, skip 2 first bytes
    uint8_t  addr_offset = RE_E0_OFFSET_ADDR_MSB;
    uint64_t mac         = p_data->address;

    if (RE_E0_MAC_MAX < p_data->address)
    {
        mac = RE_E0_INVALID_MAC;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"

    if (RE_E0_MAC_MIN > p_data->address)
    {
        mac = RE_E0_INVALID_MAC;
    }

#pragma GCC diagnostic pop
    p_buffer[addr_offset] = (mac >> RE_E0_BYTE_5_SHIFT) & RE_E0_BYTE_MASK;
    addr_offset++;
    p_buffer[addr_offset] = (mac >> RE_E0_BYTE_4_SHIFT) & RE_E0_BYTE_MASK;
    addr_offset++;
    p_buffer[addr_offset] = (mac >> RE_E0_BYTE_3_SHIFT) & RE_E0_BYTE_MASK;
    addr_offset++;
    p_buffer[addr_offset] = (mac >> RE_E0_BYTE_2_SHIFT) & RE_E0_BYTE_MASK;
    addr_offset++;
    p_buffer[addr_offset] = (mac >> RE_E0_BYTE_1_SHIFT) & RE_E0_BYTE_MASK;
    addr_offset++;
    p_buffer[addr_offset] = (mac >> RE_E0_BYTE_0_SHIFT) & RE_E0_BYTE_MASK;
}

static uint64_t
re_e0_decode_address (const uint8_t * const p_buffer)
{
    // Address is 64 bits, skip 2 first bytes
    uint8_t  addr_offset = 0;
    uint64_t mac         = 0;
    mac |= p_buffer[addr_offset];
    mac <<= RE_E0_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    mac <<= RE_E0_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    mac <<= RE_E0_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    mac <<= RE_E0_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    mac <<= RE_E0_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    return mac;
}

re_status_t
re_e0_encode (uint8_t * const p_buffer, const re_e0_data_t * const p_data)
{
    re_status_t result = RE_SUCCESS;

    if ( (NULL == p_buffer) || (NULL == p_data))
    {
        result |= RE_ERROR_NULL;
    }
    else
    {
        memset (p_buffer, 0x00U, RE_E0_DATA_LENGTH);
        p_buffer[RE_E0_OFFSET_HEADER] = RE_E0_DESTINATION;
        re_e0_encode_temperature (&p_buffer[RE_E0_OFFSET_TEMPERATURE_MSB], p_data->temperature_c);
        re_e0_encode_humidity (&p_buffer[RE_E0_OFFSET_HUMIDITY_MSB], p_data->humidity_rh);
        re_e0_encode_pressure (&p_buffer[RE_E0_OFFSET_PRESSURE_MSB], p_data->pressure_pa);
        re_e0_encode_pm (&p_buffer[RE_E0_OFFSET_PM_1_0_MSB], p_data->pm1p0_ppm);
        re_e0_encode_pm (&p_buffer[RE_E0_OFFSET_PM_2_5_MSB], p_data->pm2p5_ppm);
        re_e0_encode_pm (&p_buffer[RE_E0_OFFSET_PM_4_0_MSB], p_data->pm4p0_ppm);
        re_e0_encode_pm (&p_buffer[RE_E0_OFFSET_PM_10_0_MSB], p_data->pm10p0_ppm);
        re_e0_encode_co2 (&p_buffer[RE_E0_OFFSET_CO2_MSB], p_data->co2);
        re_e0_encode_voc (&p_buffer[RE_E0_OFFSET_VOC_INDEX_MSB], p_data->voc_index);
        re_e0_encode_nox (&p_buffer[RE_E0_OFFSET_NOX_INDEX_MSB], p_data->nox_index);
        re_e0_encode_luminosity (&p_buffer[RE_E0_OFFSET_LUMINOSITY_MSB], p_data->luminosity);
        re_e0_encode_sound (&p_buffer[RE_E0_OFFSET_SOUND_AVG_DBA], p_data->sound_avg_dba);
        re_e0_encode_sound (&p_buffer[RE_E0_OFFSET_SOUND_PEAK_SPL_DB], p_data->sound_peak_spl_db);
        re_e0_encode_sequence (&p_buffer[RE_E0_OFFSET_SEQ_CTR_MSB], p_data->measurement_count);
        re_e0_encode_voltage (&p_buffer[RE_E0_OFFSET_VOLTAGE], p_data->voltage);
        re_e0_encode_flags (&p_buffer[RE_E0_OFFSET_FLAGS], p_data);
        p_buffer[RE_E0_OFFSET_RESERVED + 0] = 0xFFU;
        p_buffer[RE_E0_OFFSET_RESERVED + 1] = 0xFFU;
        p_buffer[RE_E0_OFFSET_RESERVED + 2] = 0xFFU;
        p_buffer[RE_E0_OFFSET_RESERVED + 3] = 0xFFU;
        p_buffer[RE_E0_OFFSET_RESERVED + 4] = 0xFFU;
        p_buffer[RE_E0_OFFSET_RESERVED + 5] = 0xFFU;
        re_e0_encode_set_address (p_buffer, p_data);
    }

    return result;
}

bool re_e0_check_format (const uint8_t * const p_buffer)
{
    if (NULL == p_buffer)
    {
        return false;
    }

    if (RE_E0_RAW_PACKET_LENGTH_VAL != p_buffer[RE_E0_RAW_PACKET_LENGTH_OFFSET])
    {
        return false;
    }

    if (RE_E0_RAW_PACKET_TYPE_VAL != p_buffer[RE_E0_RAW_PACKET_TYPE_OFFSET])
    {
        return false;
    }

    const uint16_t manufacturer_id = (uint16_t) ( (uint16_t)
                                     p_buffer[RE_E0_RAW_PACKET_MANUFACTURER_ID_OFFSET_HI] << RE_E0_BYTE_1_SHIFT)
                                     + p_buffer[RE_E0_RAW_PACKET_MANUFACTURER_ID_OFFSET_LO];

    if (RE_E0_RAW_PACKET_MANUFACTURER_ID_VAL != manufacturer_id)
    {
        return false;
    }

    if (RE_E0_DESTINATION != p_buffer[RE_E0_OFFSET_PAYLOAD + RE_E0_OFFSET_HEADER])
    {
        return false;
    }

    return true;
}

re_status_t re_e0_decode (const uint8_t * const p_buffer, re_e0_data_t * const p_data)
{
    const uint8_t * const p_payload = &p_buffer[RE_E0_OFFSET_PAYLOAD];
    re_status_t result = RE_SUCCESS;

    if ( (NULL == p_payload) || (NULL == p_data))
    {
        return RE_ERROR_NULL;
    }

    memset (p_data, 0, sizeof (*p_data));

    if (RE_E0_DESTINATION != p_payload[RE_E0_OFFSET_HEADER])
    {
        return RE_ERROR_INVALID_PARAM;
    }

    p_data->temperature_c = re_e0_decode_temperature (
                                &p_payload[RE_E0_OFFSET_TEMPERATURE_MSB]);
    p_data->humidity_rh = re_e0_decode_humidity (&p_payload[RE_E0_OFFSET_HUMIDITY_MSB]);
    p_data->pressure_pa = re_e0_decode_pressure (&p_payload[RE_E0_OFFSET_PRESSURE_MSB]);
    p_data->pm1p0_ppm = re_e0_decode_pm (&p_payload[RE_E0_OFFSET_PM_1_0_MSB]);
    p_data->pm2p5_ppm = re_e0_decode_pm (&p_payload[RE_E0_OFFSET_PM_2_5_MSB]);
    p_data->pm4p0_ppm = re_e0_decode_pm (&p_payload[RE_E0_OFFSET_PM_4_0_MSB]);
    p_data->pm10p0_ppm = re_e0_decode_pm (&p_payload[RE_E0_OFFSET_PM_10_0_MSB]);
    p_data->co2 = re_e0_decode_co2 (&p_payload[RE_E0_OFFSET_CO2_MSB]);
    p_data->voc_index = re_e0_decode_voc (&p_payload[RE_E0_OFFSET_VOC_INDEX_MSB]);
    p_data->nox_index = re_e0_decode_nox (&p_payload[RE_E0_OFFSET_NOX_INDEX_MSB]);
    p_data->luminosity = re_e0_decode_luminosity (&p_payload[RE_E0_OFFSET_LUMINOSITY_MSB]);
    p_data->sound_avg_dba = re_e0_decode_sound (&p_payload[RE_E0_OFFSET_SOUND_AVG_DBA]);
    p_data->sound_peak_spl_db = re_e0_decode_sound (
                                    &p_payload[RE_E0_OFFSET_SOUND_PEAK_SPL_DB]);
    p_data->measurement_count = re_e0_decode_sequence (&p_payload[RE_E0_OFFSET_SEQ_CTR_MSB]);
    p_data->voltage = re_e0_decode_voltage (&p_payload[RE_E0_OFFSET_VOLTAGE]);
    re_e0_decode_flags (&p_payload[RE_E0_OFFSET_FLAGS], p_data);
    p_data->address = re_e0_decode_address (&p_payload[RE_E0_OFFSET_ADDR_MSB]);
    return result;
}

re_e0_data_t
re_e0_data_invalid (const uint16_t measurement_cnt, const uint64_t radio_mac)
{
    const re_e0_data_t data =
    {
        .temperature_c     = NAN,
        .humidity_rh       = NAN,
        .pressure_pa       = NAN,
        .pm1p0_ppm         = NAN,
        .pm2p5_ppm         = NAN,
        .pm4p0_ppm         = NAN,
        .pm10p0_ppm        = NAN,
        .co2               = NAN,
        .voc_index         = NAN,
        .nox_index         = NAN,
        .luminosity        = NAN,
        .sound_avg_dba     = NAN,
        .sound_peak_spl_db    = NAN,
        .measurement_count = measurement_cnt,
        .voltage           = NAN,

        .flag_usb_on                  = false,
        .flag_low_battery             = false,
        .flag_calibration_in_progress = false,
        .flag_boost_mode              = false,

        .address = radio_mac,
    };
    return data;
}

#endif
