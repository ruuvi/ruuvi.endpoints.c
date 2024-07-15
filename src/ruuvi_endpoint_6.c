#include "ruuvi_endpoint_6.h"
#include "ruuvi_endpoints.h"
#include <stddef.h>
#include <string.h>
#include <math.h>

#if RE_6_ENABLED

#define RE_6_PM_RATIO            (10.0f)
#define RE_6_CO2_RATIO           (1.0f)
#define RE_6_HUMIDITY_RATIO      (10.0f)
#define RE_6_VOC_RATIO           (1.0f)
#define RE_6_NOX_RATIO           (1.0f)
#define RE_6_TEMPERATURE_RATIO   (10.0f)

#define RE_6_MAC_MAX             (281474976710655)
#define RE_6_MAC_MIN             (0)

#define RE_6_BYTE_0_SHIFT        (0U)
#define RE_6_BYTE_1_SHIFT        (8U)
#define RE_6_BYTE_2_SHIFT        (16U)
#define RE_6_BYTE_3_SHIFT        (24U)
#define RE_6_BYTE_4_SHIFT        (32U)
#define RE_6_BYTE_5_SHIFT        (40U)
#define RE_6_BYTE_MASK           (0xFFU)

#define RE_6_RAW_PACKET_ADV_DATA_TYPE_LEN_OFFSET    (0U)
#define RE_6_RAW_PACKET_ADV_DATA_TYPE_LEN_VAL       (2U)
#define RE_6_RAW_PACKET_ADV_DATA_TYPE_FLAG1_OFFSET  (1U)
#define RE_6_RAW_PACKET_ADV_DATA_TYPE_FLAG1_VAL     (1U)
#define RE_6_RAW_PACKET_ADV_DATA_TYPE_FLAG2_OFFSET  (2U)

#define RE_6_RAW_PACKET_LENGTH_OFFSET               (3U)
#define RE_6_RAW_PACKET_LENGTH_VAL                  (27U)
#define RE_6_RAW_PACKET_TYPE_OFFSET                 (4U)
#define RE_6_RAW_PACKET_TYPE_VAL                    (0xFFU)
#define RE_6_RAW_PACKET_MANUFACTURER_ID_OFFSET_LO   (5U)
#define RE_6_RAW_PACKET_MANUFACTURER_ID_OFFSET_HI   (6U)
#define RE_6_RAW_PACKET_MANUFACTURER_ID_VAL         (0x499U)


// Avoid mocking simple function
#ifdef TEST
void re_clip (re_float * const value, const re_float min, const re_float max)
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

static void re_6_encode_pm (uint8_t * const p_slot, re_float val, const uint32_t ofs)
{
    uint16_t coded_val = RE_6_INVALID_PM;

    if (!isnan (val))
    {
        re_clip (&val, RE_6_PM_MIN, RE_6_PM_MAX);
        coded_val = (uint16_t) lrintf (val * RE_6_PM_RATIO);
    }

    coded_val &= RE_6_OFFSET_PM_MASK;
    coded_val <<= ofs;
    p_slot[0] |= ( ( (uint32_t) coded_val >> RE_6_BYTE_1_SHIFT) & RE_6_BYTE_MASK);
    p_slot[1] |= (coded_val & RE_6_BYTE_MASK);
}

static re_float re_6_decode_pm (const uint8_t * const p_slot, const uint32_t ofs)
{
    uint16_t coded_val = 0;
    coded_val |= p_slot[1] & RE_6_BYTE_MASK;
    coded_val |= ( (uint16_t) p_slot[0] & RE_6_BYTE_MASK) << RE_6_BYTE_1_SHIFT;
    coded_val >>= ofs;
    coded_val &= RE_6_OFFSET_PM_MASK;

    if (RE_6_INVALID_PM == coded_val)
    {
        return NAN;
    }

    return (re_float) coded_val / RE_6_PM_RATIO;
}

static void re_6_encode_co2 (uint8_t * const p_slot, re_float val)
{
    uint16_t coded_val = RE_6_INVALID_CO2;

    if (!isnan (val))
    {
        re_clip (&val, RE_6_CO2_MIN, RE_6_CO2_MAX);
        coded_val = (uint16_t) lrintf (val * RE_6_CO2_RATIO);
    }

    coded_val &= RE_6_OFFSET_CO2_MASK;
    coded_val <<= RE_6_OFFSET_CO2_OFS;
    p_slot[0] |= ( (uint32_t) coded_val >> RE_6_BYTE_1_SHIFT) & RE_6_BYTE_MASK;
    p_slot[1] |= (coded_val & RE_6_BYTE_MASK);
}

static re_float re_6_decode_co2 (const uint8_t * const p_slot)
{
    uint16_t coded_val = 0;
    coded_val |= p_slot[1] & RE_6_BYTE_MASK;
    coded_val |= ( (uint16_t) p_slot[0] & RE_6_BYTE_MASK) << RE_6_BYTE_1_SHIFT;
    coded_val >>= RE_6_OFFSET_CO2_OFS;
    coded_val &= RE_6_OFFSET_CO2_MASK;

    if (RE_6_INVALID_CO2 == coded_val)
    {
        return NAN;
    }

    return (re_float) coded_val / RE_6_CO2_RATIO;
}

static void re_6_encode_humidity (uint8_t * const p_slot, re_float val)
{
    uint16_t coded_val = RE_6_INVALID_HUMIDITY;

    if (!isnan (val))
    {
        re_clip (&val, RE_6_HUMIDITY_MIN, RE_6_HUMIDITY_MAX);
        coded_val = (uint16_t) lrintf (val * RE_6_HUMIDITY_RATIO);
    }

    coded_val &= RE_6_OFFSET_HUMIDITY_MASK;
    coded_val <<= RE_6_OFFSET_HUMIDITY_OFS;
    p_slot[0] |= ( (uint32_t) coded_val >> RE_6_BYTE_1_SHIFT) & RE_6_BYTE_MASK;
    p_slot[1] |= (coded_val & RE_6_BYTE_MASK);
}

static re_float re_6_decode_humidity (const uint8_t * const p_slot)
{
    uint16_t coded_val = 0;
    coded_val |= p_slot[1] & RE_6_BYTE_MASK;
    coded_val |= ( (uint16_t) p_slot[0] & RE_6_BYTE_MASK) << RE_6_BYTE_1_SHIFT;
    coded_val >>= RE_6_OFFSET_HUMIDITY_OFS;
    coded_val &= RE_6_OFFSET_HUMIDITY_MASK;

    if (RE_6_INVALID_HUMIDITY == coded_val)
    {
        return NAN;
    }

    return (re_float) coded_val / RE_6_HUMIDITY_RATIO;
}

static void re_6_encode_voc (uint8_t * const p_slot, re_float val)
{
    uint16_t coded_val = RE_6_INVALID_VOC_INDEX;

    if ( (!isnan (val)) && (val >= RE_6_VOC_INDEX_MIN) && (val <= RE_6_VOC_INDEX_MAX))
    {
        coded_val = (uint16_t) lrintf (val * RE_6_VOC_RATIO);
    }

    coded_val &= RE_6_OFFSET_VOC_INDEX_MASK;
    coded_val <<= RE_6_OFFSET_VOC_INDEX_OFS;
    p_slot[0] |= ( (uint32_t) coded_val >> RE_6_BYTE_1_SHIFT) & RE_6_BYTE_MASK;
    p_slot[1] |= (coded_val & RE_6_BYTE_MASK);
}

static re_float re_6_decode_voc (const uint8_t * const p_slot)
{
    uint16_t coded_val = 0;
    coded_val |= p_slot[1] & RE_6_BYTE_MASK;
    coded_val |= ( (uint16_t) p_slot[0] & RE_6_BYTE_MASK) << RE_6_BYTE_1_SHIFT;
    coded_val >>= RE_6_OFFSET_VOC_INDEX_OFS;
    coded_val &= RE_6_OFFSET_VOC_INDEX_MASK;

    if (RE_6_INVALID_VOC_INDEX == coded_val)
    {
        return NAN;
    }

    return (re_float) coded_val / RE_6_VOC_RATIO;
}

static void re_6_encode_nox (uint8_t * const p_slot, re_float val)
{
    uint16_t coded_val = RE_6_INVALID_NOX_INDEX;

    if ( (!isnan (val)) && (val >= RE_6_NOX_INDEX_MIN) && (val <= RE_6_NOX_INDEX_MAX))
    {
        coded_val = (uint16_t) lrintf (val * RE_6_NOX_RATIO);
    }

    coded_val &= RE_6_OFFSET_NOX_INDEX_MASK;
    coded_val <<= RE_6_OFFSET_NOX_INDEX_OFS;
    p_slot[0] |= ( (uint32_t) coded_val >> RE_6_BYTE_1_SHIFT) & RE_6_BYTE_MASK;
    p_slot[1] |= (coded_val & RE_6_BYTE_MASK);
}

static re_float re_6_decode_nox (const uint8_t * const p_slot)
{
    uint16_t coded_val = 0;
    coded_val |= p_slot[1] & RE_6_BYTE_MASK;
    coded_val |= ( (uint16_t) p_slot[0] & RE_6_BYTE_MASK) << RE_6_BYTE_1_SHIFT;
    coded_val >>= RE_6_OFFSET_NOX_INDEX_OFS;
    coded_val &= RE_6_OFFSET_NOX_INDEX_MASK;

    if (RE_6_INVALID_NOX_INDEX == coded_val)
    {
        return NAN;
    }

    return (re_float) coded_val / RE_6_NOX_RATIO;
}

static void re_6_encode_temperature (uint8_t * const p_slot, re_float val)
{
    uint16_t coded_val = RE_6_INVALID_TEMPERATURE;

    if (!isnan (val))
    {
        re_clip (&val, RE_6_TEMPERATURE_MIN, RE_6_TEMPERATURE_MAX);
        coded_val = (uint16_t) lrintf (val * RE_6_TEMPERATURE_RATIO);
    }

    coded_val &= RE_6_OFFSET_TEMPERATURE_MASK;
    coded_val <<= RE_6_OFFSET_TEMPERATURE_OFS;
    p_slot[0] |= ( (uint32_t) coded_val >> RE_6_BYTE_1_SHIFT) & RE_6_BYTE_MASK;
    p_slot[1] |= (coded_val & RE_6_BYTE_MASK);
}

static re_float re_6_decode_temperature (const uint8_t * const p_slot)
{
    uint16_t coded_val = 0;
    coded_val |= p_slot[1] & RE_6_BYTE_MASK;
    coded_val |= ( (uint16_t) p_slot[0] & RE_6_BYTE_MASK) << RE_6_BYTE_1_SHIFT;
    coded_val >>= RE_6_OFFSET_TEMPERATURE_OFS;
    coded_val &= RE_6_OFFSET_TEMPERATURE_MASK;

    if (RE_6_INVALID_TEMPERATURE == coded_val)
    {
        return NAN;
    }

    if (0 != (coded_val & ( (RE_6_OFFSET_TEMPERATURE_MASK + 1) >> 1U)))
    {
        // it's negative val, we need to set all most significant bits to '1'
        coded_val |= ~RE_6_OFFSET_TEMPERATURE_MASK;
    }

    return (re_float) (int16_t) coded_val / RE_6_TEMPERATURE_RATIO;
}

static void re_6_encode_set_address (uint8_t * const p_buffer, const re_6_data_t * p_data)
{
    // Address is 64 bits, skip 2 first bytes
    uint8_t addr_offset = RE_6_OFFSET_ADDR_MSB;
    uint64_t mac = p_data->address;

    if ( (RE_6_MAC_MAX < p_data->address) || (RE_6_MAC_MIN > p_data->address))
    {
        mac = RE_6_INVALID_MAC;
    }

    p_buffer[addr_offset] = (mac >> RE_6_BYTE_5_SHIFT) & RE_6_BYTE_MASK;
    addr_offset++;
    p_buffer[addr_offset] = (mac >> RE_6_BYTE_4_SHIFT) & RE_6_BYTE_MASK;
    addr_offset++;
    p_buffer[addr_offset] = (mac >> RE_6_BYTE_3_SHIFT) & RE_6_BYTE_MASK;
    addr_offset++;
    p_buffer[addr_offset] = (mac >> RE_6_BYTE_2_SHIFT) & RE_6_BYTE_MASK;
    addr_offset++;
    p_buffer[addr_offset] = (mac >> RE_6_BYTE_1_SHIFT) & RE_6_BYTE_MASK;
    addr_offset++;
    p_buffer[addr_offset] = (mac >> RE_6_BYTE_0_SHIFT) & RE_6_BYTE_MASK;
}

static uint64_t re_6_decode_address (const uint8_t * const p_buffer)
{
    // Address is 64 bits, skip 2 first bytes
    uint8_t addr_offset = RE_6_OFFSET_ADDR_MSB;
    uint64_t mac = 0;
    mac |= p_buffer[addr_offset];
    mac <<= RE_6_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    mac <<= RE_6_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    mac <<= RE_6_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    mac <<= RE_6_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    mac <<= RE_6_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    return mac;
}

static void re_6_encode_sequence (uint8_t * const p_buffer,
                                  const re_6_data_t * const p_data)
{
    uint16_t measurement_seq = RE_6_INVALID_SEQUENCE;

    if (RE_6_SEQ_CTR_MAX >= p_data->measurement_count)
    {
        measurement_seq = p_data->measurement_count;
    }

    p_buffer[RE_6_OFFSET_SEQ_CTR_MSB] = (measurement_seq >> RE_6_BYTE_1_SHIFT);
    p_buffer[RE_6_OFFSET_SEQ_CTR_LSB] = (measurement_seq & RE_6_BYTE_MASK);
}

static uint16_t re_6_decode_sequence (const uint8_t * const p_buffer)
{
    uint16_t measurement_seq = 0;
    measurement_seq |= p_buffer[RE_6_OFFSET_SEQ_CTR_LSB] & RE_6_BYTE_MASK;
    measurement_seq |= (p_buffer[RE_6_OFFSET_SEQ_CTR_MSB] & RE_6_BYTE_MASK) <<
                       RE_6_BYTE_1_SHIFT;
    return measurement_seq;
}

re_status_t re_6_encode (uint8_t * const p_buffer, const re_6_data_t * const p_data)
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
        re_6_encode_pm (&p_buffer[RE_6_OFFSET_PM_1_0_MSB], p_data->pm1p0_ppm,
                        RE_6_OFFSET_PM_1_0_OFS);
        re_6_encode_pm (&p_buffer[RE_6_OFFSET_PM_2_5_MSB], p_data->pm2p5_ppm,
                        RE_6_OFFSET_PM_2_5_OFS);
        re_6_encode_pm (&p_buffer[RE_6_OFFSET_PM_4_0_MSB], p_data->pm4p0_ppm,
                        RE_6_OFFSET_PM_4_0_OFS);
        re_6_encode_pm (&p_buffer[RE_6_OFFSET_PM_10_0_MSB], p_data->pm10p0_ppm,
                        RE_6_OFFSET_PM_10_0_OFS);
        re_6_encode_co2 (&p_buffer[RE_6_OFFSET_CO2_MSB], p_data->co2);
        re_6_encode_humidity (&p_buffer[RE_6_OFFSET_HUMIDITY_MSB], p_data->humidity_rh);
        re_6_encode_voc (&p_buffer[RE_6_OFFSET_VOC_INDEX_MSB], p_data->voc_index);
        re_6_encode_nox (&p_buffer[RE_6_OFFSET_NOX_INDEX_MSB], p_data->nox_index);
        re_6_encode_temperature (&p_buffer[RE_6_OFFSET_TEMPERATURE_MSB], p_data->temperature_c);
        re_6_encode_sequence (p_buffer, p_data);
        re_6_encode_set_address (p_buffer, p_data);
    }

    return result;
}

bool re_6_check_format (const uint8_t * const p_buffer)
{
    if (NULL == p_buffer)
    {
        return false;
    }

    if (RE_6_RAW_PACKET_ADV_DATA_TYPE_LEN_VAL !=
            p_buffer[RE_6_RAW_PACKET_ADV_DATA_TYPE_LEN_OFFSET])
    {
        return false;
    }

    if (RE_6_RAW_PACKET_ADV_DATA_TYPE_FLAG1_VAL !=
            p_buffer[RE_6_RAW_PACKET_ADV_DATA_TYPE_FLAG1_OFFSET])
    {
        return false;
    }

    if (RE_6_RAW_PACKET_LENGTH_VAL != p_buffer[RE_6_RAW_PACKET_LENGTH_OFFSET])
    {
        return false;
    }

    if (RE_6_RAW_PACKET_TYPE_VAL != p_buffer[RE_6_RAW_PACKET_TYPE_OFFSET])
    {
        return false;
    }

    const uint16_t manufacturer_id = (uint16_t) ( (uint16_t)
                                     p_buffer[RE_6_RAW_PACKET_MANUFACTURER_ID_OFFSET_HI] << RE_6_BYTE_1_SHIFT)
                                     + p_buffer[RE_6_RAW_PACKET_MANUFACTURER_ID_OFFSET_LO];

    if (RE_6_RAW_PACKET_MANUFACTURER_ID_VAL != manufacturer_id)
    {
        return false;
    }

    if (RE_6_DESTINATION != p_buffer[RE_6_OFFSET_PAYLOAD + RE_6_OFFSET_HEADER])
    {
        return false;
    }

    return true;
}

re_status_t re_6_decode (const uint8_t * const p_buffer, re_6_data_t * const p_data)
{
    const uint8_t * const p_payload = &p_buffer[RE_6_OFFSET_PAYLOAD];
    re_status_t result = RE_SUCCESS;

    if ( (NULL == p_payload) || (NULL == p_data))
    {
        return RE_ERROR_NULL;
    }

    memset (p_data, 0, sizeof (*p_data));

    if (RE_6_DESTINATION != p_payload[RE_6_OFFSET_HEADER])
    {
        return RE_ERROR_INVALID_PARAM;
    }

    p_data->pm1p0_ppm = re_6_decode_pm (&p_payload[RE_6_OFFSET_PM_1_0_MSB],
                                        RE_6_OFFSET_PM_1_0_OFS);
    p_data->pm2p5_ppm = re_6_decode_pm (&p_payload[RE_6_OFFSET_PM_2_5_MSB],
                                        RE_6_OFFSET_PM_2_5_OFS);
    p_data->pm4p0_ppm = re_6_decode_pm (&p_payload[RE_6_OFFSET_PM_4_0_MSB],
                                        RE_6_OFFSET_PM_4_0_OFS);
    p_data->pm10p0_ppm = re_6_decode_pm (&p_payload[RE_6_OFFSET_PM_10_0_MSB],
                                         RE_6_OFFSET_PM_10_0_OFS);
    p_data->co2 = re_6_decode_co2 (&p_payload[RE_6_OFFSET_CO2_MSB]);
    p_data->humidity_rh = re_6_decode_humidity (&p_payload[RE_6_OFFSET_HUMIDITY_MSB]);
    p_data->voc_index = re_6_decode_voc (&p_payload[RE_6_OFFSET_VOC_INDEX_MSB]);
    p_data->nox_index = re_6_decode_nox (&p_payload[RE_6_OFFSET_NOX_INDEX_MSB]);
    p_data->temperature_c = re_6_decode_temperature (&p_payload[RE_6_OFFSET_TEMPERATURE_MSB]);
    p_data->measurement_count = re_6_decode_sequence (p_payload);
    p_data->address = re_6_decode_address (p_payload);
    return result;
}

#endif
