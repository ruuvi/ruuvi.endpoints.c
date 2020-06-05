#include "unity.h"

#include "ruuvi_endpoints.h"
#include "ruuvi_endpoint_ca_uart.h"

#include <string.h>

void setUp (void)
{
    // No action needed.
}

void tearDown (void)
{
    // No action needed.
}

// Ignore tests of unimplemented functionality.
#if 0
void test_ruuvi_endpoint_ca_uart_encode_filter_ruuvi (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t expected[] =
    {
        RE_CA_UART_STX,
        2 + CMD_IN_LEN,
        RE_CA_UART_SET_FLTR,
        0x99U, 0x04U,
        RE_CA_UART_ETX
    };
    uint8_t expected_size = sizeof (expected);
    re_ca_uart_cmd_t cmd = RE_CA_UART_SET_FLTR;
    re_ca_uart_ble_filter_t params = { .manufacturer_id = 0x0499};
    re_ca_uart_payload_t payload = {0};
    payload.cmd = cmd;
    payload.params.filter = params;
    uint8_t buffer[RE_CA_UART_TX_MAX_LEN] = {0};
    err_code = re_ca_uart_encode (buffer, &expected_size, &payload);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (expected_size == sizeof (expected));
    TEST_ASSERT (!memcmp (expected, buffer, sizeof (expected)));
}

void test_ruuvi_endpoint_ca_uart_encode_filter_clear (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t expected[] =
    {
        RE_CA_UART_STX,
        0 + CMD_IN_LEN,
        RE_CA_UART_CLR_FLTR,
        RE_CA_UART_ETX
    };
    uint8_t expected_size = sizeof (expected);
    re_ca_uart_cmd_t cmd = RE_CA_UART_CLR_FLTR;
    re_ca_uart_ble_filter_t params = { .manufacturer_id = RE_CA_UART_BLE_NOFILTER};
    re_ca_uart_payload_t payload = {0};
    payload.cmd = cmd;
    payload.params.filter = params;
    uint8_t buffer[RE_CA_UART_TX_MAX_LEN] = {0};
    err_code = re_ca_uart_encode (buffer, &expected_size, &payload);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (expected_size == sizeof (expected));
    TEST_ASSERT (!memcmp (expected, buffer, sizeof (expected)));
}

void test_ruuvi_endpoint_ca_uart_encode_null (void)
{
    re_status_t err_code = RE_SUCCESS;
    re_ca_uart_cmd_t cmd = RE_CA_UART_CLR_FLTR;
    re_ca_uart_ble_filter_t params = { .manufacturer_id = RE_CA_UART_BLE_NOFILTER};
    re_ca_uart_payload_t payload = {0};
    uint8_t size = 255U;
    payload.cmd = cmd;
    payload.params.filter = params;
    uint8_t buffer[RE_CA_UART_TX_MAX_LEN] = {0};
    err_code = re_ca_uart_encode (NULL, &size, &payload);
    TEST_ASSERT (RE_ERROR_NULL == err_code);
    err_code = re_ca_uart_encode (buffer, NULL, &payload);
    TEST_ASSERT (RE_ERROR_NULL == err_code);
    err_code = re_ca_uart_encode (buffer, &size, NULL);
    TEST_ASSERT (RE_ERROR_NULL == err_code);
}

void test_ruuvi_endpoint_ca_uart_channels_encode (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t expected[] =
    {
        RE_CA_UART_STX,
        5 + CMD_IN_LEN,
        RE_CA_UART_SET_CH,
        0x00U, 0x00U, 0x00U, 0x00U, 0xA0U,
        RE_CA_UART_ETX
    };
    uint8_t expected_size = sizeof (expected);
    re_ca_uart_cmd_t cmd = RE_CA_UART_SET_CH;
    re_ca_uart_ble_ch_t params =
    {
        .ch37 = 1,
        .ch38 = 0,
        .ch39 = 1
    };
    re_ca_uart_payload_t payload = {0};
    payload.cmd = cmd;
    payload.params.channels = params;
    uint8_t buffer[RE_CA_UART_TX_MAX_LEN] = {0};
    err_code = re_ca_uart_encode (buffer, &expected_size, &payload);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (expected_size == sizeof (expected));
    TEST_ASSERT (!memcmp (expected, buffer, sizeof (expected)));
}

void test_ruuvi_endpoint_ca_uart_phy_encode (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t expected[] =
    {
        RE_CA_UART_STX,
        1 + CMD_IN_LEN,
        RE_CA_UART_SET_PHY,
        0x07U,
        RE_CA_UART_ETX
    };
    uint8_t expected_size = sizeof (expected);
    re_ca_uart_cmd_t cmd = RE_CA_UART_SET_PHY;
    re_ca_uart_ble_phy_t params =
    {
        .ble_125kbps = 1,
        .ble_1mbps = 0,
        .ble_2mbps = 1
    };
    re_ca_uart_payload_t payload = {0};
    payload.cmd = cmd;
    payload.params.phys = params;
    uint8_t buffer[RE_CA_UART_TX_MAX_LEN] = {0};
    err_code = re_ca_uart_encode (buffer, &expected_size, &payload);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (expected_size == sizeof (expected));
    TEST_ASSERT (!memcmp (expected, buffer, sizeof (expected)));
}
#endif

void test_ruuvi_endpoint_ca_uart_adv_encode (void)
{
    re_status_t err_code = RE_SUCCESS;
    const uint8_t expected_size = 41 + CMD_IN_LEN;
    uint8_t expected[] =
    {
        RE_CA_UART_STX,
        expected_size,
        RE_CA_UART_ADV_RPRT,
        0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U, 0x8DU, RE_CA_UART_FIELD_DELIMITER, //!< MAC
        0x02U, 0x01U, 0x04U, 0x1BU, 0xFFU, 0x99U, 0x04U, 0x05U, 0x0FU, 0x27U, 0x40U,
        0x35U, 0xC4U, 0x54U, RE_CA_UART_FIELD_DELIMITER, 0x50U, 0x00U, 0xC8U, 0xFCU,
        0x20U, 0xA4U, 0x56U, 0xF0U, 0x30U, 0xE5U, 0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U,
        0x8DU, RE_CA_UART_FIELD_DELIMITER, //!< Data
        0xD8U, RE_CA_UART_FIELD_DELIMITER, //RSSI
        RE_CA_UART_ETX
    };
    re_ca_uart_cmd_t cmd = RE_CA_UART_ADV_RPRT;
    re_ca_uart_ble_adv_t params =
    {
        { 0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U, 0x8DU }, //!< MAC
        {
            0x02U, 0x01U, 0x04U, 0x1BU, 0xFFU, 0x99U, 0x04U, 0x05U, 0x0FU, 0x27U, 0x40U,
            0x35U, 0xC4U, 0x54U, RE_CA_UART_FIELD_DELIMITER, 0x50U, 0x00U, 0xC8U, 0xFCU,
            0x20U, 0xA4U, 0x56U, 0xF0U, 0x30U, 0xE5U, 0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U,
            0x8DU
        }, //!< Data
        31, //!< Data length
        -40//RSSI
    };
    re_ca_uart_payload_t payload = {0};;
    payload.cmd = cmd;
    payload.params.adv = params;
    uint8_t buffer[RE_CA_UART_TX_MAX_LEN] = {0};
    uint8_t buffer_len = sizeof (buffer);
    err_code = re_ca_uart_encode (buffer, &buffer_len, &payload);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (buffer_len == sizeof (expected));
    TEST_ASSERT (!memcmp (expected, buffer, sizeof (expected)));
}

void test_ruuvi_endpoint_ca_uart_decode_filter_ruuvi (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t data[] =
    {
        RE_CA_UART_STX,
        2 + CMD_IN_LEN,
        RE_CA_UART_SET_FLTR,
        0x99U, 0x04U,
        RE_CA_UART_ETX
    };
    re_ca_uart_ble_filter_t expect_params = { .manufacturer_id = 0x0499U};
    re_ca_uart_cmd_t expect_cmd = RE_CA_UART_SET_FLTR;
    re_ca_uart_payload_t payload = {0};
    err_code = re_ca_uart_decode (data, &payload);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (!memcmp (&expect_params, &payload.params.filter, sizeof (expect_params)));
    TEST_ASSERT (!memcmp (&expect_cmd, &payload.cmd, sizeof (expect_cmd)));
}

void test_ruuvi_endpoint_ca_uart_decode_filter_invalid_ruuvi (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t data[] =
    {
        RE_CA_UART_STX,
        0 + CMD_IN_LEN,
        RE_CA_UART_SET_FLTR,
        0x99U, 0x04U,
        RE_CA_UART_ETX
    };
    re_ca_uart_ble_filter_t expect_params = { .manufacturer_id = 0x0499U};
    re_ca_uart_cmd_t expect_cmd = RE_CA_UART_SET_FLTR;
    re_ca_uart_payload_t payload = {0};
    err_code = re_ca_uart_decode (data, &payload);
    TEST_ASSERT (RE_ERROR_DECODING == err_code);
    TEST_ASSERT (memcmp (&expect_params, &payload.params.filter, sizeof (expect_params)));
    TEST_ASSERT (memcmp (&expect_cmd, &payload.cmd, sizeof (expect_cmd)));
}

void test_ruuvi_endpoint_ca_uart_decode_filter_clear (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t data[] =
    {
        RE_CA_UART_STX,
        0 + CMD_IN_LEN,
        RE_CA_UART_CLR_FLTR,
        RE_CA_UART_ETX
    };
    re_ca_uart_ble_filter_t expect_params = { .manufacturer_id = 0x0000};
    re_ca_uart_cmd_t expect_cmd = RE_CA_UART_CLR_FLTR;
    re_ca_uart_payload_t payload = {0};
    err_code = re_ca_uart_decode (data, &payload);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (!memcmp (&expect_params, &payload.params.filter, sizeof (expect_params)));
    TEST_ASSERT (!memcmp (&expect_cmd, &payload.cmd, sizeof (expect_cmd)));
}

void test_ruuvi_endpoint_ca_uart_decode_filter_invalid_clear (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t data[] =
    {
        RE_CA_UART_STX,
        2 + CMD_IN_LEN,
        RE_CA_UART_CLR_FLTR,
        RE_CA_UART_ETX
    };
    re_ca_uart_cmd_t expect_cmd = RE_CA_UART_CLR_FLTR;
    re_ca_uart_payload_t payload = {0};
    err_code = re_ca_uart_decode (data, &payload);
    TEST_ASSERT (RE_ERROR_DECODING == err_code);
    TEST_ASSERT (memcmp (&expect_cmd, &payload.cmd, sizeof (expect_cmd)));
}

void test_ruuvi_endpoint_ca_uart_decode_null (void)
{
    re_status_t err_code = RE_SUCCESS;
    re_ca_uart_payload_t payload = {0};
    uint8_t buffer[RE_CA_UART_TX_MAX_LEN] = {0};
    err_code = re_ca_uart_decode (NULL, &payload);
    TEST_ASSERT (RE_ERROR_NULL == err_code);
    err_code = re_ca_uart_decode (buffer, NULL);
    TEST_ASSERT (RE_ERROR_NULL == err_code);
}

void test_ruuvi_endpoint_ca_uart_channels_decode (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t data[] =
    {
        RE_CA_UART_STX,
        5 + CMD_IN_LEN,
        RE_CA_UART_SET_CH,
        0x00U, 0x00U, 0x00U, 0x00U, 0xA0U,
        RE_CA_UART_ETX
    };
    re_ca_uart_ble_ch_t expect_params =
    {
        .ch37 = 1,
        .ch38 = 0,
        .ch39 = 1
    };
    re_ca_uart_cmd_t expect_cmd = RE_CA_UART_SET_CH;
    re_ca_uart_payload_t payload = {0};
    err_code = re_ca_uart_decode (data, &payload);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (!memcmp (&expect_params, &payload.params.channels, sizeof (expect_params)));
    TEST_ASSERT (!memcmp (&expect_cmd, &payload.cmd, sizeof (expect_cmd)));
}

void test_ruuvi_endpoint_ca_uart_channels_decode_invalid (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t data[] =
    {
        RE_CA_UART_STX,
        0 + CMD_IN_LEN,
        RE_CA_UART_SET_CH,
        0x00U, 0x00U, 0x00U, 0x00U, 0xA0U,
        RE_CA_UART_ETX
    };
    re_ca_uart_ble_ch_t expect_params =
    {
        .ch37 = 1,
        .ch38 = 0,
        .ch39 = 1
    };
    re_ca_uart_cmd_t expect_cmd = RE_CA_UART_SET_CH;
    re_ca_uart_payload_t payload = {0};
    err_code = re_ca_uart_decode (data, &payload);
    TEST_ASSERT (RE_ERROR_DECODING == err_code);
    TEST_ASSERT (memcmp (&expect_params, &payload.params.channels, sizeof (expect_params)));
    TEST_ASSERT (memcmp (&expect_cmd, &payload.cmd, sizeof (expect_cmd)));
}

void test_ruuvi_endpoint_ca_uart_phy_decode (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t data[] =
    {
        RE_CA_UART_STX,
        1 + CMD_IN_LEN,
        RE_CA_UART_SET_PHY,
        0x05U,
        RE_CA_UART_ETX
    };
    re_ca_uart_ble_phy_t expect_params =
    {
        .ble_125kbps = 1,
        .ble_1mbps = 0,
        .ble_2mbps = 1
    };
    re_ca_uart_cmd_t expect_cmd = RE_CA_UART_SET_PHY;
    re_ca_uart_payload_t payload = {0};
    err_code = re_ca_uart_decode (data, &payload);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (!memcmp (&expect_params, &payload.params.phys, sizeof (expect_params)));
    TEST_ASSERT (!memcmp (&expect_cmd, &payload.cmd, sizeof (expect_cmd)));
}

void test_ruuvi_endpoint_ca_uart_phy_decode_invalid (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t data[] =
    {
        RE_CA_UART_STX,
        0 + CMD_IN_LEN,
        RE_CA_UART_SET_PHY,
        0x05U,
        RE_CA_UART_ETX
    };
    re_ca_uart_ble_phy_t expect_params =
    {
        .ble_125kbps = 1,
        .ble_1mbps = 0,
        .ble_2mbps = 1
    };
    re_ca_uart_cmd_t expect_cmd = RE_CA_UART_SET_PHY;
    re_ca_uart_payload_t payload = {0};
    err_code = re_ca_uart_decode (data, &payload);
    TEST_ASSERT (RE_ERROR_DECODING == err_code);
    TEST_ASSERT (memcmp (&expect_params, &payload.params.phys, sizeof (expect_params)));
    TEST_ASSERT (memcmp (&expect_cmd, &payload.cmd, sizeof (expect_cmd)));
}

void test_ruuvi_endpoint_ca_uart_decode_noetx (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t data[] =
    {
        RE_CA_UART_STX,
        0 + CMD_IN_LEN,
        RE_CA_UART_SET_PHY,
        0x05U
    };
    re_ca_uart_ble_phy_t expect_params =
    {
        .ble_125kbps = 1,
        .ble_1mbps = 0,
        .ble_2mbps = 1
    };
    re_ca_uart_cmd_t expect_cmd = RE_CA_UART_SET_PHY;
    re_ca_uart_payload_t payload = {0};
    err_code = re_ca_uart_decode (data, &payload);
    TEST_ASSERT (RE_ERROR_DECODING == err_code);
    TEST_ASSERT (memcmp (&expect_params, &payload.params.phys, sizeof (expect_params)));
    TEST_ASSERT (memcmp (&expect_cmd, &payload.cmd, sizeof (expect_cmd)));
}

void test_ruuvi_endpoint_ca_uart_adv_decode (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t data[] =
    {
        RE_CA_UART_STX,
        41U + CMD_IN_LEN,
        RE_CA_UART_ADV_RPRT,
        0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U, 0x8DU, RE_CA_UART_FIELD_DELIMITER, //!< MAC
        0x02U, 0x01U, 0x04U, 0x1BU, 0xFFU, 0x99U, 0x04U, 0x05U, 0x0FU, 0x27U, 0x40U,
        0x35U, 0xC4U, 0x54U, 0x00U, 0x50U, 0x00U, 0xC8U, 0xFCU, 0x20U, 0xA4U, 0x56U,
        0xF0U, 0x30U, 0xE5U, 0xC9U, 0x44U, 0x54U, RE_CA_UART_FIELD_DELIMITER, 0xE3U, 0x8DU,
        RE_CA_UART_FIELD_DELIMITER, //!< Data
        0xD8U, RE_CA_UART_FIELD_DELIMITER, //RSSI
        RE_CA_UART_ETX
    };
    re_ca_uart_ble_adv_t expect_params =
    {
        { 0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U, 0x8DU }, //!< MAC
        {
            0x02U, 0x01U, 0x04U, 0x1BU, 0xFFU, 0x99U, 0x04U, 0x05U, 0x0FU, 0x27U, 0x40U,
            0x35U, 0xC4U, 0x54U, 0x00U, 0x50U, 0x00U, 0xC8U, 0xFCU, 0x20U, 0xA4U, 0x56U,
            0xF0U, 0x30U, 0xE5U, 0xC9U, 0x44U, 0x54U, RE_CA_UART_FIELD_DELIMITER, 0xE3U,
            0x8DU
        },   //!< Data
        31U, //!< Data length
        -40  //RSSI
    };
    re_ca_uart_cmd_t expect_cmd = RE_CA_UART_ADV_RPRT;
    re_ca_uart_payload_t payload = {0};
    err_code = re_ca_uart_decode (data, &payload);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (!memcmp (&expect_params, &payload.params.adv, sizeof (expect_params)));
    TEST_ASSERT (!memcmp (&expect_cmd, &payload.cmd, sizeof (expect_cmd)));
}

void test_ruuvi_endpoint_ca_uart_adv_decode_nodelimiter1 (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t data[] =
    {
        RE_CA_UART_STX,
        38U + CMD_IN_LEN,
        RE_CA_UART_ADV_RPRT,
        0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U, 0x8DU, //!< MAC
        0x02U, 0x01U, 0x04U, 0x1BU, 0xFFU, 0x99U, 0x04U, 0x05U, 0x0FU, 0x27U, 0x40U,
        0x35U, 0xC4U, 0x54U, 0x00U, 0x50U, 0x00U, 0xC8U, 0xFCU, 0x20U, 0xA4U, 0x56U,
        0xF0U, 0x30U, 0xE5U, 0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U, 0x8DU, //!< Data
        0xD8U, //RSSI
        RE_CA_UART_ETX
    };
    re_ca_uart_ble_adv_t expect_params =
    {
        { 0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U, 0x8DU }, //!< MAC
        {
            0x02U, 0x01U, 0x04U, 0x1BU, 0xFFU, 0x99U, 0x04U, 0x05U, 0x0FU, 0x27U, 0x40U,
            0x35U, 0xC4U, 0x54U, 0x00U, 0x50U, 0x00U, 0xC8U, 0xFCU, 0x20U, 0xA4U, 0x56U,
            0xF0U, 0x30U, 0xE5U, 0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U, 0x8DU
        },   //!< Data
        31U, //!< Data length
        -40  //RSSI
    };
    re_ca_uart_cmd_t expect_cmd = RE_CA_UART_ADV_RPRT;
    re_ca_uart_payload_t payload = {0};
    err_code = re_ca_uart_decode (data, &payload);
    TEST_ASSERT (RE_ERROR_DECODING == err_code);
    TEST_ASSERT (memcmp (&expect_params, &payload.params.adv, sizeof (expect_params)));
    TEST_ASSERT (memcmp (&expect_cmd, &payload.cmd, sizeof (expect_cmd)));
}

void test_ruuvi_endpoint_ca_uart_adv_decode_nodelimiter2 (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t data[] =
    {
        RE_CA_UART_STX,
        40U + CMD_IN_LEN,
        RE_CA_UART_ADV_RPRT,
        0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U, 0x8DU, RE_CA_UART_FIELD_DELIMITER, //!< MAC
        0x02U, 0x01U, 0x04U, 0x1BU, 0xFFU, 0x99U, 0x04U, 0x05U, 0x0FU, 0x27U, 0x40U,
        0x35U, 0xC4U, 0x54U, 0x00U, 0x50U, 0x00U, 0xC8U, 0xFCU, 0x20U, 0xA4U, 0x56U,
        0xF0U, 0x30U, 0xE5U, 0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U, 0x8DU, //!< Data
        0xD8U, RE_CA_UART_FIELD_DELIMITER, //RSSI
        RE_CA_UART_ETX
    };
    re_ca_uart_ble_adv_t expect_params =
    {
        { 0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U, 0x8DU }, //!< MAC
        {
            0x02U, 0x01U, 0x04U, 0x1BU, 0xFFU, 0x99U, 0x04U, 0x05U, 0x0FU, 0x27U, 0x40U,
            0x35U, 0xC4U, 0x54U, 0x00U, 0x50U, 0x00U, 0xC8U, 0xFCU, 0x20U, 0xA4U, 0x56U,
            0xF0U, 0x30U, 0xE5U, 0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U, 0x8DU
        },   //!< Data
        31U, //!< Data length
        -40  //RSSI
    };
    re_ca_uart_cmd_t expect_cmd = RE_CA_UART_ADV_RPRT;
    re_ca_uart_payload_t payload = {0};
    err_code = re_ca_uart_decode (data, &payload);
    TEST_ASSERT (RE_ERROR_DECODING == err_code);
    TEST_ASSERT (memcmp (&expect_params, &payload.params.adv, sizeof (expect_params)));
    TEST_ASSERT (memcmp (&expect_cmd, &payload.cmd, sizeof (expect_cmd)));
}

void test_ruuvi_endpoint_ca_uart_adv_decode_nodelimiter3 (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t data[] =
    {
        RE_CA_UART_STX,
        41U + CMD_IN_LEN,
        RE_CA_UART_ADV_RPRT,
        0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U, 0x8DU, RE_CA_UART_FIELD_DELIMITER, //!< MAC
        0x02U, 0x01U, 0x04U, 0x1BU, 0xFFU, 0x99U, 0x04U, 0x05U, 0x0FU, 0x27U, 0x40U,
        0x35U, 0xC4U, 0x54U, 0x00U, 0x50U, 0x00U, 0xC8U, 0xFCU, 0x20U, 0xA4U, 0x56U,
        0xF0U, 0x30U, 0xE5U, 0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U, 0x8DU,
        RE_CA_UART_FIELD_DELIMITER, //!< Data
        0xD8U, RE_CA_UART_ETX, //RSSI
        RE_CA_UART_ETX
    };
    re_ca_uart_ble_adv_t expect_params =
    {
        { 0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U, 0x8DU }, //!< MAC
        {
            0x02U, 0x01U, 0x04U, 0x1BU, 0xFFU, 0x99U, 0x04U, 0x05U, 0x0FU, 0x27U, 0x40U,
            0x35U, 0xC4U, 0x54U, 0x00U, 0x50U, 0x00U, 0xC8U, 0xFCU, 0x20U, 0xA4U, 0x56U,
            0xF0U, 0x30U, 0xE5U, 0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U, 0x8DU
        },   //!< Data
        31U, //!< Data length
        -40  //RSSI
    };
    re_ca_uart_cmd_t expect_cmd = RE_CA_UART_ADV_RPRT;
    re_ca_uart_payload_t payload = {0};
    err_code = re_ca_uart_decode (data, &payload);
    TEST_ASSERT (RE_ERROR_DECODING == err_code);
    TEST_ASSERT (memcmp (&expect_params, &payload.params.adv, sizeof (expect_params)));
    TEST_ASSERT (memcmp (&expect_cmd, &payload.cmd, sizeof (expect_cmd)));
}

void test_ruuvi_endpoint_ca_uart_adv_decode_toolong (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t data[] =
    {
        RE_CA_UART_STX,
        71U + CMD_IN_LEN,
        RE_CA_UART_ADV_RPRT,
        0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U, 0x8DU, RE_CA_UART_FIELD_DELIMITER, //!< MAC
        0x02U, 0x01U, 0x04U, 0x1BU, 0xFFU, 0x99U, 0x04U, 0x05U, 0x0FU, 0x27U, 0x40U,
        0x35U, 0xC4U, 0x54U, 0x00U, 0x50U, 0x00U, 0xC8U, 0xFCU, 0x20U, 0xA4U, 0x56U,
        0xF0U, 0x30U, 0xE5U, 0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U, 0x8DU,
        0x02U, 0x01U, 0x04U, 0x1BU, 0xFFU, 0x99U, 0x04U, 0x05U, 0x0FU, 0x27U, 0x40U,
        0x35U, 0xC4U, 0x54U, 0x00U, 0x50U, 0x00U, 0xC8U, 0xFCU, 0x20U, 0xA4U, 0x56U,
        0xF0U, 0x30U, 0xE5U, 0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U, 0x8DU,
        RE_CA_UART_FIELD_DELIMITER, //!< Data
        0xD8U, //RSSI
        RE_CA_UART_ETX
    };
    re_ca_uart_ble_adv_t expect_params =
    {
        { 0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U, 0x8DU }, //!< MAC
        {
            0x02U, 0x01U, 0x04U, 0x1BU, 0xFFU, 0x99U, 0x04U, 0x05U, 0x0FU, 0x27U, 0x40U,
            0x35U, 0xC4U, 0x54U, 0x00U, 0x50U, 0x00U, 0xC8U, 0xFCU, 0x20U, 0xA4U, 0x56U,
            0xF0U, 0x30U, 0xE5U, 0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U, 0x8DU
        },   //!< Data
        31U, //!< Data length
        -40  //RSSI
    };
    re_ca_uart_cmd_t expect_cmd = RE_CA_UART_ADV_RPRT;
    re_ca_uart_payload_t payload = {0};
    err_code = re_ca_uart_decode (data, &payload);
    TEST_ASSERT (RE_ERROR_DECODING == err_code);
    TEST_ASSERT (memcmp (&expect_params, &payload.params.adv, sizeof (expect_params)));
    TEST_ASSERT (memcmp (&expect_cmd, &payload.cmd, sizeof (expect_cmd)));
}

void test_ruuvi_endpoint_ca_uart_adv_decode_nodata (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t data[] =
    {
        RE_CA_UART_STX,
        10U + CMD_IN_LEN,
        RE_CA_UART_ADV_RPRT,
        0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U, 0x8DU, RE_CA_UART_FIELD_DELIMITER, //!< MAC
        RE_CA_UART_FIELD_DELIMITER, //!< Data
        0xD8U, RE_CA_UART_FIELD_DELIMITER, //RSSI
        RE_CA_UART_ETX
    };
    re_ca_uart_ble_adv_t expect_params =
    {
        { 0xC9U, 0x44U, 0x54U, 0x29U, 0xE3U, 0x8DU }, //!< MAC
        {
        },   //!< Data
        0U, //!< Data length
        -40  //RSSI
    };
    re_ca_uart_cmd_t expect_cmd = RE_CA_UART_ADV_RPRT;
    re_ca_uart_payload_t payload = {0};
    err_code = re_ca_uart_decode (data, &payload);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (!memcmp (&expect_params, &payload.params.adv, sizeof (expect_params)));
    TEST_ASSERT (!memcmp (&expect_cmd, &payload.cmd, sizeof (expect_cmd)));
}