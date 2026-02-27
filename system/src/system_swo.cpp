/**
 * @file  system/src/system_swo.cpp
 * @brief SWO/ITM logger implementation for STM32H750 (J-Link / ST-Link SWO).
 *
 * When vendor/CMSIS headers are available (I2C_LL_VENDOR_HEADERS_AVAILABLE is
 * ON and the headers exist under external/), the real ITM implementation is
 * compiled.  Otherwise a no-op stub is compiled so that host (non-embedded)
 * builds continue to succeed without modification.
 *
 * Hardware details (STM32H750, Cortex-M7):
 *   - CoreDebug->DEMCR : enable trace
 *   - TPI->ACPR        : SWO prescaler = (cpu_hz / swo_baud) - 1
 *   - ITM->TCR         : enable ITM
 *   - ITM->TER         : enable stimulus port 0
 *   - ITM->PORT[0].u8  : single-byte write when port is not busy
 *
 * Conservative SWO baud: 2 000 000 bps (works reliably with J-Link and most
 * ST-Link v2.1 firmware versions).
 */

#include "system/inc/system_swo.h"

#ifdef I2C_LL_VENDOR_HEADERS_AVAILABLE
// ---------------------------------------------------------------------------
// Real implementation – compiled only when vendor/CMSIS headers are present.
// ---------------------------------------------------------------------------
#include <cstdint>

// Vendor / CMSIS headers provide CoreDebug, TPI, ITM register definitions.
#include "external/cmsis/inc/stm32h7xx.h"

namespace system {
namespace swo {

static constexpr uint32_t k_swo_baud = 2'000'000U;

void init(uint32_t cpu_hz)
{
    // Enable trace subsystem in the debug control register.
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    // Configure SWO baud rate prescaler.
    // ACPR = (cpu_hz / swo_baud) - 1
    TPI->ACPR = (cpu_hz / k_swo_baud) - 1U;

    // Select NRZ (UART) encoding for SWO (SPPR = 2).
    TPI->SPPR = 2U;

    // Disable formatter (not needed for single-wire SWO).
    TPI->FFCR &= ~TPI_FFCR_EnFCont_Msk;

    // Enable ITM and configure privilege / synchronisation.
    ITM->LAR  = 0xC5ACCE55U;          // unlock ITM registers
    ITM->TCR  = ITM_TCR_ITMENA_Msk    // enable ITM
               | ITM_TCR_SYNCENA_Msk  // enable sync pulses
               | ITM_TCR_TXENA_Msk    // enable DWT stimulus forwarding
               | (1U << ITM_TCR_TraceBusID_Pos);  // bus ID = 1

    // Enable stimulus port 0.
    ITM->TER = 1U;
}

void putc(char c)
{
    // Guard: ITM must be enabled and port 0 must be enabled.
    if ((ITM->TCR & ITM_TCR_ITMENA_Msk) == 0U) { return; }
    if ((ITM->TER & 1U) == 0U)               { return; }

    // Wait until the port is not busy, then write.
    // A timeout prevents an infinite hang if the debugger is not connected.
    static constexpr uint32_t k_timeout = 100'000U;
    uint32_t retries = k_timeout;
    while (ITM->PORT[0].u32 == 0U) {
        if (--retries == 0U) { return; }
    }
    ITM->PORT[0].u8 = static_cast<uint8_t>(c);
}

void write(const char* s)
{
    while (s && *s) {
        putc(*s++);
    }
}

}  // namespace swo
}  // namespace system

#else  // !I2C_LL_VENDOR_HEADERS_AVAILABLE
// ---------------------------------------------------------------------------
// No-op stub – compiled when vendor/CMSIS headers are absent (host builds).
// ---------------------------------------------------------------------------

namespace system {
namespace swo {

void init(uint32_t /*cpu_hz*/) {}
void putc(char /*c*/)          {}
void write(const char* /*s*/)  {}

}  // namespace swo
}  // namespace system

#endif  // I2C_LL_VENDOR_HEADERS_AVAILABLE
