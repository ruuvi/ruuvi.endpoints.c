#include "ruuvi_endpoint_5.h"
#include "ruuvi_endpoints.h"
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#if RE_5_ENABLED

#define RE_5_ACC_RATIO           (1000.0f)
#define RE_5_HUMI_RATIO          (400.0f)
#define RE_5_TEMP_RATIO          (200.0f)
#define RE_5_PRES_RATIO          (1.0f)
#define RE_5_PRES_OFFSET         (-50000.0f)
#define RE_5_BATT_RATIO          (1000.0f)
#define RE_5_BATT_OFFSET         (1600)
#define RE_5_BATT_MIN            (1.6f)

#define RE_5_TXPWR_RATIO         (2)
#define RE_5_TXPWR_OFFSET        (40)

#define RE_5_MAC_MAX             (281474976710655)
#define RE_5_MAC_MIN             (0)

#define RE_5_BYTE_0_SHIFT        (0U)
#define RE_5_BYTE_1_SHIFT        (8U)
#define RE_5_BYTE_2_SHIFT        (16U)
#define RE_5_BYTE_3_SHIFT        (24U)
#define RE_5_BYTE_4_SHIFT        (32U)
#define RE_5_BYTE_5_SHIFT        (40U)
#define RE_5_BYTE_MASK           (0xFFU)
#define RE_5_BYTE_VOLTAGE_OFFSET (5U)
#define RE_5_BYTE_VOLTAGE_MASK   (0x7FFU)
#define RE_5_BYTE_TX_POWER_OFFSET (0U)
#define RE_5_BYTE_TX_POWER_MASK   (0x1FU)

#define RE_5_RAW_PACKET_ADV_DATA_TYPE_LEN_OFFSET    (0U)
#define RE_5_RAW_PACKET_ADV_DATA_TYPE_LEN_VAL       (2U)
#define RE_5_RAW_PACKET_ADV_DATA_TYPE_FLAG1_OFFSET  (1U)
#define RE_5_RAW_PACKET_ADV_DATA_TYPE_FLAG1_VAL     (1U)
#define RE_5_RAW_PACKET_ADV_DATA_TYPE_FLAG2_OFFSET  (2U)

#define RE_5_RAW_PACKET_LENGTH_OFFSET               (3U)
#define RE_5_RAW_PACKET_LENGTH_VAL                  (27U)
#define RE_5_RAW_PACKET_TYPE_OFFSET                 (4U)
#define RE_5_RAW_PACKET_TYPE_VAL                    (0xFFU)
#define RE_5_RAW_PACKET_MANUFACTURER_ID_OFFSET_LO   (5U)
#define RE_5_RAW_PACKET_MANUFACTURER_ID_OFFSET_HI   (6U)
#define RE_5_RAW_PACKET_MANUFACTURER_ID_VAL         (0x499U)

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

static void re_5_encode_acceleration (uint8_t * const acceleration_slot,
                                      re_float acceleration)
{
    uint16_t coded_acceleration = RE_5_INVALID_ACCELERATION;

    if (!isnan (acceleration))
    {
        re_clip (&acceleration, RE_5_ACC_MIN, RE_5_ACC_MAX);
        int16_t rounded_acceleration = (int16_t) roundf (acceleration * RE_5_ACC_RATIO);
        // Type cast adds 2^16 to a negative signed value, not changing bits.
        coded_acceleration = (uint16_t) rounded_acceleration;
    }

    acceleration_slot[0] = (coded_acceleration >> RE_5_BYTE_1_SHIFT);
    acceleration_slot[1] = (coded_acceleration & RE_5_BYTE_MASK);
}

static re_float re_5_decode_acceleration (const uint8_t * const acceleration_slot)
{
    uint16_t coded_acceleration = 0;
    coded_acceleration |= acceleration_slot[1] & RE_5_BYTE_MASK;
    coded_acceleration |= ( (uint16_t) acceleration_slot[0] & RE_5_BYTE_MASK) <<
                          RE_5_BYTE_1_SHIFT;

    if (RE_5_INVALID_ACCELERATION == coded_acceleration)
    {
        return NAN;
    }

    return (re_float) (int16_t) coded_acceleration / RE_5_ACC_RATIO;
}

static void re_5_encode_set_address (uint8_t * const buffer, const re_5_data_t * data)
{
    // Address is 64 bits, skip 2 first bytes
    uint8_t addr_offset = RE_5_OFFSET_ADDR_MSB;
    uint64_t mac = data->address;

    if ( (RE_5_MAC_MAX < data->address) || (RE_5_MAC_MIN > data->address))
    {
        mac = RE_5_INVALID_MAC;
    }

    buffer[addr_offset] = (mac >> RE_5_BYTE_5_SHIFT) & RE_5_BYTE_MASK;
    addr_offset++;
    buffer[addr_offset] = (mac >> RE_5_BYTE_4_SHIFT) & RE_5_BYTE_MASK;
    addr_offset++;
    buffer[addr_offset] = (mac >> RE_5_BYTE_3_SHIFT) & RE_5_BYTE_MASK;
    addr_offset++;
    buffer[addr_offset] = (mac >> RE_5_BYTE_2_SHIFT) & RE_5_BYTE_MASK;
    addr_offset++;
    buffer[addr_offset] = (mac >> RE_5_BYTE_1_SHIFT) & RE_5_BYTE_MASK;
    addr_offset++;
    buffer[addr_offset] = (mac >> 0) & RE_5_BYTE_MASK;
}

static uint64_t re_5_decode_address (const uint8_t * const p_buffer)
{
    // Address is 64 bits, skip 2 first bytes
    uint8_t addr_offset = RE_5_OFFSET_ADDR_MSB;
    uint64_t mac = 0;
    mac |= p_buffer[addr_offset];
    mac <<= RE_5_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    mac <<= RE_5_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    mac <<= RE_5_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    mac <<= RE_5_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    mac <<= RE_5_BYTE_1_SHIFT;
    addr_offset++;
    mac |= p_buffer[addr_offset];
    return mac;
}

static void re_5_encode_humidity (uint8_t * const buffer, const re_5_data_t * data)
{
    uint16_t coded_humidity = RE_5_INVALID_HUMIDITY;
    re_float humidity = data->humidity_rh;

    if (!isnan (humidity))
    {
        re_clip (&humidity, RE_5_HUMI_MIN, RE_5_HUMI_MAX);
        coded_humidity = (uint16_t) roundf (humidity * RE_5_HUMI_RATIO);
    }

    buffer[RE_5_OFFSET_HUMI_MSB] = coded_humidity >> RE_5_BYTE_1_SHIFT;
    buffer[RE_5_OFFSET_HUMI_LSB] = coded_humidity & RE_5_BYTE_MASK;
}

static re_float re_5_decode_humidity (const uint8_t * const buffer)
{
    uint16_t coded_humidity = 0;
    coded_humidity |= buffer[RE_5_OFFSET_HUMI_LSB] & RE_5_BYTE_MASK;
    coded_humidity |= ( (uint16_t) buffer[RE_5_OFFSET_HUMI_MSB] & RE_5_BYTE_MASK) <<
                      RE_5_BYTE_1_SHIFT;

    if (RE_5_INVALID_HUMIDITY == coded_humidity)
    {
        return NAN;
    }

    return (re_float) coded_humidity / RE_5_HUMI_RATIO;
}

static void re_5_encode_temperature (uint8_t * const buffer, const re_5_data_t * data)
{
    uint16_t coded_temperature = RE_5_INVALID_TEMPERATURE;
    re_float temperature = data->temperature_c;

    if (!isnan (temperature))
    {
        re_clip (&temperature, RE_5_TEMP_MIN, RE_5_TEMP_MAX);
        int16_t rounded_temperature = (int16_t) roundf (temperature * RE_5_TEMP_RATIO);
        // Type cast adds 2^16 to a negative signed value, not changing bits.
        coded_temperature = (uint16_t) rounded_temperature;
    }

    buffer[RE_5_OFFSET_TEMP_MSB] = coded_temperature >> RE_5_BYTE_1_SHIFT;
    buffer[RE_5_OFFSET_TEMP_LSB] = coded_temperature & RE_5_BYTE_MASK;
}

static re_float re_5_decode_temperature (const uint8_t * const buffer)
{
    uint16_t coded_temperature = 0;
    coded_temperature |= buffer[RE_5_OFFSET_TEMP_LSB] & RE_5_BYTE_MASK;
    coded_temperature |= ( (uint16_t) buffer[RE_5_OFFSET_TEMP_MSB] & RE_5_BYTE_MASK) <<
                         RE_5_BYTE_1_SHIFT;

    if (RE_5_INVALID_TEMPERATURE == coded_temperature)
    {
        return NAN;
    }

    return (re_float) (int16_t) coded_temperature / RE_5_TEMP_RATIO;
}

static void re_5_encode_pressure (uint8_t * const buffer, const re_5_data_t * data)
{
    uint16_t coded_pressure = RE_5_INVALID_PRESSURE;
    re_float pressure = data->pressure_pa;

    if (!isnan (pressure))
    {
        re_clip (&pressure, RE_5_PRES_MIN, RE_5_PRES_MAX);
        pressure += RE_5_PRES_OFFSET;
        coded_pressure = (uint16_t) roundf (pressure * RE_5_PRES_RATIO);
    }

    buffer[RE_5_OFFSET_PRES_MSB] = coded_pressure >> RE_5_BYTE_1_SHIFT;
    buffer[RE_5_OFFSET_PRES_LSB] = coded_pressure & RE_5_BYTE_MASK;
}

static re_float re_5_decode_pressure (const uint8_t * const buffer)
{
    uint16_t coded_pressure = 0;
    coded_pressure |= buffer[RE_5_OFFSET_PRES_LSB] & RE_5_BYTE_MASK;
    coded_pressure |= ( (uint16_t) buffer[RE_5_OFFSET_PRES_MSB] & RE_5_BYTE_MASK) <<
                      RE_5_BYTE_1_SHIFT;

    if (RE_5_INVALID_PRESSURE == coded_pressure)
    {
        return NAN;
    }

    return ( (re_float) coded_pressure - RE_5_PRES_OFFSET) / RE_5_PRES_RATIO;
}

static void re_5_encode_pwr (uint8_t * const buffer, const re_5_data_t * data)
{
    uint16_t coded_voltage = RE_5_INVALID_VOLTAGE;
    re_float voltage = data->battery_v;
    uint16_t coded_tx_power = RE_5_INVALID_POWER;
    re_float tx_power = (re_float) data->tx_power;

    if (!isnan (voltage))
    {
        re_clip (&voltage, RE_5_VOLTAGE_MIN, RE_5_VOLTAGE_MAX);
        coded_voltage = (uint16_t) roundf ( (voltage * RE_5_BATT_RATIO)
                                            - RE_5_BATT_OFFSET);
    }

    // Check against original int value
    if (RE_5_INVALID_POWER != data->tx_power)
    {
        re_clip (&tx_power, RE_5_TXPWR_MIN, RE_5_TXPWR_MAX);
        coded_tx_power = (uint16_t) roundf ( (tx_power
                                              + RE_5_TXPWR_OFFSET)
                                             / RE_5_TXPWR_RATIO);
    }

    uint16_t power_info = ( (uint16_t) (coded_voltage << RE_5_BYTE_VOLTAGE_OFFSET))
                          + coded_tx_power;
    buffer[RE_5_OFFSET_POWER_MSB] = (power_info >> RE_5_BYTE_1_SHIFT);
    buffer[RE_5_OFFSET_POWER_LSB] = (power_info & RE_5_BYTE_MASK);
}

static void re_5_decode_pwr (const uint8_t * const buffer, re_float * const p_battery_v,
                             int8_t * const p_tx_power)
{
    uint16_t power_info = 0;
    power_info |= buffer[RE_5_OFFSET_POWER_LSB] & RE_5_BYTE_MASK;
    power_info |= ( (uint16_t) buffer[RE_5_OFFSET_POWER_MSB] & RE_5_BYTE_MASK) <<
                  RE_5_BYTE_1_SHIFT;
    uint16_t coded_voltage = ( (uint32_t) power_info >> RE_5_BYTE_VOLTAGE_OFFSET) &
                             RE_5_BYTE_VOLTAGE_MASK;
    uint16_t coded_tx_power = ( (uint32_t) power_info >> RE_5_BYTE_TX_POWER_OFFSET) &
                              RE_5_BYTE_TX_POWER_MASK;

    if (RE_5_INVALID_VOLTAGE == coded_voltage)
    {
        *p_battery_v = NAN;
    }
    else
    {
        *p_battery_v = (re_float) (RE_5_BATT_OFFSET + coded_voltage) / RE_5_BATT_RATIO;
    }

    if (RE_5_INVALID_POWER == coded_tx_power)
    {
        *p_tx_power = RE_5_INVALID_POWER;
    }
    else
    {
        *p_tx_power = (int8_t) lrintf ( (re_float) coded_tx_power * RE_5_TXPWR_RATIO -
                                        RE_5_TXPWR_OFFSET);
    }
}

static void re_5_encode_movement (uint8_t * const buffer, const re_5_data_t * data)
{
    uint8_t movement_count = RE_5_INVALID_MOVEMENT;

    if (RE_5_MVTCTR_MAX >= data->movement_count)
    {
        movement_count = data->movement_count;
    }

    buffer[RE_5_OFFSET_MVTCTR] = movement_count;
}

static uint8_t re_5_decode_movement (const uint8_t * const buffer)
{
    const uint8_t movement_count = buffer[RE_5_OFFSET_MVTCTR];
    return movement_count;
}

static void re_5_encode_sequence (uint8_t * const buffer, const re_5_data_t * data)
{
    uint16_t measurement_seq = RE_5_INVALID_SEQUENCE;

    if (RE_5_SEQCTR_MAX >= data->measurement_count)
    {
        measurement_seq = data->measurement_count;
    }

    buffer[RE_5_OFFSET_SEQCTR_MSB] = (measurement_seq >> RE_5_BYTE_1_SHIFT);
    buffer[RE_5_OFFSET_SEQCTR_LSB] = (measurement_seq & RE_5_BYTE_MASK);
}

static uint16_t re_5_decode_sequence (const uint8_t * const p_buffer)
{
    uint16_t measurement_seq = 0;
    measurement_seq |= p_buffer[RE_5_OFFSET_SEQCTR_LSB] & RE_5_BYTE_MASK;
    measurement_seq |= (p_buffer[RE_5_OFFSET_SEQCTR_MSB] & RE_5_BYTE_MASK) <<
                       RE_5_BYTE_1_SHIFT;
    return measurement_seq;
}

re_status_t re_5_encode (uint8_t * const buffer, const re_5_data_t * data)
{
    re_status_t result = RE_SUCCESS;

    if ( (NULL == buffer) || (NULL == data))
    {
        result |= RE_ERROR_NULL;
    }
    else
    {
        buffer[RE_5_OFFSET_HEADER] = RE_5_DESTINATION;
        re_5_encode_humidity (buffer, data);
        re_5_encode_temperature (buffer, data);
        re_5_encode_pressure (buffer, data);
        re_5_encode_acceleration (&buffer[RE_5_OFFSET_ACCX_MSB], data->accelerationx_g);
        re_5_encode_acceleration (&buffer[RE_5_OFFSET_ACCY_MSB], data->accelerationy_g);
        re_5_encode_acceleration (&buffer[RE_5_OFFSET_ACCZ_MSB], data->accelerationz_g);
        re_5_encode_movement (buffer, data);
        re_5_encode_sequence (buffer, data);
        re_5_encode_pwr (buffer, data);
        re_5_encode_set_address (buffer, data);
    }

    return result;
}

bool re_5_check_format (const uint8_t * const p_buffer)
{
    if (NULL == p_buffer)
    {
        return false;
    }

    if (RE_5_RAW_PACKET_ADV_DATA_TYPE_LEN_VAL !=
            p_buffer[RE_5_RAW_PACKET_ADV_DATA_TYPE_LEN_OFFSET])
    {
        return false;
    }

    if (RE_5_RAW_PACKET_ADV_DATA_TYPE_FLAG1_VAL !=
            p_buffer[RE_5_RAW_PACKET_ADV_DATA_TYPE_FLAG1_OFFSET])
    {
        return false;
    }

    if (RE_5_RAW_PACKET_LENGTH_VAL != p_buffer[RE_5_RAW_PACKET_LENGTH_OFFSET])
    {
        return false;
    }

    if (RE_5_RAW_PACKET_TYPE_VAL != p_buffer[RE_5_RAW_PACKET_TYPE_OFFSET])
    {
        return false;
    }

    const uint16_t manufacturer_id = (uint16_t) ( (uint16_t)
                                     p_buffer[RE_5_RAW_PACKET_MANUFACTURER_ID_OFFSET_HI] << RE_5_BYTE_1_SHIFT)
                                     + p_buffer[RE_5_RAW_PACKET_MANUFACTURER_ID_OFFSET_LO];

    if (RE_5_RAW_PACKET_MANUFACTURER_ID_VAL != manufacturer_id)
    {
        return false;
    }

    if (RE_5_DESTINATION != p_buffer[RE_5_OFFSET_PAYLOAD + RE_5_OFFSET_HEADER])
    {
        return false;
    }

    return true;
}

re_status_t re_5_decode (const uint8_t * const p_buffer, re_5_data_t * const p_data)
{
    const uint8_t * const p_payload = &p_buffer[RE_5_OFFSET_PAYLOAD];
    re_status_t result = RE_SUCCESS;

    if ( (NULL == p_payload) || (NULL == p_data))
    {
        return RE_ERROR_NULL;
    }

    memset (p_data, 0, sizeof (*p_data));

    if (RE_5_DESTINATION != p_payload[RE_5_OFFSET_HEADER])
    {
        return RE_ERROR_INVALID_PARAM;
    }

    p_data->humidity_rh = re_5_decode_humidity (p_payload);
    p_data->temperature_c = re_5_decode_temperature (p_payload);
    p_data->pressure_pa = re_5_decode_pressure (p_payload);
    p_data->accelerationx_g = re_5_decode_acceleration (&p_payload[RE_5_OFFSET_ACCX_MSB]);
    p_data->accelerationy_g = re_5_decode_acceleration (&p_payload[RE_5_OFFSET_ACCY_MSB]);
    p_data->accelerationz_g = re_5_decode_acceleration (&p_payload[RE_5_OFFSET_ACCZ_MSB]);
    p_data->movement_count = re_5_decode_movement (p_payload);
    p_data->measurement_count = re_5_decode_sequence (p_payload);
    re_5_decode_pwr (p_payload, &p_data->battery_v, &p_data->tx_power);
    p_data->address = re_5_decode_address (p_payload);
    return result;
}

#endif
