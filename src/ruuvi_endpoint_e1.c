#include "ruuvi_endpoint_e1.h"
#include "ruuvi_endpoints.h"
#include <stddef.h>
#include <string.h>
#include <math.h>
#include "ruuvi_endpoints_internal.h"

#if RE_E1_ENABLED

#define RE_E1_MAC_MAX (0xFFFFFFFFFFFFU)
#define RE_E1_MAC_MIN (0)

#define RE_E1_RAW_PACKET_LENGTH_OFFSET             (0U)
#define RE_E1_RAW_PACKET_LENGTH_VAL                (43U)
#define RE_E1_RAW_PACKET_TYPE_OFFSET               (1U)
#define RE_E1_RAW_PACKET_TYPE_VAL                  (0xFFU)
#define RE_E1_RAW_PACKET_MANUFACTURER_ID_OFFSET_LO (2U)
#define RE_E1_RAW_PACKET_MANUFACTURER_ID_OFFSET_HI (3U)
#define RE_E1_RAW_PACKET_MANUFACTURER_ID_VAL       (0x499U)

const re_u9_coeffs_t re_e1_coeffs_voc =
{
    .bit9_offset = RE_E1_VOC_BIT9_OFFSET,
    .invalid_val = RE_E1_INVALID_VOC,
    .min_val     = RE_E1_VOC_MIN,
    .max_val     = RE_E1_VOC_MAX,
    .ratio       = RE_E1_VOC_RATIO,
};

const re_u9_coeffs_t re_e1_coeffs_nox =
{
    .bit9_offset = RE_E1_NOX_BIT9_OFFSET,
    .invalid_val = RE_E1_INVALID_NOX,
    .min_val     = RE_E1_NOX_MIN,
    .max_val     = RE_E1_NOX_MAX,
    .ratio       = RE_E1_NOX_RATIO,
};

const re_u9_coeffs_t re_e1_coeffs_sound_inst_dba =
{
    .bit9_offset = RE_E1_SOUND_INST_DBA_BIT9_OFFSET,
    .invalid_val = RE_E1_INVALID_SOUND,
    .min_val     = RE_E1_SOUND_MIN,
    .max_val     = RE_E1_SOUND_MAX,
    .ratio       = RE_E1_SOUND_RATIO,
};

const re_u9_coeffs_t re_e1_coeffs_sound_avg_dba =
{
    .bit9_offset = RE_E1_SOUND_AVG_DBA_BIT9_OFFSET,
    .invalid_val = RE_E1_INVALID_SOUND,
    .min_val     = RE_E1_SOUND_MIN,
    .max_val     = RE_E1_SOUND_MAX,
    .ratio       = RE_E1_SOUND_RATIO,
};

const re_u9_coeffs_t re_e1_coeffs_sound_peak_spl_db =
{
    .bit9_offset = RE_E1_SOUND_PEAK_SPL_DB_BIT9_OFFSET,
    .invalid_val = RE_E1_INVALID_SOUND,
    .min_val     = RE_E1_SOUND_MIN,
    .max_val     = RE_E1_SOUND_MAX,
    .ratio       = RE_E1_SOUND_RATIO,
};

const re_i16_coeffs_t re_e1_coeffs_temperature =
{
    .invalid_val = RE_E1_INVALID_TEMPERATURE,
    .max_abs_val = RE_E1_TEMPERATURE_MAX,
    .ratio       = RE_E1_TEMPERATURE_RATIO,
};

static const re_u16_coeffs_t re_e1_coeffs_humidity =
{
    .invalid_val = RE_E1_INVALID_HUMIDITY,
    .min_val     = RE_E1_HUMIDITY_MIN,
    .max_val     = RE_E1_HUMIDITY_MAX,
    .ratio       = RE_E1_HUMIDITY_RATIO,
};

static const re_u16_coeffs_t re_e1_coeffs_pressure =
{
    .invalid_val = RE_E1_INVALID_PRESSURE,
    .min_val     = RE_E1_PRESSURE_MIN,
    .max_val     = RE_E1_PRESSURE_MAX,
    .ratio       = RE_E1_PRESSURE_RATIO,
};

static const re_u16_coeffs_t re_e1_coeffs_pm =
{
    .invalid_val = RE_E1_INVALID_PM,
    .min_val     = RE_E1_PM_MIN,
    .max_val     = RE_E1_PM_MAX,
    .ratio       = RE_E1_PM_RATIO,
};

static const re_u16_coeffs_t re_e1_coeffs_co2 =
{
    .invalid_val = RE_E1_INVALID_CO2,
    .min_val     = RE_E1_CO2_MIN,
    .max_val     = RE_E1_CO2_MAX,
    .ratio       = RE_E1_CO2_RATIO,
};

static void
re_e1_encode_temperature (uint8_t * const p_slot, re_float val)
{
    return re_encode_i16 (p_slot, &re_e1_coeffs_temperature, val);
}

static re_float
re_e1_decode_temperature (const uint8_t * const p_slot)
{
    return re_decode_i16 (p_slot, &re_e1_coeffs_temperature);
}

static void
re_e1_encode_humidity (uint8_t * const p_slot, re_float val)
{
    re_encode_u16 (p_slot, &re_e1_coeffs_humidity, val);
}

static re_float
re_e1_decode_humidity (const uint8_t * const p_slot)
{
    return re_decode_u16 (p_slot, &re_e1_coeffs_humidity);
}

static void
re_e1_encode_pressure (uint8_t * const p_slot, re_float val)
{
    re_encode_u16 (p_slot, &re_e1_coeffs_pressure, val);
}

static re_float
re_e1_decode_pressure (const uint8_t * const p_slot)
{
    return re_decode_u16 (p_slot, &re_e1_coeffs_pressure);
}

static void
re_e1_encode_pm (uint8_t * const p_slot, re_float val)
{
    re_encode_u16 (p_slot, &re_e1_coeffs_pm, val);
}

static re_float
re_e1_decode_pm (const uint8_t * const p_slot)
{
    return re_decode_u16 (p_slot, &re_e1_coeffs_pm);
}

static void
re_e1_encode_co2 (uint8_t * const p_slot, const re_float val)
{
    re_encode_u16 (p_slot, &re_e1_coeffs_co2, val);
}

static re_float
re_e1_decode_co2 (const uint8_t * const p_slot)
{
    return re_decode_u16 (p_slot, &re_e1_coeffs_co2);
}

static void
re_e1_encode_voc (uint8_t * const p_slot, uint8_t * const p_flags, const re_float val)
{
    re_encode_u9 (p_slot, p_flags, &re_e1_coeffs_voc, val);
}

static re_float
re_e1_decode_voc (const uint8_t * const p_slot, const uint8_t * const p_flags)
{
    return re_decode_u9 (p_slot, p_flags, &re_e1_coeffs_voc);
}

static void
re_e1_encode_nox (uint8_t * const p_slot, uint8_t * const p_flags, const re_float val)
{
    re_encode_u9 (p_slot, p_flags, &re_e1_coeffs_nox, val);
}

static re_float
re_e1_decode_nox (const uint8_t * const p_slot, const uint8_t * const p_flags)
{
    return re_decode_u9 (p_slot, p_flags, &re_e1_coeffs_nox);
}

static inline uint8_t
re_be24_get_high_byte (const uint32_t value)
{
    return (uint8_t) ( (value >> RE_BYTE_2_SHIFT) & RE_BYTE_MASK);
}

static inline uint8_t
re_be24_get_mid_byte (const uint32_t value)
{
    return (uint8_t) ( (value >> RE_BYTE_1_SHIFT) & RE_BYTE_MASK);
}

static inline uint8_t
re_be24_get_low_byte (const uint32_t value)
{
    return (uint8_t) (value & RE_BYTE_MASK);
}

static inline void
re_encode_u24 (
    uint8_t * const p_slot,
    const re_float val,
    const uint32_t invalid_val,
    const re_float min_val,
    const re_float max_val,
    const re_float scale_factor)
{
    uint32_t coded_val = invalid_val;

    if (!isnan (val))
    {
        const re_float val_clipped = RE_CLIP (val, min_val, max_val);
        coded_val                  = (uint32_t) lrintf ( (val_clipped - min_val) * scale_factor);
    }

    p_slot[0] |= re_be24_get_high_byte (coded_val);
    p_slot[1] |= re_be24_get_mid_byte (coded_val);
    p_slot[2] |= re_be24_get_low_byte (coded_val);
}

static inline re_float
re_decode_u24 (
    const uint8_t * const p_slot,
    const uint32_t       invalid_val,
    const re_float       min_val,
    const re_float       scale_factor)
{
    uint32_t coded_val = 0;
    coded_val |= ( (uint32_t) p_slot[0]) << RE_BYTE_2_SHIFT;
    coded_val |= ( (uint32_t) p_slot[1]) << RE_BYTE_1_SHIFT;
    coded_val |= ( (uint32_t) p_slot[2]) << RE_BYTE_0_SHIFT;

    if (invalid_val == coded_val)
    {
        return NAN;
    }

    return (re_float) coded_val / scale_factor + min_val;
}

static void
re_e1_encode_luminosity (uint8_t * const p_slot, const re_float val)
{
    re_encode_u24 (
        p_slot,
        val,
        RE_E1_INVALID_LUMINOSITY,
        RE_E1_LUMINOSITY_MIN,
        RE_E1_LUMINOSITY_MAX,
        RE_E1_LUMINOSITY_RATIO);
}

static re_float
re_e1_decode_luminosity (const uint8_t * const p_slot)
{
    return re_decode_u24 (p_slot, RE_E1_INVALID_LUMINOSITY, RE_E1_LUMINOSITY_MIN,
                          RE_E1_LUMINOSITY_RATIO);
}

static void
re_e1_encode_sound (
    uint8_t * const              p_slot,
    uint8_t * const              p_flags,
    const re_u9_coeffs_t * const p_coeffs,
    const re_float              val)
{
    re_encode_u9 (p_slot, p_flags, p_coeffs, val);
}

static re_float
re_e1_decode_sound (const uint8_t * const p_slot, const uint8_t * const p_flags,
                    const re_u9_coeffs_t * const p_coeffs)
{
    return re_decode_u9 (p_slot, p_flags, p_coeffs);
}

static void
re_e1_encode_sequence (uint8_t * const p_slot, const uint32_t measurement_seq)
{
    p_slot[0] = (uint8_t) ( (measurement_seq >> RE_BYTE_2_SHIFT) & RE_BYTE_MASK);
    p_slot[1] = (uint8_t) ( (measurement_seq >> RE_BYTE_1_SHIFT) & RE_BYTE_MASK);
    p_slot[2] = (uint8_t) ( (measurement_seq >> RE_BYTE_0_SHIFT) & RE_BYTE_MASK);
}

static uint32_t
re_e1_decode_sequence (const uint8_t * const p_buffer)
{
    uint32_t measurement_seq = 0;
    measurement_seq |= ( (uint32_t) p_buffer[0]) << RE_BYTE_2_SHIFT;
    measurement_seq |= ( (uint32_t) p_buffer[1]) << RE_BYTE_1_SHIFT;
    measurement_seq |= ( (uint32_t) p_buffer[2]) << RE_BYTE_0_SHIFT;
    return measurement_seq;
}

static void
re_e1_encode_flags (uint8_t * const p_slot, const re_e1_flags_t flags)
{
    if (flags.flag_calibration_in_progress)
    {
        *p_slot |= RE_E1_FLAGS_CALIBRATION_IN_PROGRESS;
    }

    if (flags.flag_button_pressed)
    {
        *p_slot |= RE_E1_FLAGS_BUTTON_PRESSED;
    }

    if (flags.flag_rtc_running_on_boot)
    {
        *p_slot |= RE_E1_FLAGS_RTC_RUNNING_ON_BOOT;
    }
}

static re_e1_flags_t
re_e1_decode_flags (const uint8_t * const p_flags)
{
    re_e1_flags_t flags = { .flag_calibration_in_progress = false,
                            .flag_button_pressed          = false,
                            .flag_rtc_running_on_boot     = false
                          };

    if (*p_flags & RE_E1_FLAGS_CALIBRATION_IN_PROGRESS)
    {
        flags.flag_calibration_in_progress = true;
    }

    if (*p_flags & RE_E1_FLAGS_BUTTON_PRESSED)
    {
        flags.flag_button_pressed = true;
    }

    if (*p_flags & RE_E1_FLAGS_RTC_RUNNING_ON_BOOT)
    {
        flags.flag_rtc_running_on_boot = true;
    }

    return flags;
}

static void
re_e1_encode_address (uint8_t * const p_buffer, const uint64_t mac_addr)
{
    // Address is 64 bits, skip 2 first bytes
    uint8_t  addr_offset = RE_E1_OFFSET_ADDR_MSB;
    uint64_t mac         = mac_addr;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"

    if (RE_E1_MAC_MAX < mac_addr)
    {
        mac = RE_E1_INVALID_MAC;
    }
    else if (RE_E1_MAC_MIN > mac_addr)
    {
        mac = RE_E1_INVALID_MAC;
    }
    else
    {
        // MISRA C:2012, 15.7 - All if...else if constructs shall be terminated with an else statement
    }

#pragma GCC diagnostic pop
    p_buffer[addr_offset] = (mac >> RE_BYTE_5_SHIFT) & RE_BYTE_MASK;
    addr_offset++;
    p_buffer[addr_offset] = (mac >> RE_BYTE_4_SHIFT) & RE_BYTE_MASK;
    addr_offset++;
    p_buffer[addr_offset] = (mac >> RE_BYTE_3_SHIFT) & RE_BYTE_MASK;
    addr_offset++;
    p_buffer[addr_offset] = (mac >> RE_BYTE_2_SHIFT) & RE_BYTE_MASK;
    addr_offset++;
    p_buffer[addr_offset] = (mac >> RE_BYTE_1_SHIFT) & RE_BYTE_MASK;
    addr_offset++;
    p_buffer[addr_offset] = (mac >> RE_BYTE_0_SHIFT) & RE_BYTE_MASK;
}

static uint64_t
re_e1_decode_address (const uint8_t * const p_buffer)
{
    // Address is 64 bits, skip 2 first bytes
    uint8_t  addr_offset = 0;
    uint64_t mac         = 0;
    mac |= p_buffer[addr_offset];
    mac <<= RE_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    mac <<= RE_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    mac <<= RE_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    mac <<= RE_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    mac <<= RE_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    return mac;
}

re_status_t
re_e1_encode (uint8_t * const p_buffer, const re_e1_data_t * const p_data)
{
    re_status_t result = RE_SUCCESS;

    if ( (NULL == p_buffer) || (NULL == p_data))
    {
        result |= RE_ERROR_NULL;
    }
    else
    {
        memset (p_buffer, 0x00U, RE_E1_DATA_LENGTH);
        p_buffer[RE_E1_OFFSET_HEADER] = RE_E1_DESTINATION;
        re_e1_encode_temperature (&p_buffer[RE_E1_OFFSET_TEMPERATURE_MSB], p_data->temperature_c);
        re_e1_encode_humidity (&p_buffer[RE_E1_OFFSET_HUMIDITY_MSB], p_data->humidity_rh);
        re_e1_encode_pressure (&p_buffer[RE_E1_OFFSET_PRESSURE_MSB], p_data->pressure_pa);
        re_e1_encode_pm (&p_buffer[RE_E1_OFFSET_PM_1_0_MSB], p_data->pm1p0_ppm);
        re_e1_encode_pm (&p_buffer[RE_E1_OFFSET_PM_2_5_MSB], p_data->pm2p5_ppm);
        re_e1_encode_pm (&p_buffer[RE_E1_OFFSET_PM_4_0_MSB], p_data->pm4p0_ppm);
        re_e1_encode_pm (&p_buffer[RE_E1_OFFSET_PM_10_0_MSB], p_data->pm10p0_ppm);
        re_e1_encode_co2 (&p_buffer[RE_E1_OFFSET_CO2_MSB], p_data->co2);
        re_e1_encode_voc (&p_buffer[RE_E1_OFFSET_VOC], &p_buffer[RE_E1_OFFSET_FLAGS],
                          p_data->voc);
        re_e1_encode_nox (&p_buffer[RE_E1_OFFSET_NOX], &p_buffer[RE_E1_OFFSET_FLAGS],
                          p_data->nox);
        re_e1_encode_luminosity (&p_buffer[RE_E1_OFFSET_LUMINOSITY_MSB], p_data->luminosity);
        re_e1_encode_sound (
            &p_buffer[RE_E1_OFFSET_SOUND_INST_DBA],
            &p_buffer[RE_E1_OFFSET_FLAGS],
            &re_e1_coeffs_sound_inst_dba,
            p_data->sound_inst_dba);
        re_e1_encode_sound (
            &p_buffer[RE_E1_OFFSET_SOUND_AVG_DBA],
            &p_buffer[RE_E1_OFFSET_FLAGS],
            &re_e1_coeffs_sound_avg_dba,
            p_data->sound_avg_dba);
        re_e1_encode_sound (
            &p_buffer[RE_E1_OFFSET_SOUND_PEAK_SPL_DB],
            &p_buffer[RE_E1_OFFSET_FLAGS],
            &re_e1_coeffs_sound_peak_spl_db,
            p_data->sound_peak_spl_db);
        re_e1_encode_sequence (&p_buffer[RE_E1_OFFSET_SEQ_CNT_MSB], p_data->seq_cnt);
        re_e1_encode_flags (&p_buffer[RE_E1_OFFSET_FLAGS], p_data->flags);
        p_buffer[RE_E1_OFFSET_RESERVED + 0] = 0xFFU;
        p_buffer[RE_E1_OFFSET_RESERVED + 1] = 0xFFU;
        p_buffer[RE_E1_OFFSET_RESERVED + 2] = 0xFFU;
        p_buffer[RE_E1_OFFSET_RESERVED + 3] = 0xFFU;
        p_buffer[RE_E1_OFFSET_RESERVED + 4] = 0xFFU;
        re_e1_encode_address (p_buffer, p_data->address);
    }

    return result;
}

bool
re_e1_check_format (const uint8_t * const p_buffer)
{
    if (NULL == p_buffer)
    {
        return false;
    }

    if (RE_E1_RAW_PACKET_LENGTH_VAL != p_buffer[RE_E1_RAW_PACKET_LENGTH_OFFSET])
    {
        return false;
    }

    if (RE_E1_RAW_PACKET_TYPE_VAL != p_buffer[RE_E1_RAW_PACKET_TYPE_OFFSET])
    {
        return false;
    }

    const uint16_t manufacturer_id = (uint16_t) ( (uint16_t)
                                     p_buffer[RE_E1_RAW_PACKET_MANUFACTURER_ID_OFFSET_HI]
                                     << RE_BYTE_1_SHIFT)
                                     + p_buffer[RE_E1_RAW_PACKET_MANUFACTURER_ID_OFFSET_LO];

    if (RE_E1_RAW_PACKET_MANUFACTURER_ID_VAL != manufacturer_id)
    {
        return false;
    }

    if (RE_E1_DESTINATION != p_buffer[RE_E1_OFFSET_PAYLOAD + RE_E1_OFFSET_HEADER])
    {
        return false;
    }

    return true;
}

re_status_t
re_e1_decode (const uint8_t * const p_buffer, re_e1_data_t * const p_data)
{
    const uint8_t * const p_payload = &p_buffer[RE_E1_OFFSET_PAYLOAD];
    re_status_t          result    = RE_SUCCESS;

    if ( (NULL == p_payload) || (NULL == p_data))
    {
        return RE_ERROR_NULL;
    }

    memset (p_data, 0, sizeof (*p_data));

    if (RE_E1_DESTINATION != p_payload[RE_E1_OFFSET_HEADER])
    {
        return RE_ERROR_INVALID_PARAM;
    }

    p_data->temperature_c  = re_e1_decode_temperature (
                                 &p_payload[RE_E1_OFFSET_TEMPERATURE_MSB]);
    p_data->humidity_rh    = re_e1_decode_humidity (&p_payload[RE_E1_OFFSET_HUMIDITY_MSB]);
    p_data->pressure_pa    = re_e1_decode_pressure (&p_payload[RE_E1_OFFSET_PRESSURE_MSB]);
    p_data->pm1p0_ppm      = re_e1_decode_pm (&p_payload[RE_E1_OFFSET_PM_1_0_MSB]);
    p_data->pm2p5_ppm      = re_e1_decode_pm (&p_payload[RE_E1_OFFSET_PM_2_5_MSB]);
    p_data->pm4p0_ppm      = re_e1_decode_pm (&p_payload[RE_E1_OFFSET_PM_4_0_MSB]);
    p_data->pm10p0_ppm     = re_e1_decode_pm (&p_payload[RE_E1_OFFSET_PM_10_0_MSB]);
    p_data->co2            = re_e1_decode_co2 (&p_payload[RE_E1_OFFSET_CO2_MSB]);
    p_data->voc            = re_e1_decode_voc (&p_payload[RE_E1_OFFSET_VOC],
                             &p_payload[RE_E1_OFFSET_FLAGS]);
    p_data->nox            = re_e1_decode_nox (&p_payload[RE_E1_OFFSET_NOX],
                             &p_payload[RE_E1_OFFSET_FLAGS]);
    p_data->luminosity     = re_e1_decode_luminosity (
                                 &p_payload[RE_E1_OFFSET_LUMINOSITY_MSB]);
    p_data->sound_inst_dba = re_e1_decode_sound (
                                 &p_payload[RE_E1_OFFSET_SOUND_INST_DBA],
                                 &p_payload[RE_E1_OFFSET_FLAGS],
                                 &re_e1_coeffs_sound_inst_dba);
    p_data->sound_avg_dba = re_e1_decode_sound (
                                &p_payload[RE_E1_OFFSET_SOUND_AVG_DBA],
                                &p_payload[RE_E1_OFFSET_FLAGS],
                                &re_e1_coeffs_sound_avg_dba);
    p_data->sound_peak_spl_db = re_e1_decode_sound (
                                    &p_payload[RE_E1_OFFSET_SOUND_PEAK_SPL_DB],
                                    &p_payload[RE_E1_OFFSET_FLAGS],
                                    &re_e1_coeffs_sound_peak_spl_db);
    p_data->seq_cnt = re_e1_decode_sequence (&p_payload[RE_E1_OFFSET_SEQ_CNT_MSB]);
    p_data->flags   = re_e1_decode_flags (&p_payload[RE_E1_OFFSET_FLAGS]);
    p_data->address = re_e1_decode_address (&p_payload[RE_E1_OFFSET_ADDR_MSB]);
    return result;
}

re_e1_data_t
re_e1_data_invalid (const uint16_t measurement_cnt, const uint64_t radio_mac)
{
    const re_e1_data_t data =
    {
        .temperature_c     = NAN,
        .humidity_rh       = NAN,
        .pressure_pa       = NAN,
        .pm1p0_ppm         = NAN,
        .pm2p5_ppm         = NAN,
        .pm4p0_ppm         = NAN,
        .pm10p0_ppm        = NAN,
        .co2               = NAN,
        .voc         = NAN,
        .nox         = NAN,
        .luminosity        = NAN,
        .sound_inst_dba     = NAN,
        .sound_avg_dba     = NAN,
        .sound_peak_spl_db    = NAN,
        .seq_cnt = measurement_cnt,

        .flags = {
            .flag_calibration_in_progress = false,
            .flag_button_pressed          = false,
            .flag_rtc_running_on_boot     = false,
        },

        .address = radio_mac,
    };
    return data;
}

#endif
