cmake_minimum_required(VERSION 3.22)
project(arch_project LANGUAGES CXX)

# ---------------------------------------------------------------------------
# Global C++ settings
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use the repository root as the common include root so that all headers are
# referenced with their full path (e.g. #include "drivers/i2c/core/inc/…").
set(REPO_ROOT ${CMAKE_SOURCE_DIR})

# ---------------------------------------------------------------------------
# INTERFACE library: i2c_core
# Portable I2C types, error codes, traits, context, and bus façade.
# No sources – header-only.
# ---------------------------------------------------------------------------
add_library(i2c_core INTERFACE)
target_include_directories(i2c_core INTERFACE ${REPO_ROOT})

# ---------------------------------------------------------------------------
# INTERFACE library: i2c_policy
# Transfer-mode policy tags (polling / irq / dma_irq).
# ---------------------------------------------------------------------------
add_library(i2c_policy INTERFACE)
target_include_directories(i2c_policy INTERFACE ${REPO_ROOT})
target_link_libraries(i2c_policy INTERFACE i2c_core)

# ---------------------------------------------------------------------------
# INTERFACE library: i2c_wait
# Wait-strategy headers (baremetal / threadx).
# ---------------------------------------------------------------------------
add_library(i2c_wait INTERFACE)
target_include_directories(i2c_wait INTERFACE ${REPO_ROOT})

# ---------------------------------------------------------------------------
# INTERFACE library: i2c_stm32h7_common
# STM32H7 peripheral descriptor (PeriphDescriptor).
# ---------------------------------------------------------------------------
add_library(i2c_stm32h7_common INTERFACE)
target_include_directories(i2c_stm32h7_common INTERFACE ${REPO_ROOT})
target_link_libraries(i2c_stm32h7_common INTERFACE i2c_core)

# ---------------------------------------------------------------------------
# INTERFACE library: i2c_stm32h7_ll
# STM32H7 LL backend skeleton (header-only; bodies to be added later).
# ---------------------------------------------------------------------------
add_library(i2c_stm32h7_ll INTERFACE)
target_include_directories(i2c_stm32h7_ll INTERFACE ${REPO_ROOT})
target_link_libraries(i2c_stm32h7_ll INTERFACE
    i2c_stm32h7_common
    i2c_policy
    i2c_wait)

# ---------------------------------------------------------------------------
# INTERFACE library: i2c_stm32h7_hal
# STM32H7 HAL backend skeleton.
# ---------------------------------------------------------------------------
add_library(i2c_stm32h7_hal INTERFACE)
target_include_directories(i2c_stm32h7_hal INTERFACE ${REPO_ROOT})
target_link_libraries(i2c_stm32h7_hal INTERFACE
    i2c_stm32h7_common
    i2c_policy
    i2c_wait)

# ---------------------------------------------------------------------------
# INTERFACE library: i2c_stm32h7_cmsis
# STM32H7 CMSIS backend skeleton.
# ---------------------------------------------------------------------------
add_library(i2c_stm32h7_cmsis INTERFACE)
target_include_directories(i2c_stm32h7_cmsis INTERFACE ${REPO_ROOT})
target_link_libraries(i2c_stm32h7_cmsis INTERFACE
    i2c_stm32h7_common
    i2c_policy
    i2c_wait)

# ---------------------------------------------------------------------------
# INTERFACE library: board_rev_a
# Board revision-A configuration and pin map.
# ---------------------------------------------------------------------------
add_library(board_rev_a INTERFACE)
target_include_directories(board_rev_a INTERFACE ${REPO_ROOT})
target_link_libraries(board_rev_a INTERFACE i2c_stm32h7_ll)

# ---------------------------------------------------------------------------
# INTERFACE library: ft6236
# FT6236 capacitive touch driver (header-only).
# ---------------------------------------------------------------------------
add_library(ft6236 INTERFACE)
target_include_directories(ft6236 INTERFACE ${REPO_ROOT})
target_link_libraries(ft6236 INTERFACE i2c_core)

# ---------------------------------------------------------------------------
# STATIC library: system_init
# Early MCU system initialisation (placeholder body in system/src/).
# ---------------------------------------------------------------------------
add_library(system_init STATIC system/src/system_init.cpp)
target_include_directories(system_init PUBLIC ${REPO_ROOT})

# ---------------------------------------------------------------------------
# EXECUTABLE: app
# Application entry point.
#
# NOTE: This target compiles the scaffolding headers but will NOT link until
#       the backend method bodies (LlBackend::init/write/read/…) are provided.
#       It is included here as a compile-time correctness check.
# ---------------------------------------------------------------------------
add_executable(app app/src/main.cpp)
target_include_directories(app PRIVATE ${REPO_ROOT})
target_link_libraries(app PRIVATE
    system_init
    board_rev_a
    ft6236)
