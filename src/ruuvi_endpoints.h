#ifndef RUUVI_ENDPOINTS_H
#define RUUVI_ENDPOINTS_H

#include <stdint.h>

#define RE_SUCCESS 0
#define RE_ERROR_INVALID_PARAM (1<<4)  //!< Invalid Parameter
#define RE_ERROR_NULL          (1<<11) //!< Null Pointer
#define RE_ERROR_ENCODING      (1<<12) //!< Data encoding failed

typedef uint32_t re_status_t;

/**
 *  Ruuvi Standard Message consists of 11 bytes: 3 are a header, 8 are payload.
 *  Header has destination, source and type.
 *  Payload format is determined by the type of a message.
 */
#define RE_STANDARD_DESTINATION_INDEX   0
#define RE_STANDARD_SOURCE_INDEX        1
#define RE_STANDARD_TYPE_INDEX          2
#define RE_STANDARD_PAYLOAD_START_INDEX 3
#define RE_STANDARD_HEADER_LENGTH       3
#define RE_STANDARD_PAYLOAD_LENGTH      8
#define RE_STANDARD_MESSAGE_LENGTH    (RE_STANDARD_HEADER_LENGTH + RE_STANDARD_PAYLOAD_LENGTH)

/** @brief If endpoint is even, type of a message is considered a write. If it is odd, type is considered a read. */
#define RE_STANDARD_TYPE_READ_BIT              (1<<0)
#define RE_STANDARD_SENSOR_CONFIGURATION_WRITE (0x02)
#define RE_STANDARD_SENSOR_CONFIGURATION_READ  (RE_STANDARD_SENSOR_CONFIGURATION_WRITE | RE_STANDARD_TYPE_READ_BIT)
#define RE_STANDARD_SENSOR_OFFSET_WRITE        (0x04)
#define RE_STANDARD_SENSOR_OFFSET_READ         (RE_STANDARD_SENSOR_OFFSET_WRITE | RE_STANDARD_TYPE_READ_BIT)
#define RE_STANDARD_LOG_CONFIGURATION_WRITE    (0x06)
#define RE_STANDARD_LOG_CONFIGURATION_READ     (RE_STANDARD_LOG_CONFIGURATION_WRITE | RE_STANDARD_TYPE_READ_BIT)
#define RE_STANDARD_VALUE_WRITE                (0x08) //!< Sensor reads are acknowledged by setting type as value write
#define RE_STANDARD_VALUE_READ                 (RE_STANDARD_VALUE_WRITE | RE_STANDARD_TYPE_READ_BIT)
#define RE_STANDARD_LOG_VALUE_WRITE            (0x10)
#define RE_STANDARD_LOG_VALUE_READ             (RE_STANDARD_LOG_VALUE_WRITE | RE_STANDARD_TYPE_READ_BIT)
#define RE_STANDARD_TYPE_ERROR                 (0xFF) //!< internal error has occured

#define RE_STANDARD_DESTINATION_ACCELERATION   0x4A //!< XYZ acceleration combined
#define RE_STANDARD_DESTINATION_ACCELERATION_X 0x40 //!< X acceleration
#define RE_STANDARD_DESTINATION_ACCELERATION_Y 0x41 //!< Y acceleration
#define RE_STANDARD_DESTINATION_ACCELERATION_Z 0x42 //!< Z acceleration
#define RE_STANDARD_DESTINATION_GYRATION       0x4B //!< XYZ gyration combined
#define RE_STANDARD_DESTINATION_GYRATION_X     0x43 //!< X gyration
#define RE_STANDARD_DESTINATION_GYRATION_Y     0x44 //!< Y gyration
#define RE_STANDARD_DESTINATION_GYRATION_Z     0x45 //!< Z gyration
#define RE_STANDARD_DESTINATION_ADC_BATTERY    0x20 //!< ADC battery vs GND
#define RE_STANDARD_DESTINATION_TEMPERATURE    0x30 //!< Temperature
#define RE_STANDARD_DESTINATION_HUMIDITY       0x31 //!< Humidity
#define RE_STANDARD_DESTINATION_PRESSURE       0x32 //!< Pressure
#define RE_STANDARD_DESTINATION_ENVIRONMENTAL  0x3A //!< Temperature, humidity, air pressure combined.
#define RE_STANDARD_DESTINATION_RTC            0x21 //!< RTC value

#endif