#pragma once
#include "tweak_base.h"

// ─── Interrupts Category ──────────────────────────────────────────────────────

// Tweak 20: Set Network Adapter Interrupt Affinity to CPU 0
class NetworkInterruptAffinityTweak : public TweakBase {
public:
    const char* Name()        const override { return "Network Adapter Interrupt Affinity"; }
    const char* Description() const override { return "Pins NIC interrupts to CPU 0 to reduce inter-core latency."; }
    const char* Detail()      const override {
        return "By default Windows can migrate NIC interrupts across cores.\n"
               "Pinning them to a single core (CPU 0) reduces cache-line\n"
               "ping-pong and gives a more predictable interrupt service time.\n"
               "May reduce multi-core throughput slightly.";
    }
    const char* Category()    const override { return "Interrupts"; }
    TweakRisk   Risk()        const override { return TweakRisk::Medium; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak 21: Disable Message-Signaled Interrupts (MSI) for network adapter
class DisableNetworkMSITweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable MSI for Network Adapter"; }
    const char* Description() const override { return "Switches NIC from MSI to legacy INTx interrupts if MSI causes issues."; }
    const char* Detail()      const override {
        return "Message Signaled Interrupts (MSI) are generally faster, but some\n"
               "NIC/motherboard combinations exhibit higher latency or missed\n"
               "interrupts with MSI enabled. Disabling forces the legacy INTx\n"
               "interrupt path which can be more reliable on affected hardware.";
    }
    const char* Category()    const override { return "Interrupts"; }
    TweakRisk   Risk()        const override { return TweakRisk::High; }
    TweakCompat Compat()      const override { return TweakCompat::AdvancedHW; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};
