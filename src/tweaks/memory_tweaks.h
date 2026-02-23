#pragma once
#include "tweak_base.h"

// ─── Memory Category ──────────────────────────────────────────────────────────

// Tweak 15: Disable Paging Executive
class DisablePagingExecutiveTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable Paging Executive"; }
    const char* Description() const override { return "Forces kernel code/drivers to stay in RAM (no swapping to pagefile)."; }
    const char* Detail()      const override {
        return "Setting DisablePagingExecutive=1 prevents the Windows kernel and\n"
               "drivers from being paged out to disk. Reduces latency spikes caused\n"
               "by page faults in kernel mode. Requires sufficient physical RAM.";
    }
    const char* Category()    const override { return "Memory"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak 16: Set Large System Cache
class LargeSystemCacheTweak : public TweakBase {
public:
    const char* Name()        const override { return "Enable Large System Cache"; }
    const char* Description() const override { return "Allocates more RAM to the file system cache for faster I/O."; }
    const char* Detail()      const override {
        return "LargeSystemCache=1 allows Windows to use most physical memory\n"
               "as a disk cache. Useful on systems with 16 GB+ RAM and heavy\n"
               "asset streaming workloads (e.g. open-world games).";
    }
    const char* Category()    const override { return "Memory"; }
    TweakRisk   Risk()        const override { return TweakRisk::Medium; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak 17: Disable Memory Compression
class DisableMemoryCompressionTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable Memory Compression"; }
    const char* Description() const override { return "Turns off Windows memory compression to reduce CPU overhead."; }
    const char* Detail()      const override {
        return "Windows 10/11 compresses infrequently-used memory pages in RAM\n"
               "to delay pagefile usage. On fast CPUs this is nearly free, but on\n"
               "cores already at capacity it adds latency. Disabling it frees\n"
               "those CPU cycles at the cost of heavier pagefile usage.";
    }
    const char* Category()    const override { return "Memory"; }
    TweakRisk   Risk()        const override { return TweakRisk::Medium; }
    TweakCompat Compat()      const override { return TweakCompat::All; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};
