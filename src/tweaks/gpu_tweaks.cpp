#include "gpu_tweaks.h"
#include "../utils/registry_utils.h"

// ─── NvidiaOptimizeTweak ─────────────────────────────────────────────────────
static const wchar_t* kNvGlobalKey =
    L"SYSTEM\\CurrentControlSet\\Control\\Class\\"
    L"{4d36e968-e325-11ce-bfc1-08002be10318}\\0000";

bool NvidiaOptimizeTweak::Apply()
{
    // PowerMizerEnable 0 = always max perf, PrerenderedFrames = 1
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kNvGlobalKey,
                                          L"PowerMizerEnable", 0);
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kNvGlobalKey,
                                      L"PowerMizerLevel", 1);
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kNvGlobalKey,
                                      L"PowerMizerLevelAC", 1);

    // D3D prerendered frames limit (applies to all DXGI apps)
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE,
        L"SYSTEM\\CurrentControlSet\\Control\\GraphicsDrivers",
        L"PendingDriverOperations", 1);

    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE,
        L"SOFTWARE\\NVIDIA Corporation\\Global\\NVTweak",
        L"Prerender", 1);

    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool NvidiaOptimizeTweak::Revert()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kNvGlobalKey,
                                          L"PowerMizerEnable", 1);
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kNvGlobalKey,
                                      L"PowerMizerLevel", 3);
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kNvGlobalKey,
                                      L"PowerMizerLevelAC", 3);
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE,
        L"SOFTWARE\\NVIDIA Corporation\\Global\\NVTweak",
        L"Prerender", 3);
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool NvidiaOptimizeTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kNvGlobalKey,
                                          L"PowerMizerEnable");
    return val.has_value() && *val == 0;
}

// ─── DisableHAGSTweak ─────────────────────────────────────────────────────────
static const wchar_t* kGraphicsDriversKey =
    L"SYSTEM\\CurrentControlSet\\Control\\GraphicsDrivers";

bool DisableHAGSTweak::Apply()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kGraphicsDriversKey,
                                          L"HwSchMode", 1); // 1 = disabled
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableHAGSTweak::Revert()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kGraphicsDriversKey,
                                          L"HwSchMode", 2); // 2 = enabled
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableHAGSTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kGraphicsDriversKey, L"HwSchMode");
    return val.has_value() && *val == 1;
}

// ─── EnableHAGSTweak ──────────────────────────────────────────────────────────

bool EnableHAGSTweak::Apply()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kGraphicsDriversKey,
                                          L"HwSchMode", 2);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool EnableHAGSTweak::Revert()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kGraphicsDriversKey,
                                          L"HwSchMode", 1);
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool EnableHAGSTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kGraphicsDriversKey, L"HwSchMode");
    return val.has_value() && *val == 2;
}
