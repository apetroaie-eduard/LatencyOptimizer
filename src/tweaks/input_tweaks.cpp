#include "input_tweaks.h"
#include "../utils/registry_utils.h"

static const wchar_t* kMouseKey = L"Control Panel\\Mouse";

// ─── DisableMouseAccelerationTweak ────────────────────────────────────────────

bool DisableMouseAccelerationTweak::Apply()
{
    bool ok = registry_utils::WriteString(HKEY_CURRENT_USER, kMouseKey,
                                           L"MouseSpeed", L"0");
    ok &= registry_utils::WriteString(HKEY_CURRENT_USER, kMouseKey,
                                       L"MouseThreshold1", L"0");
    ok &= registry_utils::WriteString(HKEY_CURRENT_USER, kMouseKey,
                                       L"MouseThreshold2", L"0");
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableMouseAccelerationTweak::Revert()
{
    bool ok = registry_utils::WriteString(HKEY_CURRENT_USER, kMouseKey,
                                           L"MouseSpeed", L"1");
    ok &= registry_utils::WriteString(HKEY_CURRENT_USER, kMouseKey,
                                       L"MouseThreshold1", L"6");
    ok &= registry_utils::WriteString(HKEY_CURRENT_USER, kMouseKey,
                                       L"MouseThreshold2", L"10");
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableMouseAccelerationTweak::IsApplied() const
{
    auto val = registry_utils::ReadString(HKEY_CURRENT_USER, kMouseKey, L"MouseSpeed");
    return val.has_value() && *val == L"0";
}

// ─── MousePollingRateTweak ────────────────────────────────────────────────────
static const wchar_t* kHIDKey =
    L"SYSTEM\\CurrentControlSet\\Services\\mouclass\\Parameters";

bool MousePollingRateTweak::Apply()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kHIDKey,
                                          L"MouseDataQueueSize", 128);
    // SampleRate hint for USB mice
    bool ok2 = registry_utils::WriteDword(HKEY_LOCAL_MACHINE,
        L"SYSTEM\\CurrentControlSet\\Services\\mouhid\\Parameters",
        L"MouseDataQueueSize", 128);
    m_lastStatus = (ok || ok2) ? TweakStatus::Applied : TweakStatus::Failed;
    return ok || ok2;
}

bool MousePollingRateTweak::Revert()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kHIDKey,
                                          L"MouseDataQueueSize", 100);
    registry_utils::WriteDword(HKEY_LOCAL_MACHINE,
        L"SYSTEM\\CurrentControlSet\\Services\\mouhid\\Parameters",
        L"MouseDataQueueSize", 100);
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool MousePollingRateTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kHIDKey,
                                          L"MouseDataQueueSize");
    return val.has_value() && *val == 128;
}

// ─── DisablePointerPrecisionTweak ─────────────────────────────────────────────

bool DisablePointerPrecisionTweak::Apply()
{
    bool ok = registry_utils::WriteString(HKEY_CURRENT_USER, kMouseKey,
                                           L"MouseSpeed", L"0");
    ok &= registry_utils::WriteString(HKEY_CURRENT_USER, kMouseKey,
                                       L"MouseThreshold1", L"0");
    ok &= registry_utils::WriteString(HKEY_CURRENT_USER, kMouseKey,
                                       L"MouseThreshold2", L"0");
    // SPI_SETMOUSE via SystemParametersInfo would be ideal but
    // registry change is persistent across reboots.
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisablePointerPrecisionTweak::Revert()
{
    bool ok = registry_utils::WriteString(HKEY_CURRENT_USER, kMouseKey,
                                           L"MouseSpeed", L"1");
    ok &= registry_utils::WriteString(HKEY_CURRENT_USER, kMouseKey,
                                       L"MouseThreshold1", L"6");
    ok &= registry_utils::WriteString(HKEY_CURRENT_USER, kMouseKey,
                                       L"MouseThreshold2", L"10");
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisablePointerPrecisionTweak::IsApplied() const
{
    auto v1 = registry_utils::ReadString(HKEY_CURRENT_USER, kMouseKey, L"MouseThreshold1");
    return v1.has_value() && *v1 == L"0";
}
