#include "ruuvi_endpoints.h"
#include "ruuvi_endpoint_ca_uart.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#if RE_CA_ENABLED

#define U16_LSB_MASK    0x00FF
#define U16_MSB_MASK    0xFF00
#define U16_MSB_OFFSET  8
#define U8_HALF_OFFSET  4
#define CMD_ACK_MASK    0x000000ff
#define I8_MIN (-128)
#define I8_MAX (127U)
#define I8_MIN (-128)
#define U8_OVERFLOW (256)

/** @brief serialise up to U64 into given buffer, MSB first. */
static inline void u64_to_array (const uint64_t u64,
                                 uint8_t * const array,
                                 uint8_t bytes)
{
    const uint8_t offset = bytes - 1;

    while (bytes--)
    {
        array[offset - bytes] = (u64 >> (8U * bytes)) & 0xFFU;
    }
}

/** @brief serialise given buffer to u64, MSB first. */
static inline uint64_t array_to_u64 (const uint8_t * const array, uint8_t bytes)
{
    const uint8_t offset = bytes - 1;
    uint64_t rvalue = 0;

    while (bytes--)
    {
        rvalue += (uint64_t) array[bytes] << ( (offset - bytes) * 8U);
    }

    return rvalue;
}

/** @brief Function for calculating CRC-16 in blocks. Conforms to CRC-CCITT (0xFFFF)*/
static uint16_t calculate_crc16 (const uint8_t * p_data,
                                 const uint32_t size,
                                 const uint16_t * p_crc)
{
    uint32_t i;
    uint16_t crc = RE_CA_CRC_DEFAULT;

    if ( (p_data == NULL)
            || (size == 0))
    {
        crc = RE_CA_CRC_INVALID;
    }
    else
    {
        if (p_crc != NULL)
        {
            crc = *p_crc;
        }

        for (i = 0; i < size; i++)
        {
            crc  = (unsigned char) (crc >> U16_MSB_OFFSET) | (crc << U16_MSB_OFFSET);
            crc ^= p_data[i];
            crc ^= (unsigned char) (crc & U16_LSB_MASK) >> U8_HALF_OFFSET;
            crc ^= (crc << U16_MSB_OFFSET) << U8_HALF_OFFSET;
            crc ^= ( (crc & U16_LSB_MASK) << U8_HALF_OFFSET) << 1;
        }
    }

    return crc;
}

/** @brief Function to add crc to buffer */
static void add_crc16 (uint8_t * const buffer,
                       uint32_t * written)
{
    uint16_t crc16 = RE_CA_CRC_INVALID;
    uint16_t p_crc = RE_CA_CRC_DEFAULT;
    crc16 = calculate_crc16 (buffer + RE_CA_UART_STX_ETX_LEN,
                             (*written) - RE_CA_UART_STX_ETX_LEN, &p_crc);

    if (crc16 != RE_CA_CRC_INVALID)
    {
        buffer[ (*written)++] = (uint8_t) (crc16 & U16_LSB_MASK);
        buffer[ (*written)++] = (uint8_t) ( (crc16 & U16_MSB_MASK) >> U16_MSB_OFFSET);
    }
}

/** @brief Function to check crc16 */
static bool check_crc (const uint8_t * const buffer,
                       const uint32_t written)
{
    uint16_t crc16 = RE_CA_CRC_INVALID;
    uint16_t p_crc = RE_CA_CRC_DEFAULT;
    uint16_t in_crc = * ( (uint16_t *) &buffer[written]);
    crc16 = calculate_crc16 (buffer + RE_CA_UART_STX_ETX_LEN,
                             written - RE_CA_UART_STX_ETX_LEN, &p_crc);

    if (in_crc != crc16)
    {
        return false;
    }

    return true;
}

static inline int8_t u8toi8 (const uint8_t byte)
{
    int16_t rval = byte;

    if (byte > I8_MAX)
    {
        rval -= U8_OVERFLOW;
    }

    return rval;
}

static inline uint8_t i8tou8 (const int8_t byte)
{
    uint8_t rval = 0;

    if (0 > byte)
    {
        rval = (uint8_t) (U8_OVERFLOW + byte);
    }
    else
    {
        rval = (uint8_t) (byte);
    }

    return rval;
}
#ifdef RE_CA_SUPPORT_OLD_CMD
static re_status_t re_ca_uart_decode_set_fltr (const uint8_t * const buffer,
        re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;

    if (buffer[RE_CA_UART_LEN_INDEX] != RE_CA_UART_CMD_SFLTR_LEN)
    {
        err_code |= RE_ERROR_DECODING;
    }
    else
    {
        payload->cmd = RE_CA_UART_SET_FLTR;
        // Filter is LSB first.
        uint16_t filter = (uint16_t) (buffer[RE_CA_UART_PAYLOAD_INDEX])
                          + (uint16_t) (buffer[RE_CA_UART_PAYLOAD_INDEX + 1] * 256U);
        payload->params.filter.manufacturer_id = filter;
    }

    return err_code;
}

static re_status_t re_ca_uart_decode_clr_fltr (const uint8_t * const buffer,
        re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;

    if (buffer[RE_CA_UART_LEN_INDEX] != RE_CA_UART_CMD_CFLTR_LEN)
    {
        err_code |= RE_ERROR_DECODING;
    }
    else
    {
        payload->cmd = RE_CA_UART_CLR_FLTR;
        payload->params.filter.manufacturer_id = 0;
    }

    return err_code;
}

static re_status_t re_ca_uart_decode_set_ch (const uint8_t * const buffer,
        re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;

    if (buffer[RE_CA_UART_LEN_INDEX] != RE_CA_UART_CMD_CH_LEN)
    {
        err_code |= RE_ERROR_DECODING;
    }
    else
    {
        payload->cmd = RE_CA_UART_SET_CH;
        payload->params.channels.ch39 =
            (buffer[RE_CA_UART_PAYLOAD_INDEX + RE_CA_UART_CH39_BYTE]
             >> RE_CA_UART_CH39_BIT) & 1U;
        payload->params.channels.ch38 =
            (buffer[RE_CA_UART_PAYLOAD_INDEX + RE_CA_UART_CH38_BYTE]
             >> RE_CA_UART_CH38_BIT) & 1U;
        payload->params.channels.ch37 =
            (buffer[RE_CA_UART_PAYLOAD_INDEX + RE_CA_UART_CH37_BYTE]
             >> RE_CA_UART_CH37_BIT) & 1U;
    }

    return err_code;
}

static re_status_t re_ca_uart_decode_set_phy (const uint8_t * const buffer,
        re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;

    if (buffer[RE_CA_UART_LEN_INDEX] != RE_CA_UART_CMD_PHY_LEN)
    {
        err_code |= RE_ERROR_DECODING;
    }
    else
    {
        payload->cmd = RE_CA_UART_SET_PHY;
        payload->params.phys.ble_125kbps =
            (buffer[RE_CA_UART_PAYLOAD_INDEX] >> RE_CA_UART_125KBPS_BIT) & 1U;
        payload->params.phys.ble_1mbps =
            (buffer[RE_CA_UART_PAYLOAD_INDEX] >> RE_CA_UART_1MBPS_BIT) & 1U;
        payload->params.phys.ble_2mbps =
            (buffer[RE_CA_UART_PAYLOAD_INDEX] >> RE_CA_UART_2MBPS_BIT) & 1U;
    }

    return err_code;
}
#endif

static re_status_t re_ca_uart_decode_bool_payload (const uint8_t * const buffer,
        re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;

    if (buffer[RE_CA_UART_LEN_INDEX] != RE_CA_UART_TX_DATA_LEN_CMD_BOOL())
    {
        err_code |= RE_ERROR_DECODING_LEN;
    }
    else
    {
        payload->cmd = buffer[RE_CA_UART_CMD_INDEX];
        payload->params.bool_param.state =
            ( (* ( (uint8_t *) &buffer[RE_CA_UART_PAYLOAD_INDEX])
               >> RE_CA_UART_BOOL_BIT) & 1U);
    }

    return err_code;
}

static re_status_t re_ca_uart_decode_fltr_id (const uint8_t * const buffer,
        re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;

    if (buffer[RE_CA_UART_LEN_INDEX] != RE_CA_UART_TX_DATA_LEN_CMD_FLTR_ID())
    {
        err_code |= RE_ERROR_DECODING_LEN;
    }
    else
    {
        payload->cmd = buffer[RE_CA_UART_CMD_INDEX];
        payload->params.fltr_id_param.id = * ( (uint16_t *) &buffer[RE_CA_UART_PAYLOAD_INDEX]);
    }

    return err_code;
}

static re_status_t re_ca_uart_decode_led_ctrl (const uint8_t * const buffer,
        re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;

    if (buffer[RE_CA_UART_LEN_INDEX] != RE_CA_UART_TX_DATA_LEN_CMD_LED_CTRL())
    {
        err_code |= RE_ERROR_DECODING_LEN;
    }
    else
    {
        payload->cmd = buffer[RE_CA_UART_CMD_INDEX];
        payload->params.led_ctrl_param.time_interval_ms = * ( (uint16_t *)
                &buffer[RE_CA_UART_PAYLOAD_INDEX]);
    }

    return err_code;
}

static re_status_t re_ca_uart_decode_ack (const uint8_t * const buffer,
        re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;

    if (buffer[RE_CA_UART_LEN_INDEX] != RE_CA_UART_TX_DATA_LEN_CMD_ACK())
    {
        err_code |= RE_ERROR_DECODING_LEN;
    }
    else
    {
        payload->cmd = buffer[RE_CA_UART_CMD_INDEX];
        payload->params.ack.cmd = (* ( (re_ca_uart_cmd_t *) &buffer[RE_CA_UART_PAYLOAD_INDEX])) &
                                  CMD_ACK_MASK;
        payload->params.ack.ack_state.state =
            ( (* ( (uint8_t *) &buffer[RE_CA_UART_PAYLOAD_INDEX
                                       + RE_CA_UART_DELIMITER_LEN
                                       + RE_CA_UART_ACK_CMD_BYTE]) >> RE_CA_UART_ACK_BIT) & 1U);
    }

    return err_code;
}

static re_status_t re_ca_uart_decode_device_id (const uint8_t * const buffer,
        re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;

    if (buffer[RE_CA_UART_LEN_INDEX] != RE_CA_UART_TX_DATA_LEN_CMD_DEVICE_ID())
    {
        err_code |= RE_ERROR_DECODING_LEN;
    }
    else
    {
        payload->cmd = buffer[RE_CA_UART_CMD_INDEX];
        payload->params.device_id.id = array_to_u64 (buffer + RE_CA_UART_PAYLOAD_INDEX,
                                       RE_CA_UART_DEVICE_ID_LEN);
        payload->params.device_id.addr = array_to_u64 (buffer + RE_CA_UART_PAYLOAD_INDEX
                                         + RE_CA_UART_DELIMITER_LEN
                                         + RE_CA_UART_DEVICE_ID_LEN,
                                         RE_CA_UART_DEVICE_ADDR_LEN);
    }

    return err_code;
}

static re_status_t re_ca_uart_decode_get_device_id (const uint8_t * const buffer,
        re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;

    if (buffer[RE_CA_UART_LEN_INDEX] != RE_CA_UART_TX_DATA_LEN_CMD_GET_DEVICE_ID())
    {
        err_code |= RE_ERROR_DECODING_LEN;
    }
    else
    {
        payload->cmd = buffer[RE_CA_UART_CMD_INDEX];
    }

    return err_code;
}

static re_status_t re_ca_uart_decode_get_all (const uint8_t * const buffer,
        re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;

    if (buffer[RE_CA_UART_LEN_INDEX] != RE_CA_UART_TX_DATA_LEN_CMD_GET_ALL_PARAMS())
    {
        err_code |= RE_ERROR_DECODING_LEN;
    }
    else
    {
        payload->cmd = buffer[RE_CA_UART_CMD_INDEX];
    }

    return err_code;
}

static re_status_t re_ca_uart_decode_set_all_params (
    const uint8_t * const buffer,
    re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;
    const uint8_t len = buffer[RE_CA_UART_LEN_INDEX];

    if ( (len != RE_CA_UART_TX_DATA_LEN_CMD_ALL_PARAMS()) &&
            (len != RE_CA_UART_TX_DATA_LEN_CMD_ALL_PARAMS_WITH_MAX_ADV_LEN()))
    {
        err_code |= RE_ERROR_DECODING_LEN;
    }
    else
    {
        payload->cmd = buffer[RE_CA_UART_CMD_INDEX];
        payload->params.all_params.fltr_id.id = * ( (uint16_t *)
                                                &buffer[RE_CA_UART_PAYLOAD_INDEX]);
        const uint8_t flags = buffer[RE_CA_UART_PAYLOAD_INDEX
                                     + RE_CA_UART_DELIMITER_LEN
                                     + RE_CA_UART_CMD_FLTR_ID_LEN];
        payload->params.all_params.bools.fltr_tags.state =
            (flags >> RE_CA_UART_ALL_FLTR_TAG_BIT) & 1U;
        payload->params.all_params.bools.use_coded_phy.state =
            (flags >> RE_CA_UART_ALL_USE_CODED_PHY_BIT) & 1U;
        payload->params.all_params.bools.use_1m_phy.state =
            (flags >> RE_CA_UART_ALL_USE_1M_PHY_BIT) & 1U;
        payload->params.all_params.bools.use_2m_phy.state =
            (flags >> RE_CA_UART_ALL_USE_2M_PHY_BIT) & 1U;
        payload->params.all_params.bools.ch_37.state =
            (flags >> RE_CA_UART_ALL_CH_37_BIT) & 1U;
        payload->params.all_params.bools.ch_38.state =
            (flags >> RE_CA_UART_ALL_CH_38_BIT) & 1U;
        payload->params.all_params.bools.ch_39.state =
            (flags >> RE_CA_UART_ALL_CH_39_BIT) & 1U;

        if (len == RE_CA_UART_TX_DATA_LEN_CMD_ALL_PARAMS_WITH_MAX_ADV_LEN())
        {
            payload->params.all_params.max_adv_len =
                buffer[RE_CA_UART_PAYLOAD_INDEX
                       + RE_CA_UART_DELIMITER_LEN
                       + RE_CA_UART_CMD_FLTR_ID_LEN
                       + RE_CA_UART_DELIMITER_LEN
                       + RE_CA_UART_CMD_ALL_BOOL_LEN];
        }
        else
        {
            payload->params.all_params.max_adv_len =
                RE_CA_UART_BLE_ALL_PARAMS_MAX_ADV_LEN_NO_LIMIT;
        }
    }

    return err_code;
}

static re_status_t re_ca_uart_decode_adv_rprt (const uint8_t * const buffer,
        re_ca_uart_payload_t * const payload)
{
    const uint8_t packet_len = buffer[RE_CA_UART_LEN_INDEX];
    _Static_assert (RE_CA_UART_PAYLOAD_ADV_RPRT_MAX_LEN <= 255,
                    "Payload too long for uint8_t");
    const uint8_t packet_overhead = RE_CA_UART_PAYLOAD_ADV_RPRT_MAX_LEN -
                                    RE_CA_UART_ADV_BYTES;

    if (packet_len < packet_overhead)
    {
        return RE_ERROR_DECODING_LEN;
    }

    const uint8_t adv_len = packet_len - packet_overhead;
    const size_t max_len = RE_CA_UART_ADV_BYTES;

    if (adv_len > max_len)
    {
        return RE_ERROR_DATA_SIZE;
    }

    const uint8_t * const  p_mac = buffer + RE_CA_UART_PAYLOAD_INDEX;
    const uint8_t * const  p_data = p_mac
                                    + RE_CA_UART_MAC_BYTES
                                    + RE_CA_UART_DELIMITER_LEN;
    const uint8_t * const  p_rssi = p_data + adv_len + RE_CA_UART_DELIMITER_LEN;

    if (RE_CA_UART_FIELD_DELIMITER != * (p_data - RE_CA_UART_DELIMITER_LEN))
    {
        return RE_ERROR_DECODING_DELIMITER;
    }

    if (RE_CA_UART_FIELD_DELIMITER != * (p_rssi - RE_CA_UART_DELIMITER_LEN))
    {
        return RE_ERROR_DECODING_DELIMITER;
    }

    if (RE_CA_UART_FIELD_DELIMITER != * (p_rssi + RE_CA_UART_RSSI_BYTES))
    {
        return RE_ERROR_DECODING_DELIMITER;
    }

    memcpy (payload->params.adv.mac, p_mac, RE_CA_UART_MAC_BYTES);
    memcpy (payload->params.adv.adv, p_data, adv_len);
    payload->cmd = RE_CA_UART_ADV_RPRT;
    payload->params.adv.rssi_db = u8toi8 (*p_rssi);
    payload->params.adv.adv_len = adv_len;
    payload->params.adv.is_coded_phy = false;
    payload->params.adv.primary_phy = RE_CA_UART_BLE_PHY_NOT_SET;
    payload->params.adv.secondary_phy = RE_CA_UART_BLE_PHY_NOT_SET;
    payload->params.adv.ch_index = RE_CA_UART_BLE_GAP_CHANNEL_INDEX_INVALID;
    payload->params.adv.tx_power = RE_CA_UART_BLE_GAP_POWER_LEVEL_INVALID;
    return RE_SUCCESS;
}

static re_status_t re_ca_uart_decode_adv_rprt2 (const uint8_t * const buffer,
        re_ca_uart_payload_t * const payload)
{
    const uint8_t packet_len = buffer[RE_CA_UART_LEN_INDEX];
    _Static_assert (RE_CA_UART_PAYLOAD_ADV_RPRT2_MAX_LEN <= 255,
                    "Payload too long for uint8_t");
    const uint8_t packet_overhead = RE_CA_UART_PAYLOAD_ADV_RPRT2_MAX_LEN -
                                    RE_CA_UART_ADV_BYTES;

    if (packet_len < packet_overhead)
    {
        return RE_ERROR_DECODING_LEN;
    }

    const uint8_t adv_len = packet_len - packet_overhead;
    const size_t max_len = RE_CA_UART_ADV_BYTES;

    if (adv_len > max_len)
    {
        return RE_ERROR_DATA_SIZE;
    }

    const uint8_t * const  p_mac = buffer + RE_CA_UART_PAYLOAD_INDEX;
    const uint8_t * const  p_data = p_mac
                                    + RE_CA_UART_MAC_BYTES
                                    + RE_CA_UART_DELIMITER_LEN;
    const uint8_t * const  p_rssi = p_data + adv_len + RE_CA_UART_DELIMITER_LEN;
    const uint8_t * const  p_extra_info = p_rssi + RE_CA_UART_RSSI_BYTES +
                                          RE_CA_UART_DELIMITER_LEN;

    if (RE_CA_UART_FIELD_DELIMITER != * (p_data - RE_CA_UART_DELIMITER_LEN))
    {
        return RE_ERROR_DECODING_DELIMITER;
    }

    if (RE_CA_UART_FIELD_DELIMITER != * (p_rssi - RE_CA_UART_DELIMITER_LEN))
    {
        return RE_ERROR_DECODING_DELIMITER;
    }

    if (RE_CA_UART_FIELD_DELIMITER != * (p_extra_info - RE_CA_UART_DELIMITER_LEN))
    {
        return RE_ERROR_DECODING_DELIMITER;
    }

    if (RE_CA_UART_FIELD_DELIMITER != * (p_extra_info +
                                         RE_CA_UART_ADV_RPRT2_EXTRA_INFO_BYTES))
    {
        return RE_ERROR_DECODING_DELIMITER;
    }

    memcpy (payload->params.adv.mac, p_mac, RE_CA_UART_MAC_BYTES);
    memcpy (payload->params.adv.adv, p_data, adv_len);
    payload->cmd = RE_CA_UART_ADV_RPRT2;
    payload->params.adv.rssi_db = u8toi8 (*p_rssi);
    payload->params.adv.adv_len = adv_len;
    payload->params.adv.primary_phy = p_extra_info[0] >> 4;
    payload->params.adv.secondary_phy = p_extra_info[0] & 0x0F;
    payload->params.adv.ch_index = p_extra_info[1];
    payload->params.adv.is_coded_phy = (0 != ( (p_extra_info[2] >> 7U) & 0x01U)) ? true :
                                       false;
    payload->params.adv.tx_power = p_extra_info[2] & 0x7FU;
    return RE_SUCCESS;
}

re_status_t re_ca_uart_decode (const uint8_t * const buffer,
                               re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;

    // Sanity check buffer format
    if (NULL == buffer)
    {
        err_code |= RE_ERROR_NULL;
    }
    else if (NULL == payload)
    {
        err_code |= RE_ERROR_NULL;
    }
    else if (RE_CA_UART_STX != buffer[RE_CA_UART_STX_INDEX])
    {
        err_code |= RE_ERROR_DECODING_STX;
    }
    else if (RE_CA_UART_ETX != buffer[buffer[RE_CA_UART_LEN_INDEX]
                                      + RE_CA_UART_CRC_SIZE
                                      + RE_CA_UART_HEADER_SIZE])
    {
        err_code |= RE_ERROR_DECODING_ETX;
    }

#ifdef RE_CA_SUPPORT_OLD_CMD
    else if (RE_CA_UART_NOT_CODED != payload->cmd)
    {
        err_code |= RE_ERROR_INVALID_PARAM;
    }

#endif
    else if (check_crc (buffer,
                        buffer[RE_CA_UART_LEN_INDEX]
                        + RE_CA_UART_HEADER_SIZE) != true)
    {
        err_code |= RE_ERROR_DECODING_CRC;
    }
    else
    {
        switch (buffer[RE_CA_UART_CMD_INDEX])
        {
#ifdef RE_CA_SUPPORT_OLD_CMD

            case RE_CA_UART_SET_FLTR:
                err_code |= re_ca_uart_decode_set_fltr (buffer, payload);
                break;

            case RE_CA_UART_CLR_FLTR:
                err_code |= re_ca_uart_decode_clr_fltr (buffer, payload);
                break;

            case RE_CA_UART_SET_CH:
                err_code |= re_ca_uart_decode_set_ch (buffer, payload);
                break;

            case RE_CA_UART_SET_PHY:
                err_code |= re_ca_uart_decode_set_phy (buffer, payload);
                break;
#endif

            case RE_CA_UART_SET_FLTR_TAGS:
            case RE_CA_UART_SET_CODED_PHY:
            case RE_CA_UART_SET_SCAN_1MB_PHY:
            case RE_CA_UART_SET_SCAN_2MB_PHY:
            case RE_CA_UART_SET_CH_37:
            case RE_CA_UART_SET_CH_38:
            case RE_CA_UART_SET_CH_39:
                err_code |= re_ca_uart_decode_bool_payload (buffer, payload);
                break;

            case RE_CA_UART_ACK:
                err_code |= re_ca_uart_decode_ack (buffer, payload);
                break;

            case RE_CA_UART_SET_FLTR_ID:
                err_code |= re_ca_uart_decode_fltr_id (buffer, payload);
                break;

            case RE_CA_UART_LED_CTRL:
                err_code |= re_ca_uart_decode_led_ctrl (buffer, payload);
                break;

            case RE_CA_UART_SET_ALL:
                err_code |= re_ca_uart_decode_set_all_params (buffer, payload);
                break;

            case RE_CA_UART_ADV_RPRT:
                err_code |= re_ca_uart_decode_adv_rprt (buffer, payload);
                break;

            case RE_CA_UART_DEVICE_ID:
                err_code |=  re_ca_uart_decode_device_id (buffer, payload);
                break;

            case RE_CA_UART_ADV_RPRT2:
                err_code |= re_ca_uart_decode_adv_rprt2 (buffer, payload);
                break;

            case RE_CA_UART_GET_DEVICE_ID:
                err_code |=  re_ca_uart_decode_get_device_id (buffer, payload);
                break;

            case RE_CA_UART_GET_ALL:
                err_code |=  re_ca_uart_decode_get_all (buffer, payload);
                break;

            default:
                err_code |= RE_ERROR_DECODING_CMD;
                break;
        }
    }

    return err_code;
}

static re_status_t re_ca_uart_encode_adv_rprt (uint8_t * const buffer,
        uint8_t * const buf_len,
        const re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;
    uint32_t written = 0;
    const uint32_t tx_len = RE_CA_UART_TX_BUF_LEN (\
                            RE_CA_UART_TX_DATA_LEN_CMD_ADV_RPRT (\
                                    payload->params.adv.adv_len));

    if (tx_len > *buf_len)
    {
        err_code |= RE_ERROR_DATA_SIZE;
    }
    else
    {
        buffer[RE_CA_UART_STX_INDEX] = RE_CA_UART_STX;
        // Payload length is different from total message length.
        buffer[RE_CA_UART_LEN_INDEX] = RE_CA_UART_TX_DATA_LEN_CMD_ADV_RPRT (
                                           payload->params.adv.adv_len);
        buffer[RE_CA_UART_CMD_INDEX] = RE_CA_UART_ADV_RPRT;
        written += RE_CA_UART_HEADER_SIZE;
        memcpy (buffer + RE_CA_UART_PAYLOAD_INDEX,
                payload->params.adv.mac,
                RE_CA_UART_MAC_BYTES);
        written += RE_CA_UART_MAC_BYTES;
        buffer[written++] = RE_CA_UART_FIELD_DELIMITER;
        memcpy (buffer + written,
                payload->params.adv.adv,
                payload->params.adv.adv_len);
        written += payload->params.adv.adv_len;
        buffer[written++] = RE_CA_UART_FIELD_DELIMITER;
        buffer[written++] = i8tou8 (payload->params.adv.rssi_db);
        buffer[written++] = RE_CA_UART_FIELD_DELIMITER;
        add_crc16 (buffer, &written);
        buffer[written++] = RE_CA_UART_ETX;
        *buf_len = written;
    }

    return err_code;
}

static re_status_t re_ca_uart_encode_adv_rprt2 (uint8_t * const buffer,
        uint8_t * const buf_len,
        const re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;
    uint32_t written = 0;

    if ( (RE_CA_UART_TX_BUF_LEN (RE_CA_UART_TX_DATA_LEN_CMD_ADV_RPRT2 (
                                     payload->params.adv.adv_len))) > *buf_len)
    {
        err_code |= RE_ERROR_DATA_SIZE;
    }
    else
    {
        buffer[RE_CA_UART_STX_INDEX] = RE_CA_UART_STX;
        // Payload length is different from total message length.
        buffer[RE_CA_UART_LEN_INDEX] = RE_CA_UART_TX_DATA_LEN_CMD_ADV_RPRT2 (
                                           payload->params.adv.adv_len);
        buffer[RE_CA_UART_CMD_INDEX] = RE_CA_UART_ADV_RPRT2;
        written += RE_CA_UART_HEADER_SIZE;
        memcpy (buffer + RE_CA_UART_PAYLOAD_INDEX,
                payload->params.adv.mac,
                RE_CA_UART_MAC_BYTES);
        written += RE_CA_UART_MAC_BYTES;
        buffer[written++] = RE_CA_UART_FIELD_DELIMITER;
        memcpy (buffer + written,
                payload->params.adv.adv,
                payload->params.adv.adv_len);
        written += payload->params.adv.adv_len;
        buffer[written++] = RE_CA_UART_FIELD_DELIMITER;
        buffer[written++] = i8tou8 (payload->params.adv.rssi_db);
        buffer[written++] = RE_CA_UART_FIELD_DELIMITER;
        buffer[written++] = ( (payload->params.adv.primary_phy & 0x0FU) << 4U) |
                            (payload->params.adv.secondary_phy & 0x0FU);
        buffer[written++] = payload->params.adv.ch_index;
        buffer[written++] = ( (payload->params.adv.is_coded_phy & 0x01U) << 7U) |
                            (payload->params.adv.tx_power & 0x7FU);
        buffer[written++] = RE_CA_UART_FIELD_DELIMITER;
        add_crc16 (buffer, &written);
        buffer[written++] = RE_CA_UART_ETX;
        *buf_len = written;
    }

    return err_code;
}

static re_status_t re_ca_uart_encode_bool_payload (uint8_t * const buffer,
        uint8_t * const buf_len,
        const re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;
    uint32_t written = 0;

    if ( (RE_CA_UART_TX_BUF_LEN (RE_CA_UART_TX_DATA_LEN_CMD_BOOL())) > *buf_len)
    {
        err_code |= RE_ERROR_DATA_SIZE;
    }
    else
    {
        buffer[RE_CA_UART_STX_INDEX] = RE_CA_UART_STX;
        // Payload length is different from total message length.
        buffer[RE_CA_UART_LEN_INDEX] = RE_CA_UART_TX_DATA_LEN_CMD_BOOL();
        buffer[RE_CA_UART_CMD_INDEX] = payload->cmd;
        written += RE_CA_UART_HEADER_SIZE;
        buffer[RE_CA_UART_PAYLOAD_INDEX] = (payload->params.bool_param.state & 1U);
        written += RE_CA_UART_CMD_BOOL_LEN;
        buffer[written++] = RE_CA_UART_FIELD_DELIMITER;
        add_crc16 (buffer, &written);
        buffer[written++] = RE_CA_UART_ETX;
        *buf_len = written;
    }

    return err_code;
}

static re_status_t re_ca_uart_encode_fltr_id (uint8_t * const buffer,
        uint8_t * const buf_len,
        const re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;
    uint32_t written = 0;

    if (RE_CA_UART_TX_BUF_LEN (RE_CA_UART_TX_DATA_LEN_CMD_FLTR_ID()) > *buf_len)
    {
        err_code |= RE_ERROR_DATA_SIZE;
    }
    else
    {
        buffer[RE_CA_UART_STX_INDEX] = RE_CA_UART_STX;
        // Payload length is different from total message length.
        buffer[RE_CA_UART_LEN_INDEX] = RE_CA_UART_TX_DATA_LEN_CMD_FLTR_ID();
        buffer[RE_CA_UART_CMD_INDEX] = payload->cmd;
        written += RE_CA_UART_HEADER_SIZE;
        memcpy (buffer + RE_CA_UART_PAYLOAD_INDEX,
                (void *) &payload->params.fltr_id_param.id,
                RE_CA_UART_CMD_FLTR_ID_LEN);
        written += RE_CA_UART_CMD_FLTR_ID_LEN;
        buffer[written++] = RE_CA_UART_FIELD_DELIMITER;
        add_crc16 (buffer, &written);
        buffer[written++] = RE_CA_UART_ETX;
        *buf_len = written;
    }

    return err_code;
}

static re_status_t re_ca_uart_encode_led_ctrl (uint8_t * const buffer,
        uint8_t * const buf_len,
        const re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;
    uint32_t written = 0;

    if (RE_CA_UART_TX_BUF_LEN (RE_CA_UART_TX_DATA_LEN_CMD_LED_CTRL()) > *buf_len)
    {
        err_code |= RE_ERROR_DATA_SIZE;
    }
    else
    {
        buffer[RE_CA_UART_STX_INDEX] = RE_CA_UART_STX;
        // Payload length is different from total message length.
        buffer[RE_CA_UART_LEN_INDEX] = RE_CA_UART_TX_DATA_LEN_CMD_LED_CTRL();
        buffer[RE_CA_UART_CMD_INDEX] = payload->cmd;
        written += RE_CA_UART_HEADER_SIZE;
        memcpy (buffer + RE_CA_UART_PAYLOAD_INDEX,
                (void *) &payload->params.led_ctrl_param.time_interval_ms,
                RE_CA_UART_CMD_LED_CTRL_LEN);
        written += RE_CA_UART_CMD_LED_CTRL_LEN;
        buffer[written++] = RE_CA_UART_FIELD_DELIMITER;
        add_crc16 (buffer, &written);
        buffer[written++] = RE_CA_UART_ETX;
        *buf_len = written;
    }

    return err_code;
}

static re_status_t re_ca_uart_encode_ack (uint8_t * const buffer,
        uint8_t * const buf_len,
        const re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;
    uint32_t written = 0;

    if (RE_CA_UART_TX_BUF_LEN (RE_CA_UART_TX_DATA_LEN_CMD_ACK()) > *buf_len)
    {
        err_code |= RE_ERROR_DATA_SIZE;
    }
    else
    {
        buffer[RE_CA_UART_STX_INDEX] = RE_CA_UART_STX;
        // Payload length is different from total message length.
        buffer[RE_CA_UART_LEN_INDEX] = RE_CA_UART_TX_DATA_LEN_CMD_ACK();
        buffer[RE_CA_UART_CMD_INDEX] = payload->cmd;
        written += RE_CA_UART_HEADER_SIZE;
        buffer[written++] = (uint8_t) payload->params.ack.cmd;
        buffer[written++] = RE_CA_UART_FIELD_DELIMITER;
        buffer[written++] = (payload->params.ack.ack_state.state & 1U);
        buffer[written++] = RE_CA_UART_FIELD_DELIMITER;
        add_crc16 (buffer, &written);
        buffer[written++] = RE_CA_UART_ETX;
        *buf_len = written;
    }

    return err_code;
}

static re_status_t re_ca_uart_encode_device_id (uint8_t * const buffer,
        uint8_t * const buf_len,
        const re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;
    uint32_t written = 0;

    if (RE_CA_UART_TX_BUF_LEN (RE_CA_UART_TX_DATA_LEN_CMD_DEVICE_ID()) > *buf_len)
    {
        err_code |= RE_ERROR_DATA_SIZE;
    }
    else
    {
        buffer[RE_CA_UART_STX_INDEX] = RE_CA_UART_STX;
        // Payload length is different from total message length.
        buffer[RE_CA_UART_LEN_INDEX] = RE_CA_UART_TX_DATA_LEN_CMD_DEVICE_ID();
        buffer[RE_CA_UART_CMD_INDEX] = payload->cmd;
        written += RE_CA_UART_HEADER_SIZE;
        u64_to_array (payload->params.device_id.id, buffer + written, RE_CA_UART_DEVICE_ID_LEN);
        written += RE_CA_UART_DEVICE_ID_LEN;
        buffer[written++] = RE_CA_UART_FIELD_DELIMITER;
        u64_to_array (payload->params.device_id.addr, buffer + written,
                      RE_CA_UART_DEVICE_ADDR_LEN);
        written += RE_CA_UART_DEVICE_ADDR_LEN;
        buffer[written++] = RE_CA_UART_FIELD_DELIMITER;
        add_crc16 (buffer, &written);
        buffer[written++] = RE_CA_UART_ETX;
        *buf_len = written;
    }

    return err_code;
}

static re_status_t re_ca_uart_encode_get_device_id (uint8_t * const buffer,
        uint8_t * const buf_len,
        const re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;
    uint32_t written = 0;

    if (RE_CA_UART_TX_BUF_LEN (RE_CA_UART_TX_DATA_LEN_CMD_GET_DEVICE_ID()) > *buf_len)
    {
        err_code |= RE_ERROR_DATA_SIZE;
    }
    else
    {
        buffer[RE_CA_UART_STX_INDEX] = RE_CA_UART_STX;
        // Payload length is different from total message length.
        buffer[RE_CA_UART_LEN_INDEX] = RE_CA_UART_TX_DATA_LEN_CMD_GET_DEVICE_ID();
        buffer[RE_CA_UART_CMD_INDEX] = payload->cmd;
        written += RE_CA_UART_HEADER_SIZE;
        add_crc16 (buffer, &written);
        buffer[written++] = RE_CA_UART_ETX;
        *buf_len = written;
    }

    return err_code;
}

static re_status_t re_ca_uart_encode_set_all_params (uint8_t * const buffer,
        uint8_t * const buf_len,
        const re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;
    uint32_t written = 0;
    const uint8_t data_len =
        (0 == payload->params.all_params.max_adv_len)
        ? RE_CA_UART_TX_DATA_LEN_CMD_ALL_PARAMS()
        : RE_CA_UART_TX_DATA_LEN_CMD_ALL_PARAMS_WITH_MAX_ADV_LEN();

    if (RE_CA_UART_TX_BUF_LEN (data_len) > *buf_len)
    {
        err_code |= RE_ERROR_DATA_SIZE;
    }
    else
    {
        buffer[RE_CA_UART_STX_INDEX] = RE_CA_UART_STX;
        // Payload length is different from total message length.
        buffer[RE_CA_UART_LEN_INDEX] = data_len;
        buffer[RE_CA_UART_CMD_INDEX] = payload->cmd;
        written += RE_CA_UART_HEADER_SIZE;
        memcpy (buffer + RE_CA_UART_PAYLOAD_INDEX,
                (void *) &payload->params.fltr_id_param.id,
                RE_CA_UART_CMD_FLTR_ID_LEN);
        written += RE_CA_UART_CMD_FLTR_ID_LEN;
        buffer[written++] = RE_CA_UART_FIELD_DELIMITER;
        buffer[written] = ( ( (payload->params.all_params.bools.fltr_tags.state & 1U)
                              << RE_CA_UART_ALL_FLTR_TAG_BIT)
                            | ( (payload->params.all_params.bools.use_coded_phy.state  & 1U)
                                << RE_CA_UART_ALL_USE_CODED_PHY_BIT)
                            | ( (payload->params.all_params.bools.use_1m_phy.state  & 1U)
                                << RE_CA_UART_ALL_USE_1M_PHY_BIT)
                            | ( (payload->params.all_params.bools.use_2m_phy.state  & 1U)
                                << RE_CA_UART_ALL_USE_2M_PHY_BIT)
                            | ( (payload->params.all_params.bools.ch_37.state  & 1U)
                                << RE_CA_UART_ALL_CH_37_BIT)
                            | ( (payload->params.all_params.bools.ch_38.state  & 1U)
                                << RE_CA_UART_ALL_CH_38_BIT)
                            | ( (payload->params.all_params.bools.ch_39.state  & 1U)
                                << RE_CA_UART_ALL_CH_39_BIT));
        written += RE_CA_UART_CMD_ALL_BOOL_LEN;
        buffer[written++] = RE_CA_UART_FIELD_DELIMITER;

        if (0 != payload->params.all_params.max_adv_len)
        {
            buffer[written] = payload->params.all_params.max_adv_len;
            written += RE_CA_UART_CMD_ALL_UINT8_LEN;
            buffer[written++] = RE_CA_UART_FIELD_DELIMITER;
        }

        add_crc16 (buffer, &written);
        buffer[written++] = RE_CA_UART_ETX;
        *buf_len = written;
    }

    return err_code;
}

static re_status_t re_ca_uart_encode_get_all (uint8_t * const buffer,
        uint8_t * const buf_len,
        const re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;
    uint32_t written = 0;

    if (RE_CA_UART_TX_BUF_LEN (RE_CA_UART_TX_DATA_LEN_CMD_GET_ALL_PARAMS()) > *buf_len)
    {
        err_code |= RE_ERROR_DATA_SIZE;
    }
    else
    {
        buffer[RE_CA_UART_STX_INDEX] = RE_CA_UART_STX;
        // Payload length is different from total message length.
        buffer[RE_CA_UART_LEN_INDEX] = RE_CA_UART_TX_DATA_LEN_CMD_GET_ALL_PARAMS();
        buffer[RE_CA_UART_CMD_INDEX] = payload->cmd;
        written += RE_CA_UART_HEADER_SIZE;
        add_crc16 (buffer, &written);
        buffer[written++] = RE_CA_UART_ETX;
        *buf_len = written;
    }

    return err_code;
}

re_status_t re_ca_uart_encode (uint8_t * const buffer, uint8_t * const buf_len,
                               const re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;

    if (NULL == buffer)
    {
        err_code |= RE_ERROR_NULL;
    }
    else if (NULL == buf_len)
    {
        err_code |= RE_ERROR_NULL;
    }
    else if (NULL == payload)
    {
        err_code |= RE_ERROR_NULL;
    }
    else
    {
        switch (payload->cmd)
        {
            case RE_CA_UART_SET_FLTR_TAGS:
            case RE_CA_UART_SET_CODED_PHY:
            case RE_CA_UART_SET_SCAN_1MB_PHY:
            case RE_CA_UART_SET_SCAN_2MB_PHY:
            case RE_CA_UART_SET_CH_37:
            case RE_CA_UART_SET_CH_38:
            case RE_CA_UART_SET_CH_39:
                err_code |= re_ca_uart_encode_bool_payload (buffer, buf_len, payload);
                break;

            case RE_CA_UART_ACK:
                err_code |= re_ca_uart_encode_ack (buffer, buf_len, payload);
                break;

            case RE_CA_UART_SET_FLTR_ID:
                err_code |= re_ca_uart_encode_fltr_id (buffer, buf_len, payload);
                break;

            case RE_CA_UART_LED_CTRL:
                err_code |= re_ca_uart_encode_led_ctrl (buffer, buf_len, payload);
                break;

            case RE_CA_UART_SET_ALL:
                err_code |= re_ca_uart_encode_set_all_params (buffer, buf_len, payload);
                break;

            case RE_CA_UART_ADV_RPRT:
                err_code |=  re_ca_uart_encode_adv_rprt (buffer, buf_len, payload);
                break;

            case RE_CA_UART_DEVICE_ID:
                err_code |=  re_ca_uart_encode_device_id (buffer, buf_len, payload);
                break;

            case RE_CA_UART_ADV_RPRT2:
                err_code |=  re_ca_uart_encode_adv_rprt2 (buffer, buf_len, payload);
                break;

            case RE_CA_UART_GET_DEVICE_ID:
                err_code |=  re_ca_uart_encode_get_device_id (buffer, buf_len, payload);
                break;

            case RE_CA_UART_GET_ALL:
                err_code |= re_ca_uart_encode_get_all (buffer, buf_len, payload);
                break;

            default:
                err_code |= RE_ERROR_INVALID_PARAM;
                break;
        }
    }

    return err_code;
}

#endif
