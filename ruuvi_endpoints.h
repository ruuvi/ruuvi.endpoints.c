#ifndef RUUVI_ENDPOINTS_H
#define RUUVI_ENDPOINTS_H

#include <stdint.h>

typedef int32_t ruuvi_endpoint_status_t;

typedef enum{
  PLAINTEXT_MESSAGE       = 0x10, // Plaintext data for info, debug etc
  BATTERY                 = 0x20, // Battery state message
  RNG                     = 0x21, // Random number
  RTC                     = 0x22, // Real time clock
  NFC                     = 0x23, // NFC message
  GPIO                    = 0x24, // GPIO message
  LED                     = 0x2A, // LED message
  TEMPERATURE             = 0x31, // Temperature message
  HUMIDITY                = 0x32,
  PRESSURE                = 0x33,
  AIR_QUALITY             = 0x34,
  ENVIRONMENTAL           = 0x3A, // Aggregate of temperature, humidity, pressure. 2+2+4 bytes.
  ACCELERATION            = 0x40,
  MAGNETOMETER            = 0x41,
  GYROSCOPE               = 0x42,
  MOVEMENT_DETECTOR       = 0x43,
  APP0                    = 0xD0 // Application specific handler
}ruuvi_endpoint_t;

/** Possible message types. Commented ones are on roadmap, but not implemented any time soon **/
typedef enum{
  SENSOR_CONFIGURATION           = 0x01, // Configure sensor
  ACTUATOR_CONFIGRATION          = 0x02, // Configure actuator
  ACKNOWLEDGEMENT                = 0x03, // Acknowledge message
  //STATUS_QUERY                   = 0x04, // Query status from endpoint - returns current configuration
  DATA_QUERY                     = 0x05, // Query data from endpoint - returns latest data
  //LOG_QUERY                      = 0x06, // Query log from endpoint - initiates bulk write
  //CAPABILITY_QUERY               = 0x07, // Query endpoint capablities: samplerate, resolution, scale, log, power consumption with settings given in query
  //SAMPLERATE_RESPONSE            = 0x08, // Response with allowed sample rates
  //RESOLUTION_RESPONSE            = 0x09, // Response with allowed resolutions
  //SCALE_RESPONSE                 = 0x10, // Response with allowed scales
  //TARGET_RESPONSE                = 0x11, // Response with allowed targets
  //POWER_RESPONSE                 = 0x12, // Response with allowed power levels
  TIMESTAMP                      = 0x13, // Timestamp related to next event
  UNKNOWN                        = 0x14, // Unknown, may be a reply if incoming message was not understood
  ERROR                          = 0x15, // Error, payload may contain details
  //CHAIN_UPSTREAM_CONFIGURATION   = 0x16, // Configure a chain endpoint
  //CHAIN_DOWNSTREAM_CONFIGURATION = 0x17, // Pass a function pointer to call with new data from actual sensor
  UINT8                          = 0x80, // Array of uint8
  INT8                           = 0x81,
  UINT16                         = 0x82,
  INT16                          = 0x83,
  UINT32                         = 0x84,
  INT32                          = 0x85,
  UINT64                         = 0x86, // Single uint64
  INT64                          = 0x87,
  ASCII                          = 0x88  // ASCII array
}ruuvi_message_type_t;


/**
 * Return status
 */
typedef enum{
  ENDPOINT_SUCCESS         = 0,  // ok
  ENDPOINT_NOT_IMPLEMENTED = 1,  // not implememented yet
  ENDPOINT_UNKNOWN         = 2,  // unknown parameter
  ENDPOINT_NOT_SUPPORTED   = 4,  // not supported
  ENDPOINT_INVALID         = 8,  // Invalid parameter for some reason
  ENDPOINT_HANDLER_ERROR   = 16, // Error in data handler
  ENDPOINT_HANDLER_NULL    = 32  // Null ptr passed to handler
}ruuvi_endpoint_ret_t;

/**
 *
 */
typedef enum{
  RUUVI_ENDPOINT_CMD_RESET     = 0,
  RUUVI_ENDPOINT_CMD_START     = 1,
  RUUVI_ENDPOINT_CMD_PAUSE     = 2,
  RUUVI_ENDPOINT_CMD_RESUME    = 3,
  RUUVI_ENDPOINT_CMD_NO_CHANGE = 255
}ruuvi_endpoint_sensor_cmd_t;


/**
 * 11 byte message fits into BLE MESH single payload
 */
typedef struct __attribute__((packed)){
  uint8_t destination_endpoint;
  uint8_t source_endpoint;
  uint8_t type;
  uint8_t payload[8];
}ruuvi_standard_message_t;

typedef struct __attribute__((packed)){
  uint8_t sample_rate;
  uint8_t transmission_rate;
  uint8_t resolution;
  uint8_t scale;
  uint8_t dsp_function;
  uint8_t dsp_parameter;
  uint8_t target;
  uint8_t cmd;
}ruuvi_endpoint_sensor_configuration_t;

/**
 *  Configuration message for an actuator, fits into std message payoad
 */
typedef struct __attribute__((packed)){
  uint8_t channel;
  uint8_t duty_cycle;
  uint8_t reserved0;
  uint8_t reserved1;
  uint8_t reserved2;
  uint8_t reserved3;
  uint8_t reserved4;
  uint8_t reserved5;
}ruuvi_endpoint_actuator_configuration_t;

// Declare message handler type
typedef ruuvi_endpoint_status_t(*message_handler)(ruuvi_standard_message_t* const message);

// Pass query to appropriate handler
ruuvi_endpoint_status_t route_message(ruuvi_standard_message_t* const message);

// Return "not known" if FW cannot handle the destination of the message
ruuvi_endpoint_status_t unknown_handler(ruuvi_standard_message_t* const message);

// Peripheral handlers
void set_environmental_handler(message_handler handler);
void set_acceleration_handler(message_handler handler);
void set_led_handler(message_handler handler);
void set_app0_handler(message_handler handler);
void set_unknown_handler(message_handler handler);


// Data transmission handlers
// void set_ble_adv_handler(message_handler handler);
// void set_ble_gatt_handler(message_handler handler);
// void set_proprietary_handler(message_handler handler);
// void set_nfc_handler(message_handler handler);
// void set_reply_handler(message_handler handler);
// void set_ram_handler(message_handler handler);
// void set_flash_handler(message_handler handler);
// void set_chain_handler(message_handler handler);

// message_handler get_reply_handler(void);
// message_handler get_ble_adv_handler(void);
// message_handler get_ble_gatt_handler(void);
// message_handler get_ble_mesh_handler(void);
// message_handler get_proprietary_handler(void);
// message_handler get_nfc_handler(void);
// message_handler get_ram_handler(void);
// message_handler get_flash_handler(void);
// message_handler get_chain_handler(void);


#endif
