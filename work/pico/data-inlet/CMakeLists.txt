cmake_minimum_required(VERSION 3.13)
set(PICO_SDK_PATH ../../pico-sdk/)

include(pico_sdk_import.cmake)
project(test_project C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()
add_executable(data-inlet
	src/blink.c++
)

pico_enable_stdio_usb(data-inlet 1)
pico_enable_stdio_uart(data-inlet 1)
pico_add_extra_outputs(data-inlet)
target_link_libraries(data-inlet pico_stdlib)
