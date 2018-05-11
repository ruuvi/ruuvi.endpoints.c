#include "sdk_application_config.h"
#if RUUVI_ENDPOINTS
#include "ruuvi_endpoints.h"

#define PLATFORM_LOG_MODULE_NAME endpoints
#if MAIN_LOG_ENABLED
#define PLATFORM_LOG_LEVEL       MAIN_LOG_LEVEL
#define PLATFORM_LOG_INFO_COLOR  MAIN_INFO_COLOR
#else
#define PLATFORM_LOG_LEVEL       0
#endif
#include "platform_log.h"
PLATFORM_LOG_MODULE_REGISTER();


/** Sensor data handlers **/
//static message_handler p_battery_handler           = NULL;
//static message_handler p_rng_handler               = NULL;
//static message_handler p_rtc_handler               = NULL;
//static message_handler p_nfc_handler               = NULL;
//static message_handler p_gpio_handler              = NULL;
static message_handler p_led_handler               = NULL;
//static message_handler p_temperature_handler       = NULL;
//static message_handler p_humidity_handler          = NULL;
//static message_handler p_pressure_handler          = NULL;
//static message_handler p_air_quality_handler       = NULL;
static message_handler p_environmental_handler      = NULL;
static message_handler p_acceleration_handler       = NULL;
//static message_handler p_magnetometer_handler      = NULL;
//static message_handler p_gyroscope_handler         = NULL;
//static message_handler p_movement_detector_handler = NULL;
static message_handler p_mam_handler               = NULL;

/** Chain handler **/
//static message_handler p_chain_handler = NULL;

/** Data traffic handlers **/
// static message_handler p_reply_handler       = NULL;
// static message_handler p_ble_adv_handler     = NULL;
// static message_handler p_ble_gatt_handler    = NULL;
// static message_handler p_ble_mesh_handler    = NULL;
// static message_handler p_proprietary_handler = NULL;
// static message_handler p_nfc_handler         = NULL;
// static message_handler p_ram_handler         = NULL;
// static message_handler p_flash_handler       = NULL;

/** Scheduler handler to call message router **/
// TODO rename as incoming message handler and parse all messages through this function?
// void ble_gatt_scheduler_event_handler(void *p_event_data, uint16_t event_size)
// {
//   //TODO: Handle incoming bulk writes
//   ruuvi_standard_message_t message = {};
//   memcpy(&message, p_event_data, sizeof(message));
//   route_message(message);
// }

/** Routes message to appropriate endpoint handler.
 *  Messages will send data to their configured transmission points
 **/
ruuvi_endpoint_status_t route_message(ruuvi_standard_message_t* const message)
{
  PLATFORM_LOG_INFO("Routing message. %x, %x, %x, \r\n", message->destination_endpoint, message->source_endpoint, message->type);
  switch (message->destination_endpoint)
  {
  // case PLAINTEXT_MESSAGE:
  //   unknown_handler(message); // Application does not handle plain text - TODO: Not implemented hander?
  //   break;

  // case BATTERY:
  //   if(p_battery_handler) {p_battery_handler(message); }
  //   else {unknown_handler(message); }
  //   break;

  // case RNG:
  //   if(p_rng_handler) {p_rng_handler(message); }
  //   else {unknown_handler(message); }
  //   break;

  // case RTC:
  //   if(p_rtc_handler) {p_rtc_handler(message); }
  //   else {unknown_handler(message); }
  //   break;

  case LED:
    if (p_led_handler) { p_led_handler(message); }
    else { unknown_handler(message); }
    break;

  // case TEMPERATURE:
  //   NRF_LOG_DEBUG("Message is a temperature message.\r\n");
  //   if(p_temperature_handler) {p_temperature_handler(message); }
  //   else {unknown_handler(message); }
  //   break;

  // case HUMIDITY:
  //   if(p_humidity_handler) {p_humidity_handler(message); }
  //   else {unknown_handler(message); }
  //   break;

  // case PRESSURE:
  //   if(p_pressure_handler) {p_pressure_handler(message); }
  //   else {unknown_handler(message); }
  //   break;

  // case AIR_QUALITY:
  //   if(p_air_quality_handler) {p_air_quality_handler(message); }
  //   else {unknown_handler(message); }
  //   break;
  case ENVIRONMENTAL:
    if (p_environmental_handler) { p_environmental_handler(message); }
    else { unknown_handler(message); }
    break;

  case ACCELERATION:
    if (p_acceleration_handler) { p_acceleration_handler(message); }
    else { unknown_handler(message); }
    break;

  // case MAGNETOMETER:
  //   if(p_magnetometer_handler) {p_magnetometer_handler(message); }
  //   else {unknown_handler(message); }
  //   break;

  // case GYROSCOPE:
  //   if(p_gyroscope_handler) {p_gyroscope_handler(message); }
  //   else {unknown_handler(message); }
  //   break;

  // case MOVEMENT_DETECTOR:
  //   if(p_movement_detector_handler) {p_movement_detector_handler(message); }
  //   else {unknown_handler(message); }
  //   break;

  case MAM:
    if (p_mam_handler) { p_mam_handler(message); }
    else { unknown_handler(message); }
    break;

  default:
    //Call chain handler if applicable
    // if(ENDPOINT_CHAIN_OFFSET <= message.destination_endpoint &&
    //   (ENDPOINT_CHAIN_OFFSET + NUM_CHAIN_CHANNELS) > message.destination_endpoint &&
    //   p_chain_handler)
    // {
    //   p_chain_handler(message);
    // }
    // else
    // {
    unknown_handler(message);
    //}
    break;
  }
  return ENDPOINT_SUCCESS;
}

// void set_temperature_handler(message_handler handler)
// {
//   p_temperature_handler = handler;
// }

void set_led_handler(message_handler handler)
{
  p_led_handler = handler;
}

void set_acceleration_handler(message_handler handler)
{
  p_acceleration_handler = handler;
}

void set_environmental_handler(message_handler handler)
{
  p_environmental_handler = handler;
}

void set_mam_handler(message_handler handler)
{
  p_mam_handler = handler;
}

// void set_reply_handler(message_handler handler)
// {
//   p_reply_handler = handler;
// }

// void set_ble_adv_handler(message_handler handler)
// {
//   p_ble_adv_handler = handler;
// }

// void set_ble_gatt_handler(message_handler handler)
// {
//   p_ble_gatt_handler = handler;
// }

// void set_ble_mesh_handler(message_handler handler)
// {
//   p_ble_mesh_handler = handler;
// }

// void set_proprietary_handler(message_handler handler)
// {
//   p_proprietary_handler = handler;
// }

// void set_nfc_handler(message_handler handler)
// {
//   p_nfc_handler = handler;
// }

// void set_ram_handler(message_handler handler)
// {
//   p_ram_handler = handler;
// }

// void set_flash_handler(message_handler handler)
// {
//   p_flash_handler = handler;
// }

// void set_chain_handler(message_handler handler)
// {
//   p_chain_handler = handler;
// }

// message_handler get_reply_handler(void)
// {
//   return p_reply_handler;
// }

// message_handler get_ble_adv_handler(void)
// {
//   return p_ble_adv_handler;
// }

// message_handler get_ble_gatt_handler(void)
// {
//   return p_ble_gatt_handler;
// }

// message_handler get_ble_mesh_handler(void)
// {
//   return p_ble_mesh_handler;
// }

// message_handler get_proprietary_handler(void)
// {
//   return p_proprietary_handler;
// }

// message_handler get_nfc_handler(void)
// {
//   return p_nfc_handler;
// }

// message_handler get_ram_handler(void)
// {
//   return p_ram_handler;
// }

// message_handler get_flash_handler(void)
// {
//   return p_flash_handler;
// }

// message_handler get_chain_handler(void)
// {
//   return p_chain_handler;
// }

// Mark payload as "unknown"
ruuvi_endpoint_status_t unknown_handler(ruuvi_standard_message_t* const message)
{
  PLATFORM_LOG_INFO("Unknown message. %x, %x, %x, \r\n", message->destination_endpoint, message->source_endpoint, message->type);
  message->type = UNKNOWN;

  return ENDPOINT_HANDLER_ERROR;
}

#endif