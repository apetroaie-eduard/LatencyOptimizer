#include "registry_tweaks.h"
#include "../utils/registry_utils.h"

// ─── DisableVisualEffectsTweak ───────────────────────────────────────────────
// VisualFXSetting 3 = custom, MinAnimate=0 disables window animations
static const wchar_t* kDesktopKey =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VisualEffects";
static const wchar_t* kDWMKey   = L"SOFTWARE\\Policies\\Microsoft\\Windows\\DWM";

bool DisableVisualEffectsTweak::Apply()
{
    bool ok = registry_utils::WriteDword(HKEY_CURRENT_USER, kDesktopKey,
                                          L"VisualFXSetting", 3);
    ok &= registry_utils::WriteDword(HKEY_CURRENT_USER,
        L"Control Panel\\Desktop\\WindowMetrics", L"MinAnimate", 0);
    ok &= registry_utils::WriteDword(HKEY_CURRENT_USER,
        L"Control Panel\\Desktop", L"UserPreferencesMask", 0x90);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableVisualEffectsTweak::Revert()
{
    bool ok = registry_utils::WriteDword(HKEY_CURRENT_USER, kDesktopKey,
                                          L"VisualFXSetting", 0);
    ok &= registry_utils::WriteDword(HKEY_CURRENT_USER,
        L"Control Panel\\Desktop\\WindowMetrics", L"MinAnimate", 1);
    // Restore default UserPreferencesMask (0x9E3E07FE typical)
    ok &= registry_utils::WriteDword(HKEY_CURRENT_USER,
        L"Control Panel\\Desktop", L"UserPreferencesMask", 0x9E3E07FE);
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableVisualEffectsTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_CURRENT_USER, kDesktopKey, L"VisualFXSetting");
    return val.has_value() && *val == 3;
}

// ─── DisableWindowsTipsTweak ─────────────────────────────────────────────────
static const wchar_t* kContentDelivery =
    L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\ContentDeliveryManager";

bool DisableWindowsTipsTweak::Apply()
{
    bool ok = registry_utils::WriteDword(HKEY_CURRENT_USER, kContentDelivery,
                                          L"SoftLandingEnabled", 0);
    ok &= registry_utils::WriteDword(HKEY_CURRENT_USER, kContentDelivery,
                                      L"SubscribedContentEnabled", 0);
    ok &= registry_utils::WriteDword(HKEY_CURRENT_USER, kContentDelivery,
                                      L"SystemPaneSuggestionsEnabled", 0);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableWindowsTipsTweak::Revert()
{
    bool ok = registry_utils::WriteDword(HKEY_CURRENT_USER, kContentDelivery,
                                          L"SoftLandingEnabled", 1);
    ok &= registry_utils::WriteDword(HKEY_CURRENT_USER, kContentDelivery,
                                      L"SubscribedContentEnabled", 1);
    ok &= registry_utils::WriteDword(HKEY_CURRENT_USER, kContentDelivery,
                                      L"SystemPaneSuggestionsEnabled", 1);
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableWindowsTipsTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_CURRENT_USER, kContentDelivery,
                                          L"SoftLandingEnabled");
    return val.has_value() && *val == 0;
}

// ─── DisableGameBarTweak ─────────────────────────────────────────────────────
static const wchar_t* kGameBarKey =
    L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\GameDVR";
static const wchar_t* kGameDVRKey =
    L"System\\GameConfigStore";

bool DisableGameBarTweak::Apply()
{
    bool ok = registry_utils::WriteDword(HKEY_CURRENT_USER, kGameBarKey,
                                          L"AppCaptureEnabled", 0);
    ok &= registry_utils::WriteDword(HKEY_CURRENT_USER, kGameDVRKey,
                                      L"GameDVR_Enabled", 0);
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE,
        L"SOFTWARE\\Policies\\Microsoft\\Windows\\GameDVR",
        L"AllowGameDVR", 0);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableGameBarTweak::Revert()
{
    bool ok = registry_utils::WriteDword(HKEY_CURRENT_USER, kGameBarKey,
                                          L"AppCaptureEnabled", 1);
    ok &= registry_utils::WriteDword(HKEY_CURRENT_USER, kGameDVRKey,
                                      L"GameDVR_Enabled", 1);
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE,
        L"SOFTWARE\\Policies\\Microsoft\\Windows\\GameDVR",
        L"AllowGameDVR", 1);
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableGameBarTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_CURRENT_USER, kGameBarKey, L"AppCaptureEnabled");
    return val.has_value() && *val == 0;
}
