cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(bitdoglab_dht11 C CXX ASM)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_executable(bitdoglab_dht11
    main.c
    dht11.c
)

target_link_libraries(bitdoglab_dht11 pico_stdlib)

pico_enable_stdio_usb(bitdoglab_dht11 1)
pico_enable_stdio_uart(bitdoglab_dht11 0)

pico_add_extra_outputs(bitdoglab_dht11)
