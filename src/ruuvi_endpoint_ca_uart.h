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


typedef enum
{
    RE_CA_UART_SET_FLTR = 1, //!< Set manufacturer ID filter.
    RE_CA_UART_CLR_FLTR = 2, //!< Clear manufacturer ID filter.
    RE_CA_UART_SET_CH   = 3, //!< Set channel mask.
    RE_CA_UART_SET_PHY  = 4, //!< Set PHY mask.
    RE_CA_UART_ADV_RPRT = 5  //!< Advertisement report
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
    uint8_t mac[RE_CA_UART_MAC_BYTES];  // MAC address
    uint8_t adv[RE_CA_UART_ADV_BYTES];  // Advertisement
    uint8_t rssi[RE_CA_UART_RSSI_BYTES]; // RSSI
} re_ca_uart_ble_scan_t; //!< Scan payload

typedef struct
{
    uint8_t stx; //!< First byte, always fixed STX.
    uint8_t len; //!< Length of payload, + length(cmd) in legacy mode.
    re_ca_uart_cmd_t cmd; //!< Command to send.
    uint8_t payload[RE_CA_UART_PAYLOAD_MAX_LEN]; //!< Command payload.
    uint8_t etx;    //!< Last byte always fixed ETX.
} re_ca_uart_tx_t; //!< Structure of CA_UART data.

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
re_status_t re_ca_uart_encode (uint8_t * const buffer, const re_ca_uart_cmd_t * const cmd,
                               const void * const params);

/**
 * @brief Decode given buffer to cmd + params.
 *
 * param[in]  buffer Buffer to decode command. Maximum RE_CA_UART_TX_MAX_LEN bytes.
 * param[out] cmd Decoded command type.
 * param[out] params Parametes of command, type depends on cmd.
 */
re_status_t re_ca_uart_decode (const uint8_t * const buffer, re_ca_uart_cmd_t * const cmd,
                               void * const params);

#endif // RUUVI_ENDPOINT_GW_UART_H
