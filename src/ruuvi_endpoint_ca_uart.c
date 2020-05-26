#include "ruuvi_endpoints.h"
#include "ruuvi_endpoint_ca_uart.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define I8_MAX (127U)
#define U8_OVERFLOW (256)

re_status_t re_ca_uart_encode (uint8_t * const buffer,
                               const re_ca_uart_payload_t * const payload)
{
    return RE_ERROR_NOT_IMPLEMENTED;
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

    if (buffer[RE_CA_UART_LEN_INDEX] != RE_CA_UART_CMD_PHY_LEN)
    {
        err_code |= RE_ERROR_DECODING;
    }
    else
    {
        payload->cmd = RE_CA_UART_ADV_RPRT;
        memcpy (payload->params.adv.mac, buffer + RE_CA_UART_PAYLOAD_INDEX, RE_CA_UART_MAC_BYTES);
        payload->params.adv.adv_len = buffer[RE_CA_UART_LEN_INDEX] - RE_CA_UART_MAC_BYTES -
                                      RE_CA_UART_RSSI_BYTES;
        memcpy (payload->params.adv.adv, buffer + RE_CA_UART_PAYLOAD_INDEX + RE_CA_UART_MAC_BYTES,
                payload->params.adv.adv_len);
        payload->params.adv.rssi_db =
            u8toi8 (buffer[RE_CA_UART_PAYLOAD_INDEX + buffer[RE_CA_UART_LEN_INDEX] -
                                                    RE_CA_UART_RSSI_BYTES]);
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
