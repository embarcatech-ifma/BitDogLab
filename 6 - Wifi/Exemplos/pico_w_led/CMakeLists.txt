# Generated CMake Pico project file

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
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# Nome do projeto
project(pico_w_conect_ip C CXX ASM)

# Inicializa o SDK do Pico
pico_sdk_init()

# Adiciona o executável com base no nome do projeto
add_executable(pico_w_conect_ip
    pico_w_conect_ip.c
)

# Define o nome do programa e versão
pico_set_program_name(pico_w_conect_ip "pico_w_conect_ip")
pico_set_program_version(pico_w_conect_ip "0.1")

# Habilita saída via USB (UART desativado)
pico_enable_stdio_uart(pico_w_conect_ip 0)
pico_enable_stdio_usb(pico_w_conect_ip 1)

# Inclui diretórios padrão
target_include_directories(pico_w_conect_ip PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Liga bibliotecas padrão e Wi-Fi
target_link_libraries(pico_w_conect_ip
    pico_stdlib
    hardware_pwm
    pico_cyw43_arch_lwip_threadsafe_background
)

# Gera arquivos binários, UF2 etc.
pico_add_extra_outputs(pico_w_conect_ip)
