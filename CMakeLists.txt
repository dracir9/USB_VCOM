cmake_minimum_required(VERSION 3.22)

# Add Library sources and include directories
add_library(USB_VCOM STATIC
    vcom.c
)

target_include_directories(USB_VCOM PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link STM32CubeMX generated library to USB_VCOM
target_link_libraries(USB_VCOM PRIVATE
    stm32cubemx
)
