cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME HL-EACT_F4_SSCV5.12)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # EtherCAT SSC sources. The duplicate SSC/Src/9252_HW.c is intentionally
    # excluded; EtherCAT/9252_HW.c is the STM32 project copy used by this target.
    SSC/Src/SSC-Device.c
    SSC/Src/coeappl.c
    SSC/Src/ecatappl.c
    SSC/Src/ecatcoe.c
    SSC/Src/ecateoe.c
    SSC/Src/ecatfoe.c
    SSC/Src/ecatslv.c
    SSC/Src/emcy.c
    SSC/Src/eoeappl.c
    SSC/Src/foeappl.c
    SSC/Src/mailbox.c
    SSC/Src/objdef.c
    SSC/Src/sdoserv.c

    # LAN9252 hardware access and STM32F407 SPI platform layer.
    EtherCAT/9252_HW.c
    EtherCAT/SPIDriver/SPIDriver.c
    EtherCAT/SPIDriver/STM32F407/STM32SPIDriver.c
    EtherCAT/SPIDriver/STM32F407/STM32HW.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    SSC/Src
    EtherCAT
    EtherCAT/SPIDriver
    EtherCAT/SPIDriver/STM32F407
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
