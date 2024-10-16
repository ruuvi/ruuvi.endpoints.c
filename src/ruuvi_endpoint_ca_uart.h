#ifndef RUUVI_ENDPOINT_CA_UART_H
#define RUUVI_ENDPOINT_CA_UART_H

#include "ruuvi_endpoints.h"
#include <stdbool.h>

#define RE_CA_CRC_DEFAULT       0xFFFF
#define RE_CA_CRC_INVALID       0

#define RE_CA_ACK_OK            0
#define RE_CA_ACK_ERROR         1

#define RE_CA_BOOL_ENABLE       1
#define RE_CA_BOOL_DISABLE      0

#define RE_CA_UART_DEVICE_ID_BYTES (8U) //!< Number of bytes in DEVICE_ID
#define RE_CA_UART_DEVICE_ADDR_BYTES (8U) //!< Number of bytes in DEVICE_ADDR
#define RE_CA_UART_MAC_BYTES (6U) //!< Number of bytes in MAC address

#if defined(RI_ADV_EXTENDED_ENABLED) && RI_ADV_EXTENDED_ENABLED
#   if defined(RI_COMM_BLE_PAYLOAD_MAX_LENGTH)
#       define RE_CA_UART_ADV_BYTES (RI_COMM_BLE_PAYLOAD_MAX_LENGTH) //!< Number of bytes in Advertisement.
#   else
#       error "RI_COMM_BLE_PAYLOAD_MAX_LENGTH must be defined when RI_ADV_EXTENDED_ENABLED=1"
#   endif
#else
#   define RE_CA_UART_ADV_BYTES (31U) //!< Number of bytes in Advertisement.
#endif

#define RE_CA_UART_RSSI_BYTES (1U) //!< Number of bytes in RSSI report.
#define RE_CA_UART_ADV_RPRT2_EXTRA_INFO_BYTES (3) //!< Number of bytes in BLE PHY extra info.
#if RE_CA_UART_LEGACY_MODE
#   define RE_CA_UART_STX (0x02U) //!< Start UART Command. ASCII STX.
#   define RE_CA_UART_ETX (0x03U) //!< End UART Command. ASCII ETX.
#   define RE_CA_UART_RSSI_MAXLEN (4U) //!< minus + 3 digits. 
#   define CMD_IN_LEN (1U) //!< Command is included in data length.
#else
#   define RE_CA_UART_STX (0xCAU) //!< Start UART Command. Not related to ASCII STX.
#   define RE_CA_UART_ETX (0x0AU) //!< End UART Command, '\n'. Not related to ASCII ETX.
#   define RE_CA_UART_RSSI_MAXLEN (RE_CA_UART_RSSI_BYTES) //!< i8.
#   define CMD_IN_LEN (0U) //!< Command is not included in data length.
#endif
#define RE_CA_UART_ADV_RPRT_FIELDS (3U)   //!< On scan: mac, data, rssi.
#define RE_CA_UART_ADV_RPRT2_FIELDS (4U)  //!< On scan: mac, data, rssi, extra_info.
#define RE_CA_UART_ADV_RPRT2_EXTRA_INFO_LEN (3U) //!< 3 bytes for PHY, ch_index, tx_power.
#define RE_CA_UART_FIELD_DELIMITER (0x2CU) //!< ','
#define RE_CA_UART_DELIMITER_LEN   (1U)    //!< 1 byte delimiter.
/** @brief STX, LEN, CMD, Payload, CRC, ETX */

#define RE_CA_UART_PAYLOAD_ADV_RPRT_MAX_LEN (RE_CA_UART_MAC_BYTES \
                                    + RE_CA_UART_ADV_BYTES \
                                    + RE_CA_UART_RSSI_MAXLEN \
                                    + RE_CA_UART_ADV_RPRT_FIELDS * RE_CA_UART_DELIMITER_LEN) //!< ADV_RPRT_LEN: data + delimiters
#define RE_CA_UART_PAYLOAD_ADV_RPRT2_MAX_LEN (RE_CA_UART_MAC_BYTES \
                                    + RE_CA_UART_ADV_BYTES \
                                    + RE_CA_UART_RSSI_MAXLEN \
                                    + RE_CA_UART_ADV_RPRT2_EXTRA_INFO_LEN \
                                    + RE_CA_UART_ADV_RPRT2_FIELDS * RE_CA_UART_DELIMITER_LEN) //!< ADV_RPRT2_LEN: data + delimiters

#define RE_CA_UART_BLE_NOFILTER (0x0000U) //!< Do not apply filter to ID.

#define RE_CA_UART_HEADER_SIZE   (3U) //!< STX + len + CMD
#define RE_CA_UART_CRC_SIZE      (2U) //!< STX + len + CMD
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

#define RE_CA_UART_CMD_SFLTR_LEN    (2U) //!< Length of filter set command payload.
#define RE_CA_UART_CMD_CFLTR_LEN    (0U) //!< Length of filter clear command payload. 
#define RE_CA_UART_CMD_CH_LEN       (5U) //!< Length of channel command payload. 
#define RE_CA_UART_CMD_PHY_LEN      (1U) //!< Length of phy command payload. 

#define RE_CA_UART_BOOL_BYTE        (0U) //!< Byte of bool params, starting from 0.
#define RE_CA_UART_BOOL_BIT         (0U) //!< Bit of bool params, starting from 0.

#define RE_CA_UART_ACK_CMD_BYTE     (1U) //!< Byte of bool params, starting from 0.
#define RE_CA_UART_ACK_BYTE         (1U) //!< Byte of bool params, starting from 0.
#define RE_CA_UART_ACK_BIT          (0U) //!< Bit of bool params, starting from 0.

#define RE_CA_UART_FLTR_ID_BYTE         (2U) //!< Byte of bool params, starting from 0.
#define RE_CA_UART_ALL_BOOL_BYTE        (1U) //!< Byte of bool params, starting from 0.
#define RE_CA_UART_ALL_FLTR_TAG_BIT     (0U) //!< Byte of bool params, starting from 0.
#define RE_CA_UART_ALL_CODED_PHY_BIT    (1U) //!< Byte of bool params, starting from 0.
#define RE_CA_UART_ALL_SCAN_PHY_BIT     (2U) //!< Byte of bool params, starting from 0.
#define RE_CA_UART_ALL_EXT_PLD_BIT      (3U) //!< Byte of bool params, starting from 0.
#define RE_CA_UART_ALL_CH_37_BIT        (4U) //!< Byte of bool params, starting from 0.
#define RE_CA_UART_ALL_CH_38_BIT        (5U) //!< Byte of bool params, starting from 0.
#define RE_CA_UART_ALL_CH_39_BIT        (6U) //!< Byte of bool params, starting from 0.

#define RE_CA_UART_STX_ETX_LEN          (1U) //!< Length of cmd with bool payload
#define RE_CA_UART_DEVICE_ID_LEN        (8U) //!< Length of device id payload
#define RE_CA_UART_DEVICE_ADDR_LEN      (6U) //!< Length of device addr payload
#define RE_CA_UART_CMD_BOOL_LEN         (1U) //!< Length of cmd with bool payload
#define RE_CA_UART_CMD_FLTR_ID_LEN      (2U) //!< Length of filter_id payload
#define RE_CA_UART_CMD_LED_CTRL_LEN     (2U) //!< Length of led_ctrl payload
#define RE_CA_UART_CMD_ACK_LEN          (1U) //!< Length of ack payload
#define RE_CA_UART_CMD_ALL_BOOL_LEN     (1U) //!< Length of cmd with bool payload
#define RE_CA_UART_CMD_ALL_UINT8_LEN    (1U) //!< Length of cmd with uint8 payload
//!< Length of all command payload

/** @breif Command types. */
typedef enum
{
#ifdef RE_CA_SUPPORT_OLD_CMD
    /* Old config CMD */
    RE_CA_UART_NOT_CODED = 0,       //!< Command is not de/encoded, do not use as-is.
    RE_CA_UART_SET_FLTR  = 1,       //!< Set manufacturer ID filter.
    RE_CA_UART_CLR_FLTR  = 2,       //!< Clear manufacturer ID filter.
    RE_CA_UART_SET_CH    = 3,       //!< Set channel mask.
    RE_CA_UART_SET_PHY   = 4,       //!< Set PHY mask.
#endif
    RE_CA_UART_SET_FLTR_TAGS    = 5, //!< Set filter tags.
    RE_CA_UART_SET_FLTR_ID      = 6, //!< Set manufacturer ID filter.
    RE_CA_UART_SET_CODED_PHY    = 7, //!< Set coded PHY.
    RE_CA_UART_SET_SCAN_1MB_PHY = 8, //!< Set scan 1MBbit/PHY.
    RE_CA_UART_SET_SCAN_2MB_PHY = 9, //!< Set scan 2MBbit/PHY.
    RE_CA_UART_SET_CH_37        = 10, //!< Set channel 37.
    RE_CA_UART_SET_CH_38        = 11, //!< Set channel 38.
    RE_CA_UART_SET_CH_39        = 12, //!< Set channel 39.
    RE_CA_UART_LED_CTRL         = 14,//!< Turn on LED.
    RE_CA_UART_SET_ALL          = 15,//!< Set all config.
    RE_CA_UART_ADV_RPRT         = 16,//!< Advertisement report. ACK no need.
    RE_CA_UART_DEVICE_ID        = 17,//!< Send device id. ACK no need.
    RE_CA_UART_ADV_RPRT2        = 18,//!< Advertisement report with extra info. ACK no need.
    RE_CA_UART_GET_DEVICE_ID    = 24,//!< Get device id. Expect RE_CA_UART_DEVICE_ID.
    RE_CA_UART_GET_ALL          = 25,//!< Get all config.
    RE_CA_UART_ACK              = 32,//!< ACK
} re_ca_uart_cmd_t;

#define RE_CA_UART_TX_BUF_LEN(data_len) (RE_CA_UART_HEADER_SIZE + \
                                     (data_len) + \
                                     RE_CA_UART_CRC_SIZE + RE_CA_UART_STX_ETX_LEN)

#define RE_CA_UART_TX_DATA_LEN_0_PARAMS() (0)

#define RE_CA_UART_TX_DATA_LEN_1_PARAM(param_len) ((param_len) + RE_CA_UART_DELIMITER_LEN)

#define RE_CA_UART_TX_DATA_LEN_2_PARAMS(data1_len, data2_len) (\
                                     (data1_len) + RE_CA_UART_DELIMITER_LEN + \
                                     (data2_len) + RE_CA_UART_DELIMITER_LEN)

#define RE_CA_UART_TX_DATA_LEN_3_PARAMS(param1_len, param2_len, param3_len) ( \
                                     (param1_len) + RE_CA_UART_DELIMITER_LEN + \
                                     (param2_len) + RE_CA_UART_DELIMITER_LEN + \
                                     (param3_len) + RE_CA_UART_DELIMITER_LEN)

#define RE_CA_UART_TX_DATA_LEN_4_PARAMS(param1_len, param2_len, param3_len, param4_len) ( \
                                     (param1_len) + RE_CA_UART_DELIMITER_LEN + \
                                     (param2_len) + RE_CA_UART_DELIMITER_LEN + \
                                     (param3_len) + RE_CA_UART_DELIMITER_LEN + \
                                     (param4_len) + RE_CA_UART_DELIMITER_LEN)

#define RE_CA_UART_TX_DATA_LEN_CMD_ADV_RPRT(data_len) \
    RE_CA_UART_TX_DATA_LEN_3_PARAMS( \
        RE_CA_UART_MAC_BYTES, \
        data_len, \
        RE_CA_UART_RSSI_BYTES)

#define RE_CA_UART_TX_DATA_LEN_CMD_ADV_RPRT2(data_len) \
    RE_CA_UART_TX_DATA_LEN_4_PARAMS( \
        RE_CA_UART_MAC_BYTES, \
        data_len, \
        RE_CA_UART_RSSI_BYTES, \
        RE_CA_UART_ADV_RPRT2_EXTRA_INFO_BYTES)

#define RE_CA_UART_TX_DATA_LEN_CMD_BOOL() \
    RE_CA_UART_TX_DATA_LEN_1_PARAM(RE_CA_UART_CMD_BOOL_LEN)

#define RE_CA_UART_TX_DATA_LEN_CMD_FLTR_ID() \
    RE_CA_UART_TX_DATA_LEN_1_PARAM(RE_CA_UART_CMD_FLTR_ID_LEN)

#define RE_CA_UART_TX_DATA_LEN_CMD_LED_CTRL() \
    RE_CA_UART_TX_DATA_LEN_1_PARAM(RE_CA_UART_CMD_LED_CTRL_LEN)

#define RE_CA_UART_TX_DATA_LEN_CMD_ACK() \
    RE_CA_UART_TX_DATA_LEN_2_PARAMS(RE_CA_UART_CMD_ACK_LEN, RE_CA_UART_CMD_BOOL_LEN)

#define RE_CA_UART_TX_DATA_LEN_CMD_DEVICE_ID() \
    RE_CA_UART_TX_DATA_LEN_2_PARAMS(RE_CA_UART_DEVICE_ID_LEN, RE_CA_UART_DEVICE_ADDR_LEN)

#define RE_CA_UART_TX_DATA_LEN_CMD_GET_DEVICE_ID() \
    RE_CA_UART_TX_DATA_LEN_0_PARAMS()

#define RE_CA_UART_TX_DATA_LEN_CMD_ALL_PARAMS() \
    RE_CA_UART_TX_DATA_LEN_2_PARAMS(RE_CA_UART_CMD_FLTR_ID_LEN, RE_CA_UART_CMD_ALL_BOOL_LEN)

#define RE_CA_UART_TX_DATA_LEN_CMD_ALL_PARAMS_WITH_MAX_ADV_LEN() \
    RE_CA_UART_TX_DATA_LEN_3_PARAMS(RE_CA_UART_CMD_FLTR_ID_LEN, RE_CA_UART_CMD_ALL_BOOL_LEN, RE_CA_UART_CMD_ALL_UINT8_LEN)

#define RE_CA_UART_TX_DATA_LEN_CMD_GET_ALL_PARAMS() \
    RE_CA_UART_TX_DATA_LEN_0_PARAMS()

/** @brief Buffer for the encoded 'bool' payload. */
typedef struct re_ca_uart_mosi_payload_buf_encoded_bool_t
{
    /** Buffer for the encoded 'bool' payload. */
    uint8_t buf[RE_CA_UART_TX_BUF_LEN (RE_CA_UART_TX_DATA_LEN_CMD_BOOL())];
} re_ca_uart_mosi_payload_buf_encoded_bool_t;

/** @brief Buffer for the encoded 'set_fltr_id' payload. */
typedef struct re_ca_uart_mosi_payload_buf_encoded_set_fltr_id_t
{
    /** Buffer for the encoded 'set_fltr_id' payload. */
    uint8_t buf[RE_CA_UART_TX_BUF_LEN (RE_CA_UART_TX_DATA_LEN_CMD_FLTR_ID())];
} re_ca_uart_mosi_payload_buf_encoded_set_fltr_id_t;

/** @brief Buffer for the encoded 'get_device_id' payload. */
typedef struct re_ca_uart_mosi_payload_buf_encoded_get_device_id_t
{
    /** Buffer for the encoded 'get_device_id' payload. */
    uint8_t buf[RE_CA_UART_TX_BUF_LEN (RE_CA_UART_TX_DATA_LEN_CMD_GET_DEVICE_ID())];
} re_ca_uart_mosi_payload_buf_encoded_get_device_id_t;

/** @brief Buffer for the encoded 'all_params' payload. */
typedef struct re_ca_uart_mosi_payload_buf_encoded_all_params_t
{
    /** Buffer for the encoded 'all_params' payload. */
    uint8_t buf[RE_CA_UART_TX_BUF_LEN (
                                          RE_CA_UART_TX_DATA_LEN_CMD_ALL_PARAMS_WITH_MAX_ADV_LEN())];
} re_ca_uart_mosi_payload_buf_encoded_all_params_t;

/** @brief Buffer for the encoded 'led_ctrl' payload. */
typedef struct re_ca_uart_mosi_payload_buf_encoded_led_ctrl_t
{
    /** Buffer for the encoded 'led_ctrl' payload. */
    uint8_t buf[RE_CA_UART_TX_BUF_LEN (RE_CA_UART_TX_DATA_LEN_CMD_LED_CTRL())];
} re_ca_uart_mosi_payload_buf_encoded_led_ctrl_t;


/** @brief Buffer for the encoded 'device_id' payload. */
typedef struct re_ca_uart_miso_payload_buf_encoded_device_id_t
{
    /** Buffer for the encoded 'device_id' payload. */
    uint8_t buf[RE_CA_UART_TX_BUF_LEN (RE_CA_UART_TX_DATA_LEN_CMD_DEVICE_ID())];
} re_ca_uart_miso_payload_buf_encoded_device_id_t;

/** @brief Buffer for the encoded 'ack' payload. */
typedef struct re_ca_uart_miso_payload_buf_encoded_ack_t
{
    /** Buffer for the encoded 'ack' payload. */
    uint8_t buf[RE_CA_UART_TX_BUF_LEN (RE_CA_UART_TX_DATA_LEN_CMD_ACK())];
} re_ca_uart_miso_payload_buf_encoded_ack_t;

/** @brief Buffer for the encoded 'get_all_params' payload. */
typedef struct re_ca_uart_miso_payload_buf_encoded_get_all_params_t
{
    /** Buffer for the encoded 'get_all_params' payload. */
    uint8_t buf[RE_CA_UART_TX_BUF_LEN (RE_CA_UART_TX_DATA_LEN_CMD_GET_ALL_PARAMS())];
} re_ca_uart_miso_payload_buf_encoded_get_all_params_t;

#pragma pack(push,1)

#ifdef RE_CA_SUPPORT_OLD_CMD
/** @brief Manufacturer filter payload. */
typedef struct
{
    uint16_t manufacturer_id; //!< Manufacturer ID, MSB first. 0x0499 for Ruuvi.
} re_ca_uart_ble_filter_t;

/** @brief Enabled BLE Channels. */
typedef struct
{
    uint8_t ch37 : 1; //!< BLE Channel 37 enabled.
    uint8_t ch38 : 1; //!< BLE Channel 38 enabled.
    uint8_t ch39 : 1; //!< BLE Channel 39 enabled.
} re_ca_uart_ble_ch_t;

/** @brief Enabled BLE PHYs. */
typedef struct
{
    uint8_t ble_125kbps : 1; //!< BLE Channel 37 enabled.
    uint8_t ble_1mbps   : 1; //!< BLE Channel 38 enabled.
    uint8_t ble_2mbps   : 1; //!< BLE Channel 39 enabled.
} re_ca_uart_ble_phy_t;
#endif

/** @brief Enabled BLE Channel. */
typedef struct
{
    uint8_t state : 1; //!< BLE Channel enabled.
} re_ca_uart_ble_bool_t;

/** @brief Manufacturer filter payload. */
typedef struct
{
    uint16_t id; //!< Manufacturer ID, MSB first. 0x0499 for Ruuvi.
} re_ca_uart_ble_fltr_id_t;

/** @brief LED control payload. */
typedef struct
{
    uint16_t time_interval_ms; //!< LED ON time interval in milliseconds
} re_ca_uart_led_ctrl_t;

/** @brief Ack filter payload. */
typedef struct
{
    re_ca_uart_cmd_t cmd; //!< Type of command discriminating the union.
    re_ca_uart_ble_bool_t ack_state; //!< Ack state.
} re_ca_uart_ble_ack_t;

/** @brief All bool state filter payload. */
typedef struct
{
    re_ca_uart_ble_bool_t fltr_tags;    //!< Filter tag bool state.
    re_ca_uart_ble_bool_t coded_phy;    //!< Coded phy bool state.
    re_ca_uart_ble_bool_t scan_phy;     //!< Scanned phy bool state.
    re_ca_uart_ble_bool_t ext_payload;  //!< Pyload extension bool state.
    re_ca_uart_ble_bool_t ch_37;        //!< Channel 37 bool state.
    re_ca_uart_ble_bool_t ch_38;        //!< Channel 38 bool state.
    re_ca_uart_ble_bool_t ch_39;        //!< Channel 39 bool state.
} re_ca_uart_ble_all_bools_t;

#define RE_CA_UART_BLE_ALL_PARAMS_MAX_ADV_LEN_NO_LIMIT (0U) //!< No limit on max advertisement length.

/** @brief BLE config struct. */
typedef struct
{
    re_ca_uart_ble_fltr_id_t fltr_id;   //!< Filter id.
    re_ca_uart_ble_all_bools_t bools;   //!< All bool state in payload.
    uint8_t max_adv_len;                //!< Max advertisement length.
} re_ca_uart_ble_all_t;

/** @brief BLE device id struct. */
typedef struct
{
    uint64_t id;                        //!< DEVICE_ID, 64bits
    uint64_t addr;                      //!< DEVICE_ADDR, 64bits
} re_ca_uart_ble_id_t;

/** @brief BLE PHY */
typedef enum re_ca_uart_ble_phy_e
{
    RE_CA_UART_BLE_PHY_AUTO = 0, //!< Automatic PHY selection (BLE_GAP_PHY_AUTO).
    RE_CA_UART_BLE_PHY_1MBPS = 1, //!< 1 Mbps PHY (BLE_GAP_PHY_1MBPS).
    RE_CA_UART_BLE_PHY_2MBPS = 2, //!< 2 Mbps PHY (BLE_GAP_PHY_2MBPS).
    RE_CA_UART_BLE_PHY_CODED = 4, //!< Coded PHY (BLE_GAP_PHY_CODED).
    RE_CA_UART_BLE_PHY_NOT_SET = 0x0F, //!< PHY is not configured (RE_CA_UART_BLE_PHY_NOT_SET).
} re_ca_uart_ble_phy_e;

#define RE_CA_UART_BLE_GAP_POWER_LEVEL_INVALID (-64) /*!< Invalid TX power level (BLE_GAP_POWER_LEVEL_INVALID). */

#define RE_CA_UART_BLE_GAP_CHANNEL_INDEX_INVALID (0xFF) /*!< Invalid channel index. */

/** @brief Advertisement payload. */
typedef struct
{
    uint8_t mac[RE_CA_UART_MAC_BYTES]; //!< MAC address, always 6 bytes. MSB first.
    uint8_t adv[RE_CA_UART_ADV_BYTES]; //!< Advertisement, variable length.
    uint8_t adv_len;                   //!< Length of advertisement.
    int8_t rssi_db;                    //!< RSSI.

    /**
     * @brief Indicates the PHY on which the primary advertising
     *        packet was received. See BLE_GAP_PHYS.
     */
    re_ca_uart_ble_phy_e primary_phy: 4;

    /**
     * @brief Indicates the PHY on which the secondary advertising packet
     * was received. See BLE_GAP_PHYS.
     * This field is set to @ref RE_CA_UART_BLE_PHY_NOT_SET if no packets were
     * received on a secondary advertising channel.
     */
    re_ca_uart_ble_phy_e secondary_phy: 4;

    /**
     * @brief Channel Index on which the last advertising packet
     * is received (0-39).
     */
    uint8_t ch_index;

    bool is_coded_phy: 1; //!< True if Coded PHY was used

    /**
     * @brief TX Power reported by the advertiser in the last packet header
     * received. This field is set to RE_CA_UART_BLE_GAP_POWER_LEVEL_INVALID
     * if the last received packet did not contain the Tx Power field.
     * @note TX Power is only included in extended advertising packets.
     */
    int8_t tx_power: 7;
} re_ca_uart_ble_adv_t;

#if 0
/**
 * @brief  Structure of CA_UART data.
 *
 * @note: Order of elements here is not representative of order of serialized
 *        structure.  Data is serialized as STX LEN CMD PAYLOAD ETX.
 */
typedef struct
{
    uint8_t payload[RE_CA_UART_PAYLOAD_MAX_LEN]; //!< Command payload.
    uint8_t stx; //!< First byte, always fixed STX.
    uint8_t len; //!< Length of payload, + length(cmd) in legacy mode.
    uint8_t etx;    //!< Last byte always fixed ETX.
    re_ca_uart_cmd_t cmd; //!< Command to send.
} re_ca_uart_tx_t;
#endif

/** @brief Structure to contain command data.
 * MISRA deviation - use of union.
 * Reasoning for using union is to allow single data type to contain information
 * of incoming binary payload, type has to be interpreted at runtime.
 * Misinterpreting the payload is avoided via member cmd which is set by decoder.
 */
typedef struct
{
    re_ca_uart_cmd_t cmd; //!< Type of command discriminating the union.
    union // -V2514
    {
#ifdef RE_CA_SUPPORT_OLD_CMD
        re_ca_uart_ble_filter_t   filter;   //!< Filter param.
        re_ca_uart_ble_ch_t       channels; //!< Channel param.
        re_ca_uart_ble_phy_t      phys;     //!< Phy param.
#endif
        re_ca_uart_ble_adv_t      adv;          //!< Advertisement report param.
        re_ca_uart_ble_ack_t      ack;          //!< Ack param.
        re_ca_uart_ble_bool_t     bool_param;   //!< Bool param.
        re_ca_uart_ble_fltr_id_t  fltr_id_param;//!< Filter id param.
        re_ca_uart_led_ctrl_t     led_ctrl_param;//!< LED control param.
        re_ca_uart_ble_all_t      all_params;   //!< All param.
        re_ca_uart_ble_id_t       device_id;    //!< Device id report.
    } params; //!< Command payload.
} re_ca_uart_payload_t; //!< Structured payload.
#pragma pack(pop)

/**
 * @brief Encode given command with given parameters into buffer.
 *
 * @param[out] buffer Buffer to encode command into.
 * @param[in,out] buf_len Input: Size of buffer. Output: Number of bytes encoded.
 * @param[in]  payload Payload to encode.
 *
 * @retval RE_SUCCESS If encoded successfully.
 * @retval RE_ERROR_NULL If any of params is NULL.
 * @retval RE_ERROR_DATA_SIZE If buf_len is not enough to encode the payload.
 * @retval RE_ERROR_INVALID_PARAM If data cannot be encoded.
 */
re_status_t re_ca_uart_encode (uint8_t * const buffer, uint8_t * const buf_len,
                               const re_ca_uart_payload_t * const payload);

/**
 * @brief Decode given buffer to @ref re_ca_uart_payload_t.
 *
 * param[in]  buffer Buffer to decode command.
 * param[out] payload Pointer to the buffer to store the decoded payload.
 * param[out] params Parameters of command, type depends on cmd.
 */
re_status_t re_ca_uart_decode (const uint8_t * const buffer,
                               re_ca_uart_payload_t * const payload);

#endif // RUUVI_ENDPOINT_GW_UART_H
