#include "misc_tweaks.h"
#include "../utils/service_utils.h"
#include "../utils/registry_utils.h"

// ─── DisableDeliveryOptimizationTweak ─────────────────────────────────────────

bool DisableDeliveryOptimizationTweak::Apply()
{
    bool ok = service_utils::StopService(L"DoSvc") || true;
    ok &= service_utils::SetStartType(L"DoSvc", SERVICE_DISABLED);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableDeliveryOptimizationTweak::Revert()
{
    bool ok = service_utils::SetStartType(L"DoSvc", SERVICE_DEMAND_START);
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableDeliveryOptimizationTweak::IsApplied() const
{
    return service_utils::GetStartType(L"DoSvc") == SERVICE_DISABLED;
}

// ─── DisableNotificationsTweak ────────────────────────────────────────────────
static const wchar_t* kToastKey =
    L"SOFTWARE\\Policies\\Microsoft\\Windows\\Explorer";
static const wchar_t* kPushNotifKey =
    L"SOFTWARE\\Policies\\Microsoft\\Windows\\CurrentVersion\\PushNotifications";

bool DisableNotificationsTweak::Apply()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kToastKey,
                                          L"DisableNotificationCenter", 1);
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kPushNotifKey,
                                      L"NoToastApplicationNotification", 1);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableNotificationsTweak::Revert()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kToastKey,
                                          L"DisableNotificationCenter", 0);
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kPushNotifKey,
                                      L"NoToastApplicationNotification", 0);
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableNotificationsTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kToastKey,
                                          L"DisableNotificationCenter");
    return val.has_value() && *val == 1;
}

// ─── DisableBackgroundAppsTweak ───────────────────────────────────────────────
static const wchar_t* kBgAppsKey =
    L"SOFTWARE\\Policies\\Microsoft\\Windows\\AppPrivacy";

bool DisableBackgroundAppsTweak::Apply()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kBgAppsKey,
                                          L"LetAppsRunInBackground", 2); // 2 = Force Deny
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableBackgroundAppsTweak::Revert()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kBgAppsKey,
                                          L"LetAppsRunInBackground", 0); // 0 = User chooses
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableBackgroundAppsTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kBgAppsKey,
                                          L"LetAppsRunInBackground");
    return val.has_value() && *val == 2;
}

// ─── DisableTelemetryTweak ────────────────────────────────────────────────────
static const wchar_t* kTelemetryKey =
    L"SOFTWARE\\Policies\\Microsoft\\Windows\\DataCollection";

bool DisableTelemetryTweak::Apply()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kTelemetryKey,
                                          L"AllowTelemetry", 0);
    ok &= service_utils::StopService(L"DiagTrack") || true;
    ok &= service_utils::SetStartType(L"DiagTrack", SERVICE_DISABLED);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableTelemetryTweak::Revert()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kTelemetryKey,
                                          L"AllowTelemetry", 1);
    ok &= service_utils::SetStartType(L"DiagTrack", SERVICE_AUTO_START);
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableTelemetryTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kTelemetryKey, L"AllowTelemetry");
    return val.has_value() && *val == 0;
}

// ─── DisableActivityHistoryTweak ─────────────────────────────────────────────
static const wchar_t* kActivityHistoryKey =
    L"SOFTWARE\\Policies\\Microsoft\\Windows\\System";

bool DisableActivityHistoryTweak::Apply()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kActivityHistoryKey,
                                          L"PublishUserActivities", 0);
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kActivityHistoryKey,
                                      L"UploadUserActivities", 0);
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kActivityHistoryKey,
                                      L"EnableActivityFeed", 0);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableActivityHistoryTweak::Revert()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kActivityHistoryKey,
                                          L"PublishUserActivities", 1);
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kActivityHistoryKey,
                                      L"UploadUserActivities", 1);
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kActivityHistoryKey,
                                      L"EnableActivityFeed", 1);
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableActivityHistoryTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kActivityHistoryKey,
                                          L"PublishUserActivities");
    return val.has_value() && *val == 0;
}

// ─── DisableStorageSenseTweak ────────────────────────────────────────────────
static const wchar_t* kStorageSenseKey =
    L"SOFTWARE\\Policies\\Microsoft\\Windows\\StorageSense";

bool DisableStorageSenseTweak::Apply()
{
    bool ok = registry_utils::CreateKey(HKEY_LOCAL_MACHINE, kStorageSenseKey);
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kStorageSenseKey,
                                      L"AllowStorageSenseGlobal", 0);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableStorageSenseTweak::Revert()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kStorageSenseKey,
                                          L"AllowStorageSenseGlobal", 1);
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableStorageSenseTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kStorageSenseKey,
                                          L"AllowStorageSenseGlobal");
    return val.has_value() && *val == 0;
}

// ─── DisableErrorReportingTweak ──────────────────────────────────────────────
static const wchar_t* kWERKey =
    L"SOFTWARE\\Policies\\Microsoft\\Windows\\Windows Error Reporting";

bool DisableErrorReportingTweak::Apply()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kWERKey,
                                          L"Disabled", 1);
    ok &= service_utils::StopService(L"WerSvc") || true;
    ok &= service_utils::SetStartType(L"WerSvc", SERVICE_DISABLED);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableErrorReportingTweak::Revert()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kWERKey,
                                          L"Disabled", 0);
    ok &= service_utils::SetStartType(L"WerSvc", SERVICE_DEMAND_START);
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableErrorReportingTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kWERKey, L"Disabled");
    return val.has_value() && *val == 1;
}
