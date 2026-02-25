/**
 * @file  drivers/i2c/policy/inc/i2c_policy_polling.h
 * @brief PollingPolicy – blocking busy-wait transfer mode.
 *
 * In polling mode the CPU spins inside the transfer function, checking the
 * peripheral status registers on every iteration, until the transfer completes
 * or the timeout expires.
 *
 * Trade-offs
 * ----------
 * + Simplest implementation, easiest to debug.
 * + No RTOS dependency; works bare-metal from any context.
 * - CPU is blocked for the entire transfer duration.
 * - Not suitable for long transfers or real-time workloads.
 *
 * Extension point
 * ---------------
 * Replace PollingPolicy with IrqPolicy or DmaIrqPolicy
 * (see drivers/i2c/policy/inc/) to free the CPU during transfers.
 */
#pragma once

namespace i2c {
namespace policy {

/// Tag type that selects blocking busy-wait transfer mode.
struct PollingPolicy {
    /// Tag used by backends for compile-time policy dispatch.
    struct type_tag {};
};

}  // namespace policy
}  // namespace i2c
