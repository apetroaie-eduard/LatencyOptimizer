#pragma once
#include "tweak_base.h"

// ─── Timers Category ──────────────────────────────────────────────────────────

// Tweak 18: Request high-resolution system timer (0.5 ms)
class HighResTimerTweak : public TweakBase {
public:
    const char* Name()        const override { return "High-Resolution System Timer"; }
    const char* Description() const override { return "Sets the global timer resolution to ~0.5 ms via bcdedit."; }
    const char* Detail()      const override {
        return "Windows defaults to a 15.6 ms timer resolution. Setting it to\n"
               "0.5 ms via bcdedit /set useplatformclock true and reducing the\n"
               "minimum resolution improves scheduling granularity, which lowers\n"
               "sleep-jitter and frame-time variance in games.";
    }
    const char* Category()    const override { return "Timers"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak 19: Disable Dynamic Tick
class DisableDynamicTickTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable Dynamic Tick"; }
    const char* Description() const override { return "Prevents Windows from reducing the timer tick rate during idle."; }
    const char* Detail()      const override {
        return "Dynamic tick allows the OS to coalesce timer interrupts to save\n"
               "power. This can introduce jitter at the cost of a wakeup latency.\n"
               "Disabling it forces a constant tick rate for consistent scheduling.";
    }
    const char* Category()    const override { return "Timers"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};
