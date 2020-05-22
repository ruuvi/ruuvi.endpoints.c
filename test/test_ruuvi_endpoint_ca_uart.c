#include "unity.h"

#include "ruuvi_endpoints.h"
#include "ruuvi_endpoint_ca_uart.h"

#include <string.h>

#if RE_CA_UART_LEGACY_MODE
#   define CMD_IN_LEN (1U) //!< Command is included in data length.
#else
#   define CMD_IN_LEN (0U) //!< Command is not included in data length.
#endif

void setUp (void)
{
    // No action needed.
}

void tearDown (void)
{
    // No action needed.
}

void test_ruuvi_endpoint_ca_uart_encode_filter_ruuvi (void)
{
    re_status_t err_code = RE_SUCCESS;
    uint8_t expected[] =
    {
        RE_CA_UART_STX,
        2 + CMD_IN_LEN,
        RE_CA_UART_SET_FLTR,
        0x99, 0x04,
        RE_CA_UART_ETX
    };
    re_ca_uart_cmd_t cmd = RE_CA_UART_SET_FLTR;
    re_ca_uart_ble_filter_t params = { .manufacturer_id = 0x0499};
    uint8_t buffer[RE_CA_UART_TX_MAX_LEN] = {0};
    err_code = re_ca_uart_encode (buffer, &cmd, &params);
    TEST_ASSERT (RE_SUCCESS == err_code);
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
    re_ca_uart_cmd_t cmd = RE_CA_UART_CLR_FLTR;
    re_ca_uart_ble_filter_t params = { .manufacturer_id = RE_CA_UART_BLE_NOFILTER};
    uint8_t buffer[RE_CA_UART_TX_MAX_LEN] = {0};
    err_code = re_ca_uart_encode (buffer, &cmd, &params);
    TEST_ASSERT (RE_SUCCESS == err_code);
    TEST_ASSERT (!memcmp (expected, buffer, sizeof (expected)));
}

void test_ruuvi_endpoint_ca_uart_encode_null (void)
{
    re_status_t err_code = RE_SUCCESS;
    re_ca_uart_cmd_t cmd = RE_CA_UART_CLR_FLTR;
    re_ca_uart_ble_filter_t params = { .manufacturer_id = RE_CA_UART_BLE_NOFILTER};
    uint8_t buffer[RE_CA_UART_TX_MAX_LEN] = {0};
    err_code = re_ca_uart_encode (NULL, &cmd, &params);
    TEST_ASSERT (RE_ERROR_NULL == err_code);
    err_code = re_ca_uart_encode (buffer, NULL, &params);
    TEST_ASSERT (RE_ERROR_NULL == err_code);
    err_code = re_ca_uart_encode (buffer, &cmd, NULL);
    TEST_ASSERT (RE_ERROR_NULL == err_code);
}
