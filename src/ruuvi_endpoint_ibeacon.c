#include "ruuvi_endpoint_ibeacon.h"
#include <string.h>

re_status_t re_ibeacon_encode (uint8_t * const buffer,
                               const re_ibeacon_data_t * data)
{
    if (NULL == buffer  || NULL == data) { return RE_ERROR_NULL; }
    if (RE_IBEACON_INVALID_TX_POWER == data->tx_power) { return RE_ERROR_INVALID_PARAM; }

    for (int i = 0; i < RE_IBEACON_PROXIMITY_UUID_SIZE; i++)
    {
        buffer[RE_IBEACON_OFFSET_PROXIMITY_UUID + i] = data->proximity_uuid[i];
    }

    uint16_t major = data->major;
    buffer[RE_IBEACON_OFFSET_MAJOR_MSB] = (major >> 8);
    buffer[RE_IBEACON_OFFSET_MAJOR_LSB] = (major & 0xFF);

    uint16_t minor = data->minor;
    buffer[RE_IBEACON_OFFSET_MAJOR_MSB] = (minor >> 8);
    buffer[RE_IBEACON_OFFSET_MAJOR_LSB] = (minor & 0xFF);

    int8_t tx_power = data->tx_power;
    buffer[RE_IBEACON_OFFSET_TX_POWER] = tx_power;

    return RE_SUCCESS;
}
