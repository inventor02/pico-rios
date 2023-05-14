cmake_minimum_required(VERSION 3.12)
set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(PRJ_NAME "rios")

include(lib/pico-sdk/pico_sdk_init.cmake)

project(${PRJ_NAME})
pico_sdk_init()

add_executable(${PRJ_NAME})
pico_add_extra_outputs(${PRJ_NAME})

target_link_libraries(${PRJ_NAME}
  pico_stdlib
  hardware_timer
)

add_subdirectory(src)