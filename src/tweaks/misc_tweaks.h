#pragma once
#include "tweak_base.h"

// ─── Misc Category ────────────────────────────────────────────────────────────

// Tweak 28: Disable Delivery Optimization service
class DisableDeliveryOptimizationTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable Delivery Optimization Service"; }
    const char* Description() const override { return "Disables the DoSvc service that manages peer-to-peer update distribution."; }
    const char* Detail()      const override {
        return "Delivery Optimization (DoSvc) uses background bandwidth to\n"
               "distribute Windows/Store updates via P2P. Disabling the service\n"
               "prevents it from consuming CPU and network resources in the\n"
               "background during gaming or work sessions.";
    }
    const char* Category()    const override { return "Misc"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak 29: Disable Notifications
class DisableNotificationsTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable Notifications"; }
    const char* Description() const override { return "Suppresses Action Centre toast notifications globally."; }
    const char* Detail()      const override {
        return "Windows notifications (toast popups) can briefly preempt rendering\n"
               "threads and cause frame-time spikes. Disabling the Action Centre\n"
               "via registry prevents the notification framework from running\n"
               "its UI animations during gameplay.";
    }
    const char* Category()    const override { return "Misc"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak 30: Disable Background Apps
class DisableBackgroundAppsTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable Background Apps"; }
    const char* Description() const override { return "Prevents UWP apps from running in the background."; }
    const char* Detail()      const override {
        return "Windows allows UWP (Store) apps to receive updates and push\n"
               "notifications in the background. Disabling background app access\n"
               "reduces CPU/RAM consumption from apps like Mail, Calendar, and\n"
               "Edge running hidden tasks.";
    }
    const char* Category()    const override { return "Misc"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak 31: Disable Windows Telemetry
class DisableTelemetryTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable Windows Telemetry"; }
    const char* Description() const override { return "Sets telemetry level to 0 (Security) to minimize data collection overhead."; }
    const char* Detail()      const override {
        return "Windows telemetry periodically sends diagnostic data to Microsoft\n"
               "using background tasks and the DiagTrack service. Setting the\n"
               "AllowTelemetry value to 0 reduces it to the minimum (Security)\n"
               "level and disables most of the associated background tasks.";
    }
    const char* Category()    const override { return "Misc"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak: Disable Activity History
class DisableActivityHistoryTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable Activity History"; }
    const char* Description() const override { return "Prevents Windows from collecting and uploading activity history."; }
    const char* Detail()      const override {
        return "Windows tracks application usage, file opens, and browsing history\n"
               "in the Activity History timeline. This runs background tasks to\n"
               "collect and sync data. Disabling it reduces CPU overhead and\n"
               "network usage from background telemetry syncing.";
    }
    const char* Category()    const override { return "Misc"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak: Disable Storage Sense
class DisableStorageSenseTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable Storage Sense"; }
    const char* Description() const override { return "Prevents automatic background disk cleanup tasks."; }
    const char* Detail()      const override {
        return "Storage Sense periodically runs cleanup tasks to free disk space\n"
               "by deleting temporary files and emptying the recycle bin.\n"
               "These background I/O operations can cause latency spikes during\n"
               "gaming sessions. Disable if you prefer manual cleanup.";
    }
    const char* Category()    const override { return "Misc"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak: Disable Windows Error Reporting
class DisableErrorReportingTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable Windows Error Reporting"; }
    const char* Description() const override { return "Stops WER from collecting crash dumps and sending reports."; }
    const char* Detail()      const override {
        return "Windows Error Reporting (WER) collects crash dumps and sends\n"
               "diagnostic data to Microsoft. The WerSvc service and its data\n"
               "collection can consume CPU and disk I/O in the background.\n"
               "Disabling it prevents automatic error report generation.";
    }
    const char* Category()    const override { return "Misc"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};
