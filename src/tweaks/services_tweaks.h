#pragma once
#include "tweak_base.h"

// ─── Services Category ───────────────────────────────────────────────────────
// Tweak 1: Disable SysMain (Superfetch)
class DisableSysMainTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable SysMain (Superfetch)"; }
    const char* Description() const override { return "Stops Superfetch to reduce background disk I/O."; }
    const char* Detail()      const override {
        return "SysMain pre-loads frequently used applications into RAM.\n"
               "Disabling it reduces background disk and CPU usage.\n"
               "Safe to disable on SSDs; may slow cold-start on HDDs.";
    }
    const char* Category()    const override { return "Services"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak 2: Disable Windows Search Indexing
class DisableWSearchTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable Windows Search Indexer"; }
    const char* Description() const override { return "Stops WSearch service to reduce background CPU/disk usage."; }
    const char* Detail()      const override {
        return "Windows Search continuously indexes files for fast search results.\n"
               "Disabling it eliminates the constant background I/O at the cost\n"
               "of slower Start Menu searches.";
    }
    const char* Category()    const override { return "Services"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak 3: Disable Windows Update Delivery Optimization (P2P)
class DisableWUDOTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable Update Delivery Optimization P2P"; }
    const char* Description() const override { return "Prevents Windows from uploading updates to other PCs."; }
    const char* Detail()      const override {
        return "Delivery Optimization uses your bandwidth to distribute Windows\n"
               "updates to other machines on the internet.\n"
               "Disabling P2P mode (set to LAN-only) reduces upload overhead.";
    }
    const char* Category()    const override { return "Services"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};
