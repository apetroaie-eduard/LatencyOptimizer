#include "dpc_tweaks.h"
#include "../utils/registry_utils.h"
#include "../utils/service_utils.h"

// NVIDIA display adapter class key (device 0000)
static const wchar_t* kNvDisplayKey =
    L"SYSTEM\\CurrentControlSet\\Control\\Class\\"
    L"{4d36e968-e325-11ce-bfc1-08002be10318}\\0000";

// ─── DisableNvidiaHDCPTweak ─────────────────────────────────────────────────

bool DisableNvidiaHDCPTweak::Apply()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kNvDisplayKey,
                                          L"RmHdcpEnable", 0);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableNvidiaHDCPTweak::Revert()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kNvDisplayKey,
                                          L"RmHdcpEnable", 1);
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableNvidiaHDCPTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kNvDisplayKey,
                                          L"RmHdcpEnable");
    return val.has_value() && *val == 0;
}

// ─── NvidiaPerCpuDpcTweak ───────────────────────────────────────────────────

bool NvidiaPerCpuDpcTweak::Apply()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kNvDisplayKey,
                                          L"RmGpsPsEnablePerCpuCoreDpc", 1);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool NvidiaPerCpuDpcTweak::Revert()
{
    bool ok = registry_utils::DeleteValue(HKEY_LOCAL_MACHINE, kNvDisplayKey,
                                           L"RmGpsPsEnablePerCpuCoreDpc");
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool NvidiaPerCpuDpcTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kNvDisplayKey,
                                          L"RmGpsPsEnablePerCpuCoreDpc");
    return val.has_value() && *val == 1;
}

// ─── DisableNvidiaASPMTweak ─────────────────────────────────────────────────

bool DisableNvidiaASPMTweak::Apply()
{
    // Bit 0 = disable L0s, Bit 1 = disable L1 → 0x3 = disable both
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kNvDisplayKey,
                                          L"RmDisableGpuASPMFlags", 0x3);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableNvidiaASPMTweak::Revert()
{
    bool ok = registry_utils::DeleteValue(HKEY_LOCAL_MACHINE, kNvDisplayKey,
                                           L"RmDisableGpuASPMFlags");
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableNvidiaASPMTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kNvDisplayKey,
                                          L"RmDisableGpuASPMFlags");
    return val.has_value() && *val == 0x3;
}

// ─── EnableGPUMSITweak ─────────────────────────────────────────────────────

bool EnableGPUMSITweak::Apply()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kNvDisplayKey,
                                          L"MSISupported", 1);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool EnableGPUMSITweak::Revert()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kNvDisplayKey,
                                          L"MSISupported", 0);
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool EnableGPUMSITweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kNvDisplayKey,
                                          L"MSISupported");
    return val.has_value() && *val == 1;
}

// ─── DisableNvTelemetryTweak ────────────────────────────────────────────────

bool DisableNvTelemetryTweak::Apply()
{
    bool ok = service_utils::StopService(L"NvTelemetryContainer") || true;
    ok &= service_utils::SetStartType(L"NvTelemetryContainer", SERVICE_DISABLED);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableNvTelemetryTweak::Revert()
{
    bool ok = service_utils::SetStartType(L"NvTelemetryContainer", SERVICE_AUTO_START);
    if (ok) service_utils::StartService(L"NvTelemetryContainer");
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableNvTelemetryTweak::IsApplied() const
{
    return service_utils::GetStartType(L"NvTelemetryContainer") == SERVICE_DISABLED;
}

// ─── GPUInterruptAffinityTweak ──────────────────────────────────────────────
// Uses GPU class key {4d36e968-...}

bool GPUInterruptAffinityTweak::Apply()
{
    // IrqPolicySpecifiedProcessors = 5, AffinityMask = 0x02 = CPU 1
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kNvDisplayKey,
                                          L"*InterruptAffinityPolicy", 5);
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kNvDisplayKey,
                                      L"*InterruptAffinity", 0x02);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool GPUInterruptAffinityTweak::Revert()
{
    bool ok = registry_utils::DeleteValue(HKEY_LOCAL_MACHINE, kNvDisplayKey,
                                           L"*InterruptAffinityPolicy");
    ok &= registry_utils::DeleteValue(HKEY_LOCAL_MACHINE, kNvDisplayKey,
                                       L"*InterruptAffinity");
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool GPUInterruptAffinityTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kNvDisplayKey,
                                          L"*InterruptAffinityPolicy");
    return val.has_value() && *val == 5;
}

// ─── DisableNvidiaEnergyDriverTweak ─────────────────────────────────────────

bool DisableNvidiaEnergyDriverTweak::Apply()
{
    // Disable deep-idle P-state transitions
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kNvDisplayKey,
                                          L"EnableRID73519", 0);
    // PerfLevelSrc 0x2222 = prefer performance on all sub-systems
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kNvDisplayKey,
                                      L"PerfLevelSrc", 0x2222);
    // Disable dynamic PState switching (keeps GPU clocks stable)
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kNvDisplayKey,
                                      L"DisableDynamicPstate", 1);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableNvidiaEnergyDriverTweak::Revert()
{
    bool ok = registry_utils::DeleteValue(HKEY_LOCAL_MACHINE, kNvDisplayKey,
                                           L"EnableRID73519");
    ok &= registry_utils::DeleteValue(HKEY_LOCAL_MACHINE, kNvDisplayKey,
                                       L"PerfLevelSrc");
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kNvDisplayKey,
                                      L"DisableDynamicPstate", 0);
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableNvidiaEnergyDriverTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kNvDisplayKey,
                                          L"DisableDynamicPstate");
    return val.has_value() && *val == 1;
}
