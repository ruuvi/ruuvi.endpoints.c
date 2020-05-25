#ifndef RUUVI_ENDPOINT_CA_UART_H
#define RUUVI_ENDPOINT_CA_UART_H

#include "ruuvi_endpoints.h"

//032e344199bd,0206011bff9904xxxxxxxxxxxxxxxxxxxx,-72,\n
#define RE_CA_UART_MAC_BYTES (6U) //!< Number of bytes in MAC address
#define RE_CA_UART_ADV_BYTES (31U) //!< Number of bytes in Advertisement. 
#define RE_CA_UART_RSSI_BYTES (1U) //!< Number of bytes in RSSI report.
#if RE_CA_UART_LEGACY_MODE
#   define RE_CA_UART_STX (0x02U) //!< Start UART Command. ASCII STX.
#   define RE_CA_UART_ETX (0x03U) //!< End UART Command. ASCII ETX.
#   define RE_CA_UART_RSSI_MAXLEN (4U) //!< minus + 3 digits. 
#else
#   define RE_CA_UART_STX (0xCAU) //!< Start UART Command. Not related to ASCII STX.
#   define RE_CA_UART_ETX (0x12U) //!< End UART Command, '\n'. Not related to ASCII ETX.
#   define RE_CA_UART_RSSI_MAXLEN (RE_CA_UART_RSSI_BYTES) //!< i8.
#endif
#define RE_CA_UART_MAC_MAXLEN (RE_CA_UART_MAC_BYTES) //!< Length of MAC in UART.
#define RE_CA_UART_ADV_MAXLEN (RE_CA_UART_ADV_BYTES) //!< Length of adv.
#define RE_CA_UART_MAXFIELDS (3U)   //!< On scan: mac, data, rssi.
#define RE_CA_UART_PAYLOAD_MAX_LEN (RE_CA_UART_MAC_MAXLEN \
                                    + RE_CA_UART_ADV_MAXLEN \
                                    + RE_CA_UART_MAXFIELDS - 1U) //!< data + delimiters
#define RE_CA_UART_FIELD_DELIMITER (0x2CU) //!< ','
/** @brief STX, LEN, CMD, Payload, ETX */
#define RE_CA_UART_TX_MAX_LEN (RE_CA_UART_PAYLOAD_MAX_LEN + 4U)

#define RE_CA_UART_BLE_NOFILTER (0x0000U) //!< Do not apply filter to ID.

#define RE_CA_UART_HEADER_SIZE   (3U) //!< STX + len + CMD
#define RE_CA_UART_STX_INDEX     (0U) //!< Position of stx byte.
#define RE_CA_UART_LEN_INDEX     (1U) //!< Position of length byte.
#define RE_CA_UART_CMD_INDEX     (2U) //!< Position of CMD byte.
#define RE_CA_UART_PAYLOAD_INDEX (3U) //!< Start of payload.

#define RE_CA_UART_CH39_BYTE     (4U) //!< Byte of channel 39, starting from 0.
#define RE_CA_UART_CH39_BIT      (7U) //!< Bit of channel 39, starting from 0.
#define RE_CA_UART_CH38_BYTE     (4U) //!< Byte of channel 38, starting from 0.
#define RE_CA_UART_CH38_BIT      (6U) //!< Bit of channel 38, starting from 0.
#define RE_CA_UART_CH37_BYTE     (4U) //!< Byte of channel 37, starting from 0.
#define RE_CA_UART_CH37_BIT      (5U) //!< Bit of channel 37, starting from 0.

#define RE_CA_UART_125KBPS_BIT   (0U) //!< Bit of 125kpbs modulation, starting from 0.
#define RE_CA_UART_1MBPS_BIT     (1U) //!< Bit of 125kpbs modulation, starting from 0.
#define RE_CA_UART_2MBPS_BIT     (2U) //!< Bit of 125kpbs modulation, starting from 0.

#define RE_CA_UART_CMD_SFLTR_LEN (2U) //!< Length of filter set command payload.
#define RE_CA_UART_CMD_CFLTR_LEN (0U) //!< Length of filter clear command payload. 
#define RE_CA_UART_CMD_CH_LEN    (5U) //!< Length of channel command payload. 
#define RE_CA_UART_CMD_PHY_LEN   (1U) //!< Length of phy command payload. 

// ETX is at variable position after payload

typedef enum
{
    RE_CA_UART_NOT_CODED = 0, //!< Command is not de/encoded, do not use as-is.
    RE_CA_UART_SET_FLTR  = 1, //!< Set manufacturer ID filter.
    RE_CA_UART_CLR_FLTR  = 2, //!< Clear manufacturer ID filter.
    RE_CA_UART_SET_CH    = 3, //!< Set channel mask.
    RE_CA_UART_SET_PHY   = 4, //!< Set PHY mask.
    RE_CA_UART_ADV_RPRT  = 5  //!< Advertisement report.
} re_ca_uart_cmd_t;          //!< Command types.

typedef struct
{
    uint16_t manufacturer_id; //!< Manufacturer ID, MSB first. 0x0499 for Ruuvi.
} re_ca_uart_ble_filter_t;    //!< Manufacturer filter payload.

typedef struct
{
    uint8_t ch37 : 1; //!< BLE Channel 37 enabled.
    uint8_t ch38 : 1; //!< BLE Channel 38 enabled.
    uint8_t ch39 : 1; //!< BLE Channel 39 enabled.
} re_ca_uart_ble_ch_t; //!< Enabled BLE Channels.

typedef struct
{
    uint8_t ble_125kbps : 1; //!< BLE Channel 37 enabled.
    uint8_t ble_1mbps   : 1; //!< BLE Channel 38 enabled.
    uint8_t ble_2mbps   : 1; //!< BLE Channel 39 enabled.
} re_ca_uart_ble_phy_t;     //!< Enabled BLE PHYs.

typedef struct
{
    uint8_t mac[RE_CA_UART_MAC_BYTES];   // MAC address.
    uint8_t adv[RE_CA_UART_ADV_BYTES];   // Advertisement.
    uint8_t rssi[RE_CA_UART_RSSI_BYTES]; // RSSI.
} re_ca_uart_ble_adv_t;                  //!< Advertisement payload.

typedef struct
{
    uint8_t stx; //!< First byte, always fixed STX.
    uint8_t len; //!< Length of payload, + length(cmd) in legacy mode.
    re_ca_uart_cmd_t cmd; //!< Command to send.
    uint8_t payload[RE_CA_UART_PAYLOAD_MAX_LEN]; //!< Command payload.
    uint8_t etx;    //!< Last byte always fixed ETX.
} re_ca_uart_tx_t; //!< Structure of CA_UART data.

// MISRA deviation - use of union.
// Reasoning for using union is to allow single data type to contain information
// of incoming binary payload, type has to be interpreted at runtime.
// Misinterpreting the payload is avoided via member cmd which is set by decoder.
typedef struct
{
    re_ca_uart_cmd_t cmd; //!< Type of command discriminating the union.
    union
    {
        re_ca_uart_ble_filter_t filter;   //!< Filter param.
        re_ca_uart_ble_ch_t     channels; //!< Channel param.
        re_ca_uart_ble_phy_t    phys;     //!< Phy param.
        re_ca_uart_ble_adv_t    adv;      //!< Advertisement report param.
    } params;
} re_ca_uart_payload_t; //!< Structured payload;

/**
 * @brief Encode given command with given parameters into buffer.
 *
 * param[out] buffer Buffer to encode command into. RE_CA_UART_TX_MAX_LEN bytes.
 * param[in]  cmd Command to encode.
 * param[in]  params Parametes of command, type depends on cmd.
 *
 * @retval RE_SUCCESS If encoded successfully.
 * @retval RE_ERROR_NULL If any of params is NULL.
 * @retval RE_ERROR_INVALID_PARAM If data cannot be encoded.
 */
re_status_t re_ca_uart_encode (uint8_t * const buffer,
                               const re_ca_uart_payload_t * const payload);

/**
 * @brief Decode given buffer to cmd + params.
 *
 * param[in]  buffer Buffer to decode command. Maximum RE_CA_UART_TX_MAX_LEN bytes.
 * param[out] cmd Decoded command type.
 * param[out] params Parametes of command, type depends on cmd.
 */
re_status_t re_ca_uart_decode (const uint8_t * const buffer,
                               re_ca_uart_payload_t * const cmd);

#endif // RUUVI_ENDPOINT_GW_UART_H
