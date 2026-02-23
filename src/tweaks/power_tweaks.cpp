#include "power_tweaks.h"
#include "../utils/cmd_utils.h"
#include "../utils/registry_utils.h"

// ─── UltimatePerformancePlanTweak ─────────────────────────────────────────────

bool UltimatePerformancePlanTweak::Apply()
{
    // Duplicate the scheme (idempotent if already exists)
    cmd_utils::RunCommand(L"powercfg",
        L"/duplicatescheme e9a42b02-d5df-448d-aa00-03f14749eb61", false, true);

    // Set it as active
    std::string guid = kGUID;
    std::wstring wguid(guid.begin(), guid.end());
    int rc = cmd_utils::RunCommand(L"powercfg",
        L"/setactive " + wguid, false, true);

    m_lastStatus = (rc == 0) ? TweakStatus::Applied : TweakStatus::Failed;
    return rc == 0;
}

bool UltimatePerformancePlanTweak::Revert()
{
    // Restore Balanced plan (381b4222-f694-41f0-9685-ff5bb260df2e)
    int rc = cmd_utils::RunCommand(L"powercfg",
        L"/setactive 381b4222-f694-41f0-9685-ff5bb260df2e", false, true);
    m_lastStatus = (rc == 0) ? TweakStatus::Reverted : TweakStatus::Failed;
    return rc == 0;
}

bool UltimatePerformancePlanTweak::IsApplied() const
{
    std::string out = cmd_utils::CaptureOutput("powercfg /getactivescheme");
    return out.find(kGUID) != std::string::npos;
}

// ─── DisableUSBSuspendTweak ───────────────────────────────────────────────────
// Registry path controlling USB selective suspend for the active power plan
static const wchar_t* kUSBKey =
    L"SYSTEM\\CurrentControlSet\\Services\\USB";

bool DisableUSBSuspendTweak::Apply()
{
    // Value 0 = disable selective suspend
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kUSBKey,
                                          L"DisableSelectiveSuspend", 1);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableUSBSuspendTweak::Revert()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kUSBKey,
                                          L"DisableSelectiveSuspend", 0);
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableUSBSuspendTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kUSBKey,
                                          L"DisableSelectiveSuspend");
    return val.has_value() && *val == 1;
}
