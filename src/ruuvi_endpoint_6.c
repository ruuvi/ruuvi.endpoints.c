#include "ruuvi_endpoint_6.h"
#include "ruuvi_endpoints.h"
#include <stddef.h>
#include <string.h>
#include <math.h>
#include "ruuvi_endpoints_internal.h"

#if RE_6_ENABLED

// Advertising Data (AD) Element 1: Flags
#define RE_6_RAW_PACKET_AD1_FLAGS_LEN_OFFSET     (0U)
#define RE_6_RAW_PACKET_AD1_FLAGS_LEN_VAL        (2U)
#define RE_6_RAW_PACKET_AD1_FLAGS_AD_TYPE_OFFSET (1U)
#define RE_6_RAW_PACKET_AD1_FLAGS_AD_TYPE_VAL    (1U)
#define RE_6_RAW_PACKET_AD1_FLAGS_AD_DATA_OFFSET (2U)

// Advertising Data (AD) Element 2: Complete List of 16-bit Service Class UUIDs
#define RE_6_RAW_PACKET_AD2_UUID_LEN_OFFSET      (3U)
#define RE_6_RAW_PACKET_AD2_UUID_LEN_VAL         (3U)
#define RE_6_RAW_PACKET_AD2_UUID_AD_TYPE_OFFSET  (4U)
#define RE_6_RAW_PACKET_AD2_UUID_AD_TYPE_VAL     (3U)
#define RE_6_RAW_PACKET_AD2_UUID_AD_DATA1_OFFSET (5U)
#define RE_6_RAW_PACKET_AD2_UUID_AD_DATA2_OFFSET (6U)
#define RE_6_RAW_PACKET_AD2_UUID_VAL             (0xFC98U)

// Advertising Data (AD) Element 3: Manufacturer Specific Data (MSD)
#define RE_6_RAW_PACKET_AD3_MSD_LEN_OFFSET                (7U)
#define RE_6_RAW_PACKET_AD3_MSD_LEN_VAL                   (23U)
#define RE_6_RAW_PACKET_AD3_MSD_AD_TYPE_OFFSET            (8U)
#define RE_6_RAW_PACKET_AD3_MSD_AD_TYPE_VAL               (0xFFU)
#define RE_6_RAW_PACKET_AD3_MSD_MANUFACTURER_ID_OFFSET_LO (9U)
#define RE_6_RAW_PACKET_AD3_MSD_MANUFACTURER_ID_OFFSET_HI (10U)
#define RE_6_RAW_PACKET_AD3_MANUFACTURER_ID_VAL           (0x499U)

const re_u9_coeffs_t re_6_coeffs_vox =
{
    .bit9_offset = RE_6_VOC_BIT9_OFFSET,
    .invalid_val = RE_6_INVALID_VOC,
    .min_val     = RE_6_VOC_MIN,
    .max_val     = RE_6_VOC_MAX,
    .ratio       = RE_6_VOC_RATIO,
};

const re_u9_coeffs_t re_6_coeffs_nox =
{
    .bit9_offset = RE_6_NOX_BIT9_OFFSET,
    .invalid_val = RE_6_INVALID_NOX,
    .min_val     = RE_6_NOX_MIN,
    .max_val     = RE_6_NOX_MAX,
    .ratio       = RE_6_NOX_RATIO,
};

const re_u9_coeffs_t re_6_coeffs_sound_avg_dba =
{
    .bit9_offset = RE_6_SOUND_AVG_DBA_BIT9_OFFSET,
    .invalid_val = RE_6_INVALID_SOUND,
    .min_val     = RE_6_SOUND_MIN,
    .max_val     = RE_6_SOUND_MAX,
    .ratio       = RE_6_SOUND_RATIO,
};

const re_i16_coeffs_t re_6_coeffs_temperature =
{
    .invalid_val = RE_6_INVALID_TEMPERATURE,
    .max_abs_val = RE_6_TEMPERATURE_MAX,
    .ratio       = RE_6_TEMPERATURE_RATIO,
};

static const re_u16_coeffs_t re_6_coeffs_humidity =
{
    .invalid_val = RE_6_INVALID_HUMIDITY,
    .min_val     = RE_6_HUMIDITY_MIN,
    .max_val     = RE_6_HUMIDITY_MAX,
    .ratio       = RE_6_HUMIDITY_RATIO,
};

static const re_u16_coeffs_t re_6_coeffs_pressure =
{
    .invalid_val = RE_6_INVALID_PRESSURE,
    .min_val     = RE_6_PRESSURE_MIN,
    .max_val     = RE_6_PRESSURE_MAX,
    .ratio       = RE_6_PRESSURE_RATIO,
};

static const re_u16_coeffs_t re_6_coeffs_pm =
{
    .invalid_val = RE_6_INVALID_PM,
    .min_val     = RE_6_PM_MIN,
    .max_val     = RE_6_PM_MAX,
    .ratio       = RE_6_PM_RATIO,
};

static const re_u16_coeffs_t re_6_coeffs_co2 =
{
    .invalid_val = RE_6_INVALID_CO2,
    .min_val     = RE_6_CO2_MIN,
    .max_val     = RE_6_CO2_MAX,
    .ratio       = RE_6_CO2_RATIO,
};

static void
re_6_encode_temperature (uint8_t * const p_slot, const re_float val)
{
    return re_encode_i16 (p_slot, &re_6_coeffs_temperature, val);
}

static re_float
re_6_decode_temperature (const uint8_t * const p_slot)
{
    return re_decode_i16 (p_slot, &re_6_coeffs_temperature);
}

static void
re_6_encode_humidity (uint8_t * const p_slot, const re_float val)
{
    re_encode_u16 (p_slot, &re_6_coeffs_humidity, val);
}

static re_float
re_6_decode_humidity (const uint8_t * const p_slot)
{
    return re_decode_u16 (p_slot, &re_6_coeffs_humidity);
}

static void
re_6_encode_pressure (uint8_t * const p_slot, const re_float val)
{
    re_encode_u16 (p_slot, &re_6_coeffs_pressure, val);
}

static re_float
re_6_decode_pressure (const uint8_t * const p_slot)
{
    return re_decode_u16 (p_slot, &re_6_coeffs_pressure);
}

static void
re_6_encode_pm (uint8_t * const p_slot, const re_float val)
{
    re_encode_u16 (p_slot, &re_6_coeffs_pm, val);
}

static re_float
re_6_decode_pm (const uint8_t * const p_slot)
{
    return re_decode_u16 (p_slot, &re_6_coeffs_pm);
}

static void
re_6_encode_co2 (uint8_t * const p_slot, const re_float val)
{
    re_encode_u16 (p_slot, &re_6_coeffs_co2, val);
}

static re_float
re_6_decode_co2 (const uint8_t * const p_slot)
{
    return re_decode_u16 (p_slot, &re_6_coeffs_co2);
}

static void
re_6_encode_voc (uint8_t * const p_slot, uint8_t * const p_flags, const re_float val)
{
    re_encode_u9 (p_slot, p_flags, &re_6_coeffs_vox, val);
}

static re_float
re_6_decode_voc (const uint8_t * const p_slot, const uint8_t * const p_flags)
{
    return re_decode_u9 (p_slot, p_flags, &re_6_coeffs_vox);
}

static void
re_6_encode_nox (uint8_t * const p_slot, uint8_t * const p_flags, const re_float val)
{
    re_encode_u9 (p_slot, p_flags, &re_6_coeffs_nox, val);
}

static re_float
re_6_decode_nox (const uint8_t * const p_slot, const uint8_t * const p_flags)
{
    return re_decode_u9 (p_slot, p_flags, &re_6_coeffs_nox);
}

static void
re_6_encode_luminosity (uint8_t * const p_slot, re_float val)
{
    uint8_t coded_val = RE_6_INVALID_LUMINOSITY;

    if (!isnan (val))
    {
        const re_float val_clipped = RE_CLIP (val, RE_6_LUMINOSITY_MIN, RE_6_LUMINOSITY_MAX);
        coded_val                  = (uint8_t) lrintf (logf (val_clipped + 1) *
                                     RE_6_LUMINOSITY_RATIO);
    }

    *p_slot = coded_val;
}

static re_float
re_6_decode_luminosity (const uint8_t * const p_slot)
{
    uint8_t coded_val = *p_slot;

    if (RE_6_INVALID_LUMINOSITY == coded_val)
    {
        return NAN;
    }

    return (expf ( (re_float) coded_val / RE_6_LUMINOSITY_RATIO) - 1.0f);
}

static void
re_6_encode_sound_avg_dba (uint8_t * const p_slot, uint8_t * const p_flags, re_float val)
{
    re_encode_u9 (p_slot, p_flags, &re_6_coeffs_sound_avg_dba, val);
}

static re_float
re_6_decode_sound_avg_dba (const uint8_t * const p_slot, const uint8_t * const p_flags)
{
    return re_decode_u9 (p_slot, p_flags, &re_6_coeffs_sound_avg_dba);
}

static void
re_6_encode_seq_cnt2 (uint8_t * const p_slot, const uint8_t seq_cnt2)
{
    *p_slot = seq_cnt2;
}

static uint8_t
re_6_decode_seq_cnt2 (const uint8_t * const p_slot)
{
    return *p_slot;
}

static void
re_6_encode_flags (uint8_t * const p_slot, const re_6_flags_t flags)
{
    if (flags.flag_calibration_in_progress)
    {
        *p_slot |= RE_6_FLAGS_CALIBRATION_IN_PROGRESS;
    }

    if (flags.flag_button_pressed)
    {
        *p_slot |= RE_6_FLAGS_BUTTON_PRESSED;
    }

    if (flags.flag_rtc_running_on_boot)
    {
        *p_slot |= RE_6_FLAGS_RTC_RUNNING_ON_BOOT;
    }
}

static re_6_flags_t
re_6_decode_flags (const uint8_t * const p_flags)
{
    re_6_flags_t flags = { .flag_calibration_in_progress = false,
                           .flag_button_pressed          = false,
                           .flag_rtc_running_on_boot     = false
                         };

    if (0 != (*p_flags & RE_6_FLAGS_CALIBRATION_IN_PROGRESS))
    {
        flags.flag_calibration_in_progress = true;
    }

    if (0 != (*p_flags & RE_6_FLAGS_BUTTON_PRESSED))
    {
        flags.flag_button_pressed = true;
    }

    if (0 != (*p_flags & RE_6_FLAGS_RTC_RUNNING_ON_BOOT))
    {
        flags.flag_rtc_running_on_boot = true;
    }

    return flags;
}

static void
re_6_encode_address (uint8_t * const p_buffer, const re_6_mac_addr_24_t mac_addr)
{
    uint32_t mac_addr_u24 = ( ( (uint32_t) mac_addr.byte3) << RE_BYTE_2_SHIFT)
                            | ( ( (uint32_t) mac_addr.byte4) << RE_BYTE_1_SHIFT)
                            | ( ( (uint32_t) mac_addr.byte5) << RE_BYTE_0_SHIFT);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"

    if (RE_6_MAC_MAX < mac_addr_u24)
    {
        mac_addr_u24 = RE_6_INVALID_MAC;
    }
    else if (RE_6_MAC_MIN > mac_addr_u24) // cppcheck-suppress unsignedLessThanZero
    {
        mac_addr_u24 = RE_6_INVALID_MAC;
    }
    else
    {
        // MISRA C:2012, 15.7 - All if...else if constructs shall be terminated with an else statement
    }

#pragma GCC diagnostic pop
    p_buffer[RE_6_OFFSET_ADDR_3] = (mac_addr_u24 >> RE_BYTE_2_SHIFT) & RE_BYTE_MASK;
    p_buffer[RE_6_OFFSET_ADDR_4] = (mac_addr_u24 >> RE_BYTE_1_SHIFT) & RE_BYTE_MASK;
    p_buffer[RE_6_OFFSET_ADDR_5] = (mac_addr_u24 >> RE_BYTE_0_SHIFT) & RE_BYTE_MASK;
}

static re_6_mac_addr_24_t
re_6_decode_address (const uint8_t * const p_buffer)
{
    const re_6_mac_addr_24_t mac_addr = { .byte3 = p_buffer[RE_6_OFFSET_ADDR_3],
                                          .byte4 = p_buffer[RE_6_OFFSET_ADDR_4],
                                          .byte5 = p_buffer[RE_6_OFFSET_ADDR_5]
                                        };
    return mac_addr;
}

re_status_t
re_6_encode (uint8_t * const p_buffer, const re_6_data_t * const p_data)
{
    re_status_t result = RE_SUCCESS;

    if ( (NULL == p_buffer) || (NULL == p_data))
    {
        result |= RE_ERROR_NULL;
    }
    else
    {
        memset (p_buffer, 0, RE_6_DATA_LENGTH);
        p_buffer[RE_6_OFFSET_HEADER] = RE_6_DESTINATION;
        re_6_encode_temperature (&p_buffer[RE_6_OFFSET_TEMPERATURE_MSB], p_data->temperature_c);
        re_6_encode_humidity (&p_buffer[RE_6_OFFSET_HUMIDITY_MSB], p_data->humidity_rh);
        re_6_encode_pressure (&p_buffer[RE_6_OFFSET_PRESSURE_MSB], p_data->pressure_pa);
        re_6_encode_pm (&p_buffer[RE_6_OFFSET_PM_2_5_MSB], p_data->pm2p5_ppm);
        re_6_encode_co2 (&p_buffer[RE_6_OFFSET_CO2_MSB], p_data->co2);
        re_6_encode_voc (&p_buffer[RE_6_OFFSET_VOC], &p_buffer[RE_6_OFFSET_FLAGS], p_data->voc);
        re_6_encode_nox (&p_buffer[RE_6_OFFSET_NOX], &p_buffer[RE_6_OFFSET_FLAGS], p_data->nox);
        re_6_encode_luminosity (&p_buffer[RE_6_OFFSET_LUMINOSITY], p_data->luminosity);
        re_6_encode_sound_avg_dba (
            &p_buffer[RE_6_OFFSET_SOUND_AVG_DBA],
            &p_buffer[RE_6_OFFSET_FLAGS],
            p_data->sound_avg_dba);
        re_6_encode_seq_cnt2 (&p_buffer[RE_6_OFFSET_SEQ_CNT2], p_data->seq_cnt2);
        re_6_encode_flags (&p_buffer[RE_6_OFFSET_FLAGS], p_data->flags);
        re_6_encode_address (p_buffer, p_data->mac_addr_24);
    }

    return result;
}

bool
re_6_check_format (const uint8_t * const p_buffer)
{
    if (NULL == p_buffer)
    {
        return false;
    }

    if (RE_6_RAW_PACKET_AD1_FLAGS_LEN_VAL != p_buffer[RE_6_RAW_PACKET_AD1_FLAGS_LEN_OFFSET])
    {
        return false;
    }

    if (RE_6_RAW_PACKET_AD1_FLAGS_AD_TYPE_VAL !=
            p_buffer[RE_6_RAW_PACKET_AD1_FLAGS_AD_TYPE_OFFSET])
    {
        return false;
    }

    if (RE_6_RAW_PACKET_AD2_UUID_LEN_VAL != p_buffer[RE_6_RAW_PACKET_AD2_UUID_LEN_OFFSET])
    {
        return false;
    }

    if (RE_6_RAW_PACKET_AD2_UUID_AD_TYPE_VAL !=
            p_buffer[RE_6_RAW_PACKET_AD2_UUID_AD_TYPE_OFFSET])
    {
        return false;
    }

    const uint16_t uuid = (uint16_t) ( (uint16_t)
                                       p_buffer[RE_6_RAW_PACKET_AD2_UUID_AD_DATA2_OFFSET] << RE_BYTE_1_SHIFT)
                          + p_buffer[RE_6_RAW_PACKET_AD2_UUID_AD_DATA1_OFFSET];

    if (RE_6_RAW_PACKET_AD2_UUID_VAL != uuid)
    {
        return false;
    }

    if (RE_6_RAW_PACKET_AD3_MSD_LEN_VAL != p_buffer[RE_6_RAW_PACKET_AD3_MSD_LEN_OFFSET])
    {
        return false;
    }

    if (RE_6_RAW_PACKET_AD3_MSD_AD_TYPE_VAL !=
            p_buffer[RE_6_RAW_PACKET_AD3_MSD_AD_TYPE_OFFSET])
    {
        return false;
    }

    const uint16_t manufacturer_id = (uint16_t) ( (uint16_t)
                                     p_buffer[RE_6_RAW_PACKET_AD3_MSD_MANUFACTURER_ID_OFFSET_HI]
                                     << RE_BYTE_1_SHIFT)
                                     + p_buffer[RE_6_RAW_PACKET_AD3_MSD_MANUFACTURER_ID_OFFSET_LO];

    if (RE_6_RAW_PACKET_AD3_MANUFACTURER_ID_VAL != manufacturer_id)
    {
        return false;
    }

    if (RE_6_DESTINATION != p_buffer[RE_6_OFFSET_PAYLOAD + RE_6_OFFSET_HEADER])
    {
        return false;
    }

    return true;
}

re_status_t
re_6_decode (const uint8_t * const p_buffer, re_6_data_t * const p_data)
{
    const uint8_t * const p_payload = &p_buffer[RE_6_OFFSET_PAYLOAD];
    re_status_t          result    = RE_SUCCESS;

    if ( (NULL == p_payload) || (NULL == p_data))
    {
        return RE_ERROR_NULL;
    }

    memset (p_data, 0, sizeof (*p_data));

    if (RE_6_DESTINATION != p_payload[RE_6_OFFSET_HEADER])
    {
        return RE_ERROR_INVALID_PARAM;
    }

    p_data->temperature_c = re_6_decode_temperature (&p_payload[RE_6_OFFSET_TEMPERATURE_MSB]);
    p_data->humidity_rh   = re_6_decode_humidity (&p_payload[RE_6_OFFSET_HUMIDITY_MSB]);
    p_data->pressure_pa   = re_6_decode_pressure (&p_payload[RE_6_OFFSET_PRESSURE_MSB]);
    p_data->pm2p5_ppm     = re_6_decode_pm (&p_payload[RE_6_OFFSET_PM_2_5_MSB]);
    p_data->co2           = re_6_decode_co2 (&p_payload[RE_6_OFFSET_CO2_MSB]);
    p_data->voc           = re_6_decode_voc (&p_payload[RE_6_OFFSET_VOC],
                            &p_payload[RE_6_OFFSET_FLAGS]);
    p_data->nox           = re_6_decode_nox (&p_payload[RE_6_OFFSET_NOX],
                            &p_payload[RE_6_OFFSET_FLAGS]);
    p_data->luminosity    = re_6_decode_luminosity (&p_payload[RE_6_OFFSET_LUMINOSITY]);
    p_data->sound_avg_dba = re_6_decode_sound_avg_dba (
                                &p_payload[RE_6_OFFSET_SOUND_AVG_DBA],
                                &p_payload[RE_6_OFFSET_FLAGS]);
    p_data->seq_cnt2    = re_6_decode_seq_cnt2 (&p_payload[RE_6_OFFSET_SEQ_CNT2]);
    p_data->flags       = re_6_decode_flags (&p_payload[RE_6_OFFSET_FLAGS]);
    p_data->mac_addr_24 = re_6_decode_address (p_payload);
    return result;
}

re_6_data_t
re_6_data_invalid (const uint16_t measurement_cnt, const uint64_t radio_mac)
{
    const re_6_mac_addr_24_t mac_addr_24 = { .byte3 = (uint8_t) ( (radio_mac >> RE_BYTE_2_SHIFT) & RE_BYTE_MASK),
                                             .byte4 = (uint8_t) ( (radio_mac >> RE_BYTE_1_SHIFT) & RE_BYTE_MASK),
                                             .byte5 = (uint8_t) ( (radio_mac >> RE_BYTE_0_SHIFT) & RE_BYTE_MASK)
                                           };
    const re_6_data_t data =
    {
        .temperature_c     = NAN,
        .humidity_rh       = NAN,
        .pressure_pa       = NAN,
        .pm2p5_ppm         = NAN,
        .co2               = NAN,
        .voc               = NAN,
        .nox               = NAN,
        .luminosity        = NAN,
        .sound_avg_dba     = NAN,
        .seq_cnt2          = measurement_cnt & RE_BYTE_MASK,
        .flags             = {
            .flag_calibration_in_progress = false,
            .flag_button_pressed          = false,
            .flag_rtc_running_on_boot     = false,
        },
        .mac_addr_24       = mac_addr_24,
    };
    return data;
}

#endif
