#ifndef RUUVI_ENDPOINTS_H
#define RUUVI_ENDPOINTS_H

#include <stdint.h>

#define RUUVI_ENDPOINTS_SEMVER "0.1.0"     //!< SEMVER of endpoints.

#define RE_SUCCESS               (0U)
#define RE_ERROR_INVALID_PARAM   (1U << 4U)  //!< Invalid Parameter
#define RE_ERROR_NULL            (1U << 11U) //!< Null Pointer
#define RE_ERROR_ENCODING        (1U << 12U) //!< Data encoding failed
#define RE_ERROR_NOT_IMPLEMENTED (1U<<16U)   //!< Not implemented yet

typedef uint32_t re_status_t; //!< Status code

/**
 *  Ruuvi Standard Message consists of 11 bytes: 3 are a header, 8 are payload.
 *  Header has destination, source and type.
 *  Payload format is determined by the type of a message.
 */
#define RE_STANDARD_DESTINATION_INDEX   (0U)
#define RE_STANDARD_SOURCE_INDEX        (1U)
#define RE_STANDARD_TYPE_INDEX          (2U)
#define RE_STANDARD_PAYLOAD_START_INDEX (3U)
#define RE_STANDARD_HEADER_LENGTH       (3U)
#define RE_STANDARD_PAYLOAD_LENGTH      (8U)
#define RE_STANDARD_MESSAGE_LENGTH      (RE_STANDARD_HEADER_LENGTH + RE_STANDARD_PAYLOAD_LENGTH)

/** @brief If endpoint is even, type of a message is considered a write. If it is odd, type is considered a read. */
#define RE_STANDARD_TYPE_READ_BIT              (1U << 0U)
#define RE_STANDARD_SENSOR_CONFIGURATION_WRITE (0x02U)
#define RE_STANDARD_SENSOR_CONFIGURATION_READ  (RE_STANDARD_SENSOR_CONFIGURATION_WRITE | RE_STANDARD_TYPE_READ_BIT)
#define RE_STANDARD_SENSOR_OFFSET_WRITE        (0x04U)
#define RE_STANDARD_SENSOR_OFFSET_READ         (RE_STANDARD_SENSOR_OFFSET_WRITE | RE_STANDARD_TYPE_READ_BIT)
#define RE_STANDARD_LOG_CONFIGURATION_WRITE    (0x06U)
#define RE_STANDARD_LOG_CONFIGURATION_READ     (RE_STANDARD_LOG_CONFIGURATION_WRITE | RE_STANDARD_TYPE_READ_BIT)
#define RE_STANDARD_VALUE_WRITE                (0x08U) //!< Sensor reads are acknowledged by setting type as value write
#define RE_STANDARD_VALUE_READ                 (RE_STANDARD_VALUE_WRITE | RE_STANDARD_TYPE_READ_BIT)
#define RE_STANDARD_LOG_VALUE_WRITE            (0x10U)
#define RE_STANDARD_LOG_VALUE_READ             (RE_STANDARD_LOG_VALUE_WRITE | RE_STANDARD_TYPE_READ_BIT)
#define RE_STANDARD_TYPE_ERROR                 (0xFFU) //!< internal error has occured

#define RE_STANDARD_DESTINATION_ACCELERATION   (0x4AU) //!< XYZ acceleration combined
#define RE_STANDARD_DESTINATION_ACCELERATION_X (0x40U) //!< X acceleration
#define RE_STANDARD_DESTINATION_ACCELERATION_Y (0x41U) //!< Y acceleration
#define RE_STANDARD_DESTINATION_ACCELERATION_Z (0x42U) //!< Z acceleration
#define RE_STANDARD_DESTINATION_GYRATION       (0x4BU) //!< XYZ gyration combined
#define RE_STANDARD_DESTINATION_GYRATION_X     (0x43U) //!< X gyration
#define RE_STANDARD_DESTINATION_GYRATION_Y     (0x44U) //!< Y gyration
#define RE_STANDARD_DESTINATION_GYRATION_Z     (0x45U) //!< Z gyration
#define RE_STANDARD_DESTINATION_ADC_BATTERY    (0x20U) //!< ADC battery vs GND
#define RE_STANDARD_DESTINATION_TEMPERATURE    (0x30U) //!< Temperature
#define RE_STANDARD_DESTINATION_HUMIDITY       (0x31U) //!< Humidity
#define RE_STANDARD_DESTINATION_PRESSURE       (0x32U) //!< Pressure
#define RE_STANDARD_DESTINATION_ENVIRONMENTAL  (0x3AU) //!< Temp Humi Pres combined.
#define RE_STANDARD_DESTINATION_RTC            (0x21U) //!< RTC value

#endif