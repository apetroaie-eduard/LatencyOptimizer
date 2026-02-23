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

// Tweak: Disable Print Spooler
class DisablePrintSpoolerTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable Print Spooler"; }
    const char* Description() const override { return "Stops the Print Spooler service to reduce background overhead."; }
    const char* Detail()      const override {
        return "The Print Spooler service manages print jobs and loads printer\n"
               "drivers. If you do not use a local printer, disabling it frees\n"
               "memory and eliminates a historically common attack surface.\n"
               "Re-enable if you need to print.";
    }
    const char* Category()    const override { return "Services"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak: Disable Diagnostic Services
class DisableDiagServicesTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable Diagnostic Services"; }
    const char* Description() const override { return "Disables DiagSvc and dmwappushservice to reduce background activity."; }
    const char* Detail()      const override {
        return "The Diagnostic Service (DiagSvc) and dmwappushservice handle\n"
               "Connected User Experiences telemetry routing. They consume\n"
               "CPU and network bandwidth in the background. Disabling them\n"
               "reduces background data collection activity.";
    }
    const char* Category()    const override { return "Services"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};
