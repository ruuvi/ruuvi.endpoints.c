if(IDF_VERSION_MAJOR GREATER_EQUAL 4)
    idf_component_register(
        SRCS "src/ruuvi_endpoints.c"
        SRCS "src/ruuvi_endpoints.h"
        SRCS "src/ruuvi_endpoint_3.c"
        SRCS "src/ruuvi_endpoint_3.h"
        SRCS "src/ruuvi_endpoint_5.c"
        SRCS "src/ruuvi_endpoint_5.h"
        SRCS "src/ruuvi_endpoint_6.c"
        SRCS "src/ruuvi_endpoint_6.h"
        SRCS "src/ruuvi_endpoint_c5.c"
        SRCS "src/ruuvi_endpoint_c5.h"
        SRCS "src/ruuvi_endpoint_e0.c"
        SRCS "src/ruuvi_endpoint_e0.h"
        SRCS "src/ruuvi_endpoint_f0.c"
        SRCS "src/ruuvi_endpoint_f0.h"
        SRCS "src/ruuvi_endpoint_ca_uart.c"
        SRCS "src/ruuvi_endpoint_ca_uart.h"
        SRCS "src/ruuvi_endpoint_ibeacon.c"
        SRCS "src/ruuvi_endpoint_ibeacon.h"
        INCLUDE_DIRS "src"
        )
elseif (DEFINED ENV{ZEPHYR_BASE})
    project(ruuvi.endpoints.c)
    zephyr_interface_library_named(ruuvi.endpoints.c)
    zephyr_library_sources(
            src/ruuvi_endpoints.c
            src/ruuvi_endpoint_3.c
            src/ruuvi_endpoint_5.c
            src/ruuvi_endpoint_6.c
            src/ruuvi_endpoint_c5.c
            src/ruuvi_endpoint_e0.c
            src/ruuvi_endpoint_f0.c
            src/ruuvi_endpoint_ca_uart.c
            src/ruuvi_endpoint_ibeacon.c
    )
    zephyr_library_include_directories(src)
    zephyr_include_directories(src)
else()
    set(COMPONENT_SRCDIRS src)
    set(COMPONENT_ADD_INCLUDEDIRS src)
    register_component()
endif()
