#pragma once
#include "tweak_base.h"

// ─── Power Category ───────────────────────────────────────────────────────────

// Tweak 7: Activate Ultimate Performance power plan
class UltimatePerformancePlanTweak : public TweakBase {
public:
    const char* Name()        const override { return "Ultimate Performance Power Plan"; }
    const char* Description() const override { return "Activates the hidden Ultimate Performance power scheme."; }
    const char* Detail()      const override {
        return "The Ultimate Performance plan removes micro-latency from power\n"
               "management by keeping CPUs at maximum P-state and disabling\n"
               "power saving idle states. Increases power usage noticeably.\n"
               "Available on Windows 10 1803+ / Windows 11.";
    }
    const char* Category()    const override { return "Power"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;

private:
    // GUID of the Ultimate Performance plan
    static constexpr const char* kGUID = "e9a42b02-d5df-448d-aa00-03f14749eb61";
};

// Tweak 8: Disable USB Selective Suspend
class DisableUSBSuspendTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable USB Selective Suspend"; }
    const char* Description() const override { return "Prevents USB devices from being power-suspended, reducing input latency spikes."; }
    const char* Detail()      const override {
        return "USB Selective Suspend allows the hub driver to suspend individual\n"
               "USB ports to save power. This can cause a small but measurable\n"
               "latency spike when the device wakes up. Disabling it keeps all\n"
               "USB ports active at all times.";
    }
    const char* Category()    const override { return "Power"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};
