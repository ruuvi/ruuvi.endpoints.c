#include "application_config.h"
#if RUUVI_ENDPOINTS_ENABLED
#include <stddef.h>
#include "ruuvi_endpoints.h"

/** Sensor data handlers **/
//static message_handler p_battery_handler           = NULL;
//static message_handler p_rng_handler               = NULL;
//static message_handler p_rtc_handler               = NULL;
//static message_handler p_nfc_handler               = NULL;
//static message_handler p_gpio_handler              = NULL;
static message_handler p_led_handler                 = NULL;
//static message_handler p_temperature_handler       = NULL;
//static message_handler p_humidity_handler          = NULL;
//static message_handler p_pressure_handler          = NULL;
//static message_handler p_air_quality_handler       = NULL;
static message_handler p_environmental_handler       = NULL;
static message_handler p_acceleration_handler        = NULL;
static message_handler p_magnetism_handler           = NULL;
static message_handler p_gyration_handler            = NULL;
//static message_handler p_movement_detector_handler = NULL;
static message_handler p_app0_handler                = NULL;

/** Data traffic handlers **/
// static message_handler p_reply_handler       = NULL;
// static message_handler p_ble_adv_handler     = NULL;
// static message_handler p_ble_gatt_handler    = NULL;
// static message_handler p_ble_mesh_handler    = NULL;
// static message_handler p_proprietary_handler = NULL;
// static message_handler p_nfc_handler         = NULL;
// static message_handler p_ram_handler         = NULL;
// static message_handler p_flash_handler       = NULL;


/** Routes message to appropriate endpoint handler.
 *  Messages will send data to their configured transmission points
 **/
ruuvi_endpoint_status_t route_message(ruuvi_standard_message_t* const message)
{
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

  case MAGNETISM:
     if(p_magnetism_handler) {p_magnetism_handler(message); }
     else {unknown_handler(message); }
     break;

   case GYRATION:
     if(p_gyration_handler) {p_gyration_handler(message); }
     else {unknown_handler(message); }
     break;

  // case MOVEMENT_DETECTOR:
  //   if(p_movement_detector_handler) {p_movement_detector_handler(message); }
  //   else {unknown_handler(message); }
  //   break;

  case APP0:
    if (p_app0_handler) { p_app0_handler(message); }
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

void set_gyration_handler(message_handler handler)
{
  p_gyration_handler = handler;
}

void set_magnetism_handler(message_handler handler)
{
  p_magnetism_handler = handler;
}

void set_app0_handler(message_handler handler)
{
  p_app0_handler = handler;
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

// Mark payload as "unknown"
ruuvi_endpoint_status_t unknown_handler(ruuvi_standard_message_t* const message)
{
  message->type = UNKNOWN;

  return ENDPOINT_HANDLER_ERROR;
}

#endif