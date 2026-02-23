#pragma once
#include "tweak_base.h"

// ─── Input Category ───────────────────────────────────────────────────────────

// Tweak 22: Disable Mouse Acceleration
class DisableMouseAccelerationTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable Mouse Acceleration"; }
    const char* Description() const override { return "Sets MouseSpeed=0 and disables enhance pointer precision."; }
    const char* Detail()      const override {
        return "Mouse acceleration (Enhance Pointer Precision) scales pointer\n"
               "movement based on speed, making aim inconsistent in games.\n"
               "Disabling it gives a 1:1 relationship between physical and\n"
               "on-screen movement regardless of speed.";
    }
    const char* Category()    const override { return "Input"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak 23: Set Mouse Polling Rate Registry hint (8000 Hz capable)
class MousePollingRateTweak : public TweakBase {
public:
    const char* Name()        const override { return "Set Mouse Polling Rate (Registry)"; }
    const char* Description() const override { return "Sets SampleRate hint to 1000 for USB HID mice."; }
    const char* Detail()      const override {
        return "The Windows HID driver can report the polling rate of the mouse\n"
               "via the SampleRate registry value. Setting it to 1000 ensures\n"
               "the driver requests the highest standard rate from the device.\n"
               "Actual rate depends on the hardware.";
    }
    const char* Category()    const override { return "Input"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak 24: Disable Pointer Precision (fine-grained)
class DisablePointerPrecisionTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable Pointer Precision"; }
    const char* Description() const override { return "Sets MouseThreshold values to 0 for raw linear movement."; }
    const char* Detail()      const override {
        return "MouseThreshold1/2 and MouseSpeed control the legacy acceleration\n"
               "curve in Windows. Setting all three to 0 ensures no acceleration\n"
               "is applied even if the Enhance Pointer Precision checkbox is on.";
    }
    const char* Category()    const override { return "Input"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};
