#pragma once
#include "tweak_base.h"

// ─── Registry Category ────────────────────────────────────────────────────────

// Tweak 4: Disable Visual Effects (animations/transparency)
class DisableVisualEffectsTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable Visual Effects"; }
    const char* Description() const override { return "Turns off animations and transparency for lower GPU/CPU overhead."; }
    const char* Detail()      const override {
        return "Disables smooth window animations, fade effects and transparency.\n"
               "Reduces compositing overhead — especially helpful on systems\n"
               "where the iGPU or dGPU is under load.";
    }
    const char* Category()    const override { return "Registry"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak 5: Disable Windows Tips and Suggestions
class DisableWindowsTipsTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable Windows Tips & Suggestions"; }
    const char* Description() const override { return "Prevents Windows from showing tips and ads in the UI."; }
    const char* Detail()      const override {
        return "Windows periodically shows tips, app suggestions, and promotional\n"
               "content. Disabling these registry keys stops the background tasks\n"
               "that generate and display them.";
    }
    const char* Category()    const override { return "Registry"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak 6: Disable Xbox Game Bar
class DisableGameBarTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable Xbox Game Bar"; }
    const char* Description() const override { return "Disables the Xbox Game Bar overlay to reduce CPU overhead in games."; }
    const char* Detail()      const override {
        return "Xbox Game Bar hooks into every DirectX/OpenGL application.\n"
               "Disabling it removes the Win+G shortcut overhead and its\n"
               "background monitoring processes.";
    }
    const char* Category()    const override { return "Registry"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak: Disable Cortana
class DisableCortanaTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable Cortana"; }
    const char* Description() const override { return "Disables Cortana to reduce background CPU and network usage."; }
    const char* Detail()      const override {
        return "Cortana runs background processes for voice recognition and\n"
               "Bing integration. Disabling it via policy frees CPU cycles and\n"
               "stops background network requests. Search still works via the\n"
               "standard Windows Search interface.";
    }
    const char* Category()    const override { return "Registry"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak: Disable Fast Startup (Hibernation)
class DisableFastStartupTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable Fast Startup"; }
    const char* Description() const override { return "Disables hybrid shutdown for clean boot and reduced driver issues."; }
    const char* Detail()      const override {
        return "Fast Startup saves kernel state to the hibernation file on shutdown\n"
               "for faster boot. This can cause driver issues, stale state, and\n"
               "prevents clean re-initialization of hardware. Disabling it ensures\n"
               "a full cold boot with fresh driver initialization every time.";
    }
    const char* Category()    const override { return "Registry"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};
