#pragma once
#include "tweak_base.h"

// ─── Scheduler Category ───────────────────────────────────────────────────────

// Tweak 25: Set Win32PrioritySeparation for foreground boost
class Win32PrioritySeparationTweak : public TweakBase {
public:
    const char* Name()        const override { return "Win32 Priority Separation"; }
    const char* Description() const override { return "Sets Win32PrioritySeparation=26 to favour foreground application threads."; }
    const char* Detail()      const override {
        return "Win32PrioritySeparation controls how the NT scheduler distributes\n"
               "CPU quanta between foreground and background threads.\n"
               "Value 26 (0x1A) = short, variable quanta with maximum foreground\n"
               "priority boost — ideal for interactive and gaming workloads.";
    }
    const char* Category()    const override { return "Scheduler"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak 26: Set GPU/Game process CPU priority class (High)
class GameCPUPriorityTweak : public TweakBase {
public:
    const char* Name()        const override { return "Game CPU Priority (High)"; }
    const char* Description() const override { return "Configures the MMCSS Games profile to High scheduling priority."; }
    const char* Detail()      const override {
        return "The Multimedia Class Scheduler Service (MMCSS) elevates the\n"
               "priority of registered threads. Setting the 'Games' task\n"
               "scheduling category priority to High ensures game threads\n"
               "receive more CPU time during contention.";
    }
    const char* Category()    const override { return "Scheduler"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak 27: Set SFIO Priority (Multimedia MMCSS)
class SFIOPriorityTweak : public TweakBase {
public:
    const char* Name()        const override { return "SFIO Priority (MMCSS)"; }
    const char* Description() const override { return "Sets the MMCSS Pro Audio SFIO priority for lower audio/game latency."; }
    const char* Detail()      const override {
        return "Scheduled File I/O (SFIO) priority affects how Windows prioritises\n"
               "disk I/O requests from threads registered with MMCSS.\n"
               "Setting it to High reduces audio glitches and game hitches\n"
               "caused by background disk activity.";
    }
    const char* Category()    const override { return "Scheduler"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};
