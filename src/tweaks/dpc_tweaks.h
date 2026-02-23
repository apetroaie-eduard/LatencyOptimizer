#pragma once
#include "tweak_base.h"

// ─── DPC Latency Category ────────────────────────────────────────────────────
// Advanced tweaks targeting Deferred Procedure Call (DPC) routine execution time,
// with an emphasis on NVIDIA GPU driver DPC overhead.

// Tweak: Disable NVIDIA HDCP (High-bandwidth Digital Content Protection)
// HDCP handshake logic runs inside the NVIDIA display driver's DPC routine.
// Disabling it removes that overhead entirely.
class DisableNvidiaHDCPTweak : public TweakBase {
public:
    const char* Name()        const override { return "NVIDIA: Disable HDCP"; }
    const char* Description() const override { return "Disables HDCP in the NVIDIA driver to eliminate HDCP handshake DPC overhead."; }
    const char* Detail()      const override {
        return "HDCP (High-bandwidth Digital Content Protection) validation runs\n"
               "inside nvlddmkm.sys DPC routines on every display mode change and\n"
               "periodically during playback. Disabling RmHdcpEnable removes this\n"
               "entire code path from the driver's DPC, measurably lowering worst-case\n"
               "DPC latency. Does not affect gaming; may prevent playback of\n"
               "HDCP-protected streaming content (Netflix in Edge, etc.).";
    }
    const char* Category()    const override { return "DPC Latency"; }
    TweakRisk   Risk()        const override { return TweakRisk::Medium; }
    TweakCompat Compat()      const override { return TweakCompat::NvidiaOnly; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak: NVIDIA Per-CPU Core DPC Distribution
// By default the NVIDIA driver funnels all DPC work to a single core.
// Enabling RmGpsPsEnablePerCpuCoreDpc distributes DPC processing across
// multiple cores, reducing per-core DPC execution time.
class NvidiaPerCpuDpcTweak : public TweakBase {
public:
    const char* Name()        const override { return "NVIDIA: Per-CPU DPC Distribution"; }
    const char* Description() const override { return "Distributes NVIDIA driver DPC work across multiple CPU cores."; }
    const char* Detail()      const override {
        return "The NVIDIA kernel driver (nvlddmkm.sys) schedules all DPC callbacks\n"
               "on a single processor by default. Setting RmGpsPsEnablePerCpuCoreDpc=1\n"
               "tells the driver to spread DPC work across available cores. This\n"
               "directly lowers the maximum DPC execution time observed on any single\n"
               "core, which is what tools like LatencyMon report. Requires reboot.";
    }
    const char* Category()    const override { return "DPC Latency"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::NvidiaOnly; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak: Disable NVIDIA GPU ASPM (Active State Power Management)
// PCIe ASPM transitions (L0s/L1) on the GPU link cause latency spikes in the
// driver's DPC routine while the link re-trains. Disabling ASPM keeps the PCIe
// link at full speed, eliminating these spikes.
class DisableNvidiaASPMTweak : public TweakBase {
public:
    const char* Name()        const override { return "NVIDIA: Disable GPU PCIe ASPM"; }
    const char* Description() const override { return "Disables PCIe Active State Power Management on the GPU link to prevent DPC spikes."; }
    const char* Detail()      const override {
        return "PCIe L0s/L1 power states save power by partly shutting down the\n"
               "PCIe link. When the GPU needs to communicate, the link must re-train,\n"
               "causing 50-200us stalls that appear as DPC latency spikes in\n"
               "nvlddmkm.sys. Setting RmDisableGpuASPMFlags=1 keeps the link at\n"
               "full L0 speed at all times. Increases idle GPU power by ~1-3W.\n"
               "Requires reboot.";
    }
    const char* Category()    const override { return "DPC Latency"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::NvidiaOnly; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak: Enable MSI Mode for GPU
// Unlike NICs where MSI can sometimes hurt, GPUs almost universally benefit
// from MSI mode. MSI avoids the shared IRQ line and its associated DPC
// serialization, giving the GPU its own dedicated interrupt vector.
class EnableGPUMSITweak : public TweakBase {
public:
    const char* Name()        const override { return "GPU: Enable MSI (Message-Signaled Interrupts)"; }
    const char* Description() const override { return "Enables MSI for the GPU, giving it a dedicated interrupt vector to reduce DPC queueing."; }
    const char* Detail()      const override {
        return "Legacy line-based interrupts are shared among devices, forcing DPC\n"
               "routines to serialize. MSI gives the GPU a dedicated interrupt vector\n"
               "that bypasses shared IRQ routing. This reduces interrupt-to-DPC latency\n"
               "and eliminates contention with other devices on the same IRQ line.\n"
               "Most modern GPUs support MSI; NVIDIA drivers default to it on newer\n"
               "systems, but older installs or OEM images may still use line-based.\n"
               "Requires reboot.";
    }
    const char* Category()    const override { return "DPC Latency"; }
    TweakRisk   Risk()        const override { return TweakRisk::Medium; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak: Disable NVIDIA Telemetry Container
// NvTelemetryContainer runs periodic tasks that execute inside kernel callbacks,
// contributing to DPC/ISR latency. It has no effect on GPU functionality.
class DisableNvTelemetryTweak : public TweakBase {
public:
    const char* Name()        const override { return "NVIDIA: Disable Telemetry Container"; }
    const char* Description() const override { return "Stops and disables NvTelemetryContainer, removing its periodic DPC overhead."; }
    const char* Detail()      const override {
        return "NvTelemetryContainer is an NVIDIA service that collects usage and\n"
               "crash telemetry. Its periodic ETW flush and WMI query operations\n"
               "trigger kernel callbacks that contribute to measured DPC latency.\n"
               "Disabling this service has zero impact on GPU performance or driver\n"
               "functionality. Only affects NVIDIA crash report uploads.";
    }
    const char* Category()    const override { return "DPC Latency"; }
    TweakRisk   Risk()        const override { return TweakRisk::Safe; }
    TweakCompat Compat()      const override { return TweakCompat::NvidiaOnly; }
    bool RequiresBackup()     const override { return false; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak: GPU Interrupt Affinity to Dedicated Core
// Pins GPU interrupts (and their resulting DPCs) to a specific CPU core,
// separate from NIC and other device interrupts. Prevents cross-device DPC
// contention on the same core.
class GPUInterruptAffinityTweak : public TweakBase {
public:
    const char* Name()        const override { return "GPU: Pin Interrupts to CPU 1"; }
    const char* Description() const override { return "Pins GPU interrupts to CPU core 1 to isolate GPU DPCs from NIC/other DPCs."; }
    const char* Detail()      const override {
        return "By default, GPU interrupts may land on the same core as network\n"
               "or USB interrupts, causing DPC routines from different drivers to\n"
               "queue behind each other. Pinning GPU interrupts to CPU 1 (while\n"
               "NIC is on CPU 0) ensures GPU DPC execution is never blocked by\n"
               "unrelated device DPCs. Pairs well with the NIC Interrupt Affinity\n"
               "tweak. Requires reboot.";
    }
    const char* Category()    const override { return "DPC Latency"; }
    TweakRisk   Risk()        const override { return TweakRisk::Medium; }
    TweakCompat Compat()      const override { return TweakCompat::All; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};

// Tweak: Disable NVIDIA Ultra Low Power State (ULPS equivalent for NVIDIA)
// Disables deep idle power states on the GPU. Waking from deep idle causes
// a burst of DPC activity and high latency while the GPU re-initializes.
class DisableNvidiaEnergyDriverTweak : public TweakBase {
public:
    const char* Name()        const override { return "NVIDIA: Disable PState Deep Idle (P8)"; }
    const char* Description() const override { return "Prevents the GPU from entering deep idle (P8), avoiding wake-up DPC storms."; }
    const char* Detail()      const override {
        return "When the GPU enters its deepest idle state (P8/D3), waking it back\n"
               "up triggers a burst of DPC activity in nvlddmkm.sys that can exceed\n"
               "500us. Setting EnableRID73519=0 and PerfLevelSrc=0x2222 forces the\n"
               "GPU to stay in a higher performance state. Increases idle power draw\n"
               "by ~5-15W but eliminates idle-to-active DPC storms. Useful for\n"
               "competitive gaming where consistent DPC latency matters more than\n"
               "idle power.";
    }
    const char* Category()    const override { return "DPC Latency"; }
    TweakRisk   Risk()        const override { return TweakRisk::Medium; }
    TweakCompat Compat()      const override { return TweakCompat::NvidiaOnly; }
    bool RequiresBackup()     const override { return true; }

    bool Apply()    override;
    bool Revert()   override;
    bool IsApplied() const override;
};
