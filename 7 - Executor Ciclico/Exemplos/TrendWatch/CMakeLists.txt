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

project(TrendWatch C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(TrendWatch main.c setup.c irq_handlers.c tarefa1_temp.c tarefa2_display.c
inc/display_utils.c
inc/big_string_drawer.c
inc/ssd1306_i2c.c
inc/font_big_logo_data.c
tarefa3_tendencia.c
tarefa4_controla_neopixel.c
testes_cores.c
LabNeoPixel/neopixel_driver.c
LabNeoPixel/efeitos.c
tarefa5_movel_gpio_deadline.c)

pico_set_program_name(TrendWatch "TrendWatch")
pico_set_program_version(TrendWatch "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(TrendWatch 0)
pico_enable_stdio_usb(TrendWatch 1)

# Add the standard library to the build
target_link_libraries(TrendWatch pico_stdlib hardware_adc
    hardware_dma
    hardware_irq
    hardware_watchdog
    hardware_i2c
    hardware_pio)

# Add the standard include files to the build
target_include_directories(TrendWatch PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/inc ${CMAKE_CURRENT_LIST_DIR}/LabNeoPixel)

# Add any user requested libraries
target_link_libraries(TrendWatch)

pico_add_extra_outputs(TrendWatch)

# Generate PIO header
pico_generate_pio_header(TrendWatch ${CMAKE_CURRENT_LIST_DIR}/LabNeoPixel/ws2818b.pio)