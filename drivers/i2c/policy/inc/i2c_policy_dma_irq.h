/**
 * @file  drivers/i2c/policy/inc/i2c_policy_dma_irq.h
 * @brief DmaIrqPolicy – DMA + interrupt transfer mode.
 *
 * A DMA stream moves data while the CPU does other work.  The I2C event ISR
 * handles address-phase ACK/NACK; the DMA TC and TE ISRs signal completion
 * and errors.
 *
 * Trade-offs
 * ----------
 * + Lowest CPU overhead for large transfers.
 * + Best throughput on long bursts.
 * - Requires both I2C event/error ISRs and DMA Tx/Rx ISRs to be wired up.
 * - DMA channel/stream must be reserved in the board config.
 * - Setup complexity is the highest of the three policies.
 *
 * IRQ glue
 * --------
 * The companion file
 *   drivers/i2c/backend/stm32h7/<type>/src/i2c_irq_glue.cpp
 * must define all four ISR functions and populate all four handler slots
 * (IrqEventHandler, IrqErrorHandler, DmaTxHandler, DmaRxHandler) in the
 * PeriphDescriptor.
 */
#pragma once

namespace i2c {
namespace policy {

/// Tag type that selects DMA + interrupt transfer mode.
struct DmaIrqPolicy {
    struct type_tag {};
};

}  // namespace policy
}  // namespace i2c
