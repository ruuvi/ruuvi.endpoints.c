#ifndef RUUVI_ENDPOINTS_H
#define RUUVI_ENDPOINTS_H

#include <stdint.h>

#define RUUVI_ENDPOINT_SUCCESS 0
#define RUUVI_ENDPOINT_ERROR_NULL (1<<11) ///< Null Pointer

typedef int32_t ruuvi_endpoint_status_t;

#endif