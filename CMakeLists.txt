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
# Wait-strategy headers (baremetal / threadx / timed_busy).
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
# STM32H7 LL backend – header-only portion (template declarations).
# ---------------------------------------------------------------------------
add_library(i2c_stm32h7_ll INTERFACE)
target_include_directories(i2c_stm32h7_ll INTERFACE ${REPO_ROOT})
target_link_libraries(i2c_stm32h7_ll INTERFACE
    i2c_stm32h7_common
    i2c_policy
    i2c_wait)

# ---------------------------------------------------------------------------
# STATIC library: i2c_stm32h7_ll_impl
# STM32H7 LL backend polling implementation.
#
# Requires vendor headers provided via external/:
#   external/cmsis/inc  – stm32h7xx.h and CMSIS core
#   external/ll/inc     – stm32h7xx_ll_i2c.h, stm32h7xx_ll_gpio.h, etc.
#
# Set STM32H750xx (or STM32H743xx) via the build system or IDE preprocessor
# definitions.  See docs/deps/stm32cube_external_layout.md for details.
#
# This target is excluded from the default host build when vendor headers are
# absent.  Enable it by setting I2C_LL_VENDOR_HEADERS_AVAILABLE=ON.
# ---------------------------------------------------------------------------
option(I2C_LL_VENDOR_HEADERS_AVAILABLE
    "Set to ON when external/ vendor headers are present to compile i2c_stm32h7_ll_impl"
    OFF)

if(I2C_LL_VENDOR_HEADERS_AVAILABLE)
    add_library(i2c_stm32h7_ll_impl STATIC
        drivers/i2c/backend/stm32h7/ll/src/i2c_ll_backend.cpp
        drivers/i2c/backend/stm32h7/ll/src/i2c_ll_irq_glue.cpp
        board/rev_a/src/board_system_timer_stm32h7_ll.cpp
        board/rev_a/src/board_clock_stm32h7_ll.cpp)
    target_include_directories(i2c_stm32h7_ll_impl PUBLIC
        ${REPO_ROOT}
        ${REPO_ROOT}/external/cmsis/inc
        ${REPO_ROOT}/external/ll/inc)
    target_link_libraries(i2c_stm32h7_ll_impl PUBLIC
        i2c_stm32h7_ll
        board_rev_a
        system_timer
        system_clocks)
endif()

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
# STATIC library: system_clocks
# System clock snapshot (system::clocks::get/set/reset_defaults).
# ---------------------------------------------------------------------------
add_library(system_clocks STATIC system/src/system_clocks.cpp)
target_include_directories(system_clocks PUBLIC ${REPO_ROOT})

# ---------------------------------------------------------------------------
# STATIC library: system_swo
# Minimal SWO/ITM printf-style logger for STM32H750 bring-up.
#
# When I2C_LL_VENDOR_HEADERS_AVAILABLE is ON (and CMSIS headers are present
# under external/cmsis/inc), the real ITM implementation is compiled.
# Otherwise a no-op stub is compiled so that host builds remain functional.
# ---------------------------------------------------------------------------
add_library(system_swo STATIC )
target_include_directories(system_swo PUBLIC ${REPO_ROOT})
if(I2C_LL_VENDOR_HEADERS_AVAILABLE)
    target_compile_definitions(system_swo PUBLIC I2C_LL_VENDOR_HEADERS_AVAILABLE)
    target_include_directories(system_swo PUBLIC
        ${REPO_ROOT}/external/cmsis/inc)
endif()

# ---------------------------------------------------------------------------
# INTERFACE library: system_timer
# Portable system timer abstraction (header-only template).
# ---------------------------------------------------------------------------
add_library(system_timer INTERFACE)
target_include_directories(system_timer INTERFACE ${REPO_ROOT})

# ---------------------------------------------------------------------------
# INTERFACE library: board_bsp
# Board-level BSP public headers (board.h, board_irq_priorities.h,
# board_revision_select.h).  Header-only.
# ---------------------------------------------------------------------------
add_library(board_bsp INTERFACE)
target_include_directories(board_bsp INTERFACE ${REPO_ROOT})
target_link_libraries(board_bsp INTERFACE board_rev_a system_timer)

# ---------------------------------------------------------------------------
# STATIC library: board_rev_a_src
# Revision-A BSP source files that do NOT require vendor headers:
#   board_init.cpp, board_timebase.cpp
# ---------------------------------------------------------------------------
add_library(board_rev_a_src STATIC
    board/rev_a/src/board_init.cpp
    board/rev_a/src/board_timebase.cpp)
target_include_directories(board_rev_a_src PUBLIC ${REPO_ROOT})
target_link_libraries(board_rev_a_src PUBLIC board_rev_a system_timer system_clocks)

# ---------------------------------------------------------------------------
# EXECUTABLE: app
# Application entry point.
#
# NOTE: This target compiles the scaffolding headers for a correctness check.
#       Full linking requires i2c_stm32h7_ll_impl (enable with
#       -DI2C_LL_VENDOR_HEADERS_AVAILABLE=ON and vendor headers in external/).
# ---------------------------------------------------------------------------
add_executable(app app/src/main.cpp)
target_include_directories(app PRIVATE ${REPO_ROOT})
target_link_libraries(app PRIVATE
    system_init
    system_clocks
    system_swo
    board_bsp
    board_rev_a_src
    ft6236)
