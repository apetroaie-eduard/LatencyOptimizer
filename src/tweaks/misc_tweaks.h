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
