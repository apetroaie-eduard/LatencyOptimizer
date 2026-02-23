#pragma once
#include "tweak_base.h"

// ─── GPU Category ─────────────────────────────────────────────────────────────

// Tweak 12: NVIDIA – Maximum Performance + Pre-rendered frames = 1
class NvidiaOptimizeTweak : public TweakBase {
public:
    const char* Name()        const override { return "NVIDIA: Max Performance + Prerendered Frames=1"; }
    const char* Description() const override { return "Sets NVIDIA driver to Max Performance mode and reduces pre-rendered frames."; }
    const char* Detail()      const override {
        return "Sets PowerMizerEnable=0 (always max clocks) and\n"
               "PrerenderedFrames=1 via registry for all NVIDIA adapters.\n"
               "Reduces render-queue latency at the cost of higher GPU power draw.\n"
               "Requires NVIDIA GPU.";
    }
    const char* Category()    const override { return "GPU"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::NvidiaOnly; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak 13: Disable Hardware-Accelerated GPU Scheduling (HAGS)
class DisableHAGSTweak : public TweakBase {
public:
    const char* Name()        const override { return "Disable Hardware-Accelerated GPU Scheduling"; }
    const char* Description() const override { return "Disables HAGS for reduced scheduling overhead on older GPUs."; }
    const char* Detail()      const override {
        return "HAGS moves GPU scheduling from the CPU to the GPU itself.\n"
               "On older or AMD GPUs this can increase latency. Disable if\n"
               "you experience stuttering or higher frame times with HAGS on.";
    }
    const char* Category()    const override { return "GPU"; }
    TweakRisk   Risk()        const override { return TweakRisk::Medium; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak 14: Enable Hardware-Accelerated GPU Scheduling (HAGS)
class EnableHAGSTweak : public TweakBase {
public:
    const char* Name()        const override { return "Enable Hardware-Accelerated GPU Scheduling"; }
    const char* Description() const override { return "Enables HAGS for reduced CPU-side scheduling overhead on modern GPUs."; }
    const char* Detail()      const override {
        return "On NVIDIA RTX 20-series and newer / AMD RDNA2+ GPUs,\n"
               "HAGS can reduce latency by offloading scheduling work to the GPU.\n"
               "Enable only if your GPU supports it (driver version 451.48+).";
    }
    const char* Category()    const override { return "GPU"; }
    TweakRisk   Risk()        const override { return TweakRisk::Medium; }
    TweakCompat Compat()      const override { return TweakCompat::AdvancedHW; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};
