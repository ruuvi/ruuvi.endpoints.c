#include "ruuvi_endpoints.h"
#include "ruuvi_endpoint_ca_uart.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define I8_MAX (127U)
#define I8_MIN (-128)
#define U8_OVERFLOW (256)

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

static re_status_t re_ca_uart_decode_adv_rprt (const uint8_t * const buffer,
        re_ca_uart_payload_t * const payload)
{
    re_status_t err_code = RE_SUCCESS;
    const uint8_t adv_len = buffer[RE_CA_UART_LEN_INDEX]
                            - RE_CA_UART_MAC_BYTES
                            - RE_CA_UART_RSSI_BYTES
                            - (RE_CA_UART_MAXFIELDS) * RE_CA_UART_DELIMITER_LEN;
    const uint8_t * const  p_rssi = buffer
                                    + RE_CA_UART_PAYLOAD_INDEX
                                    + buffer[RE_CA_UART_LEN_INDEX]
                                    - RE_CA_UART_RSSI_BYTES
                                    - RE_CA_UART_DELIMITER_LEN;
    const uint8_t * const  p_data = buffer
                                    + RE_CA_UART_PAYLOAD_INDEX
                                    + RE_CA_UART_MAC_BYTES
                                    + RE_CA_UART_DELIMITER_LEN;

    if (adv_len > RE_CA_UART_ADV_BYTES)
    {
        err_code |= RE_ERROR_DECODING;
    }
    else if (RE_CA_UART_FIELD_DELIMITER != * (p_data - RE_CA_UART_DELIMITER_LEN))
    {
        err_code |= RE_ERROR_DECODING;
    }
    else if (RE_CA_UART_FIELD_DELIMITER != * (p_rssi - RE_CA_UART_DELIMITER_LEN))
    {
        err_code |= RE_ERROR_DECODING;
    }
    else if (RE_CA_UART_FIELD_DELIMITER != * (p_rssi + RE_CA_UART_RSSI_BYTES))
    {
        err_code |= RE_ERROR_DECODING;
    }
    else
    {
        memcpy (payload->params.adv.mac,
                buffer + RE_CA_UART_PAYLOAD_INDEX,
                RE_CA_UART_MAC_BYTES);
        memcpy (payload->params.adv.adv,
                p_data,
                adv_len);
        payload->cmd = RE_CA_UART_ADV_RPRT;
        payload->params.adv.rssi_db = u8toi8 (*p_rssi);
        payload->params.adv.adv_len = adv_len;
    }

    return err_code;
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
        err_code |= RE_ERROR_DECODING;
    }
    else if (RE_CA_UART_ETX != buffer[buffer[RE_CA_UART_LEN_INDEX] + RE_CA_UART_HEADER_SIZE])
    {
        err_code |= RE_ERROR_DECODING;
    }
    else if (RE_CA_UART_NOT_CODED != payload->cmd)
    {
        err_code |= RE_ERROR_INVALID_PARAM;
    }
    else
    {
        switch (buffer[RE_CA_UART_CMD_INDEX])
        {
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

            case RE_CA_UART_ADV_RPRT:
                err_code |= re_ca_uart_decode_adv_rprt (buffer, payload);
                break;

            default:
                err_code |= RE_ERROR_DECODING;
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
    uint8_t written = 0;

    if (RE_CA_UART_TX_MAX_LEN > *buf_len)
    {
        err_code |= RE_ERROR_DATA_SIZE;
    }
    else
    {
        buffer[RE_CA_UART_STX_INDEX] = RE_CA_UART_STX;
        // Payload length is different from total message length.
        buffer[RE_CA_UART_LEN_INDEX] = RE_CA_UART_MAC_BYTES
                                       + payload->params.adv.adv_len
                                       + RE_CA_UART_RSSI_BYTES
                                       + RE_CA_UART_ADV_FIELDS * RE_CA_UART_DELIMITER_LEN;
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
            case RE_CA_UART_ADV_RPRT:
                err_code |=  re_ca_uart_encode_adv_rprt (buffer, buf_len, payload);
                break;

            default:
                err_code |= RE_ERROR_INVALID_PARAM;
                break;
        }
    }

    return err_code;
}