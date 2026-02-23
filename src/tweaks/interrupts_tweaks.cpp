#include "interrupts_tweaks.h"
#include "../utils/registry_utils.h"

// Network device class GUID
static const wchar_t* kNetClassKey =
    L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e972-e325-11ce-bfc1-08002be10318}\\0000";

// ─── NetworkInterruptAffinityTweak ────────────────────────────────────────────

bool NetworkInterruptAffinityTweak::Apply()
{
    // *InterruptAffinityPolicy:  5 = IrqPolicySpecifiedProcessors
    // *InterruptAffinity:        0x01 = CPU 0
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kNetClassKey,
                                          L"*InterruptAffinityPolicy", 5);
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kNetClassKey,
                                      L"*InterruptAffinity", 0x01);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool NetworkInterruptAffinityTweak::Revert()
{
    bool ok = registry_utils::DeleteValue(HKEY_LOCAL_MACHINE, kNetClassKey,
                                           L"*InterruptAffinityPolicy");
    ok &= registry_utils::DeleteValue(HKEY_LOCAL_MACHINE, kNetClassKey,
                                       L"*InterruptAffinity");
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool NetworkInterruptAffinityTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kNetClassKey,
                                          L"*InterruptAffinityPolicy");
    return val.has_value() && *val == 5;
}

// ─── DisableNetworkMSITweak ───────────────────────────────────────────────────
// MSISupported 0 = legacy INTx, 1 = MSI

bool DisableNetworkMSITweak::Apply()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kNetClassKey,
                                          L"MSISupported", 0);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableNetworkMSITweak::Revert()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kNetClassKey,
                                          L"MSISupported", 1);
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableNetworkMSITweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kNetClassKey,
                                          L"MSISupported");
    return val.has_value() && *val == 0;
}
