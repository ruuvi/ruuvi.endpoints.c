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
        SRCS "src/ruuvi_endpoint_ca_uart.c"
        SRCS "src/ruuvi_endpoint_ca_uart.h"
        SRCS "src/ruuvi_endpoint_ibeacon.c"
        SRCS "src/ruuvi_endpoint_ibeacon.h"
        INCLUDE_DIRS "src"
        )
elseif(CMAKE_PROJECT_NAME STREQUAL "ruuvi.node_nrf91.c")
    zephyr_include_directories(src)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/ruuvi_endpoints.c)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/ruuvi_endpoint_3.c)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/ruuvi_endpoint_5.c)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/ruuvi_endpoint_6.c)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/ruuvi_endpoint_c5.c)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/ruuvi_endpoint_ca_uart.c)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/ruuvi_endpoint_ibeacon.c)
elseif (DEFINED ENV{ZEPHYR_BASE})
    project(ruuvi.endpoints.c)
    zephyr_interface_library_named(ruuvi.endpoints.c)
    zephyr_library_sources(
            src/ruuvi_endpoints.c
            src/ruuvi_endpoint_3.c
            src/ruuvi_endpoint_5.c
            src/ruuvi_endpoint_6.c
            src/ruuvi_endpoint_c5.c
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
