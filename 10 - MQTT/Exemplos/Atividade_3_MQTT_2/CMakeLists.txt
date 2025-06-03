# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(MQTT_2 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(MQTT_2 main.c main_auxiliar.c
        WIFI_/fila_circular.c
        WIFI_/rgb_pwm_control.c
        WIFI_/conexao.c
        OLED_/display.c
        OLED_/oled_utils.c
        OLED_/ssd1306_i2c.c
        OLED_/setup_oled.c
        WIFI_/mqtt_lwip.c
        estado_mqtt.c
        )

pico_set_program_name(MQTT_2 "MQTT_2")
pico_set_program_version(MQTT_2 "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(MQTT_2 0)
pico_enable_stdio_usb(MQTT_2 1)

# Add the standard library to the build
target_link_libraries(MQTT_2
        pico_stdlib
        pico_multicore
        pico_sync
        hardware_pwm
        pico_cyw43_arch_lwip_threadsafe_background
        hardware_i2c
        pico_lwip_mqtt
        )

# Add the standard include files to the build
target_include_directories(MQTT_2 PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/WIFI_
        ${CMAKE_CURRENT_LIST_DIR}/OLED_
)

# Add any user requested libraries
#target_link_libraries(MQTT_2)

pico_add_extra_outputs(MQTT_2)