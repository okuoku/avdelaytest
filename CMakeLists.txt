cmake_minimum_required(VERSION 3.0.0)
project(avdelaytest)

set(SDLROOT c:/libs/SDL2) # Arrange this
if(NOT EXISTS ${SDLROOT}/include/SDL.h)
    message(FATAL_ERROR "SDL2 was not found on ${SDLROOT}. Adjust SDLROOT in ${CMAKE_CURRENT_LIST_FILE}")
endif()

add_executable(avdelaytest WIN32
    delaytest.c)

set(libroot ${SDLROOT}/lib/x64)
include_directories(${SDLROOT}/include)

target_link_libraries(avdelaytest
    ${libroot}/SDL2.lib
    ${libroot}/SDL2main.lib)

add_custom_command(TARGET avdelaytest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${SDLROOT}/lib/x64/SDL2.dll
    ${CMAKE_CURRENT_BINARY_DIR})

