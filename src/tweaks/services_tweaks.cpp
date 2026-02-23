#include "services_tweaks.h"
#include "../utils/service_utils.h"
#include "../utils/registry_utils.h"

// ─── DisableSysMainTweak ─────────────────────────────────────────────────────

bool DisableSysMainTweak::Apply()
{
    bool ok = service_utils::StopService(L"SysMain")
           || true; // tolerate already-stopped
    ok &= service_utils::SetStartType(L"SysMain", SERVICE_DISABLED);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableSysMainTweak::Revert()
{
    bool ok = service_utils::SetStartType(L"SysMain", SERVICE_AUTO_START);
    if (ok) service_utils::StartService(L"SysMain");
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableSysMainTweak::IsApplied() const
{
    return service_utils::GetStartType(L"SysMain") == SERVICE_DISABLED;
}

// ─── DisableWSearchTweak ─────────────────────────────────────────────────────

bool DisableWSearchTweak::Apply()
{
    bool ok = service_utils::StopService(L"WSearch") || true;
    ok &= service_utils::SetStartType(L"WSearch", SERVICE_DISABLED);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableWSearchTweak::Revert()
{
    bool ok = service_utils::SetStartType(L"WSearch", SERVICE_AUTO_START);
    if (ok) service_utils::StartService(L"WSearch");
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableWSearchTweak::IsApplied() const
{
    return service_utils::GetStartType(L"WSearch") == SERVICE_DISABLED;
}

// ─── DisableWUDOTweak ────────────────────────────────────────────────────────
// DODownloadMode: 0 = disabled, 1 = LAN only, 3 = LAN+internet
static const wchar_t* kDOKey =
    L"SOFTWARE\\Policies\\Microsoft\\Windows\\DeliveryOptimization";

bool DisableWUDOTweak::Apply()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kDOKey, L"DODownloadMode", 0);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableWUDOTweak::Revert()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kDOKey, L"DODownloadMode", 1);
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableWUDOTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kDOKey, L"DODownloadMode");
    return val.has_value() && *val == 0;
}

// ─── DisablePrintSpoolerTweak ───────────────────────────────────────────────

bool DisablePrintSpoolerTweak::Apply()
{
    bool ok = service_utils::StopService(L"Spooler") || true;
    ok &= service_utils::SetStartType(L"Spooler", SERVICE_DISABLED);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisablePrintSpoolerTweak::Revert()
{
    bool ok = service_utils::SetStartType(L"Spooler", SERVICE_AUTO_START);
    if (ok) service_utils::StartService(L"Spooler");
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisablePrintSpoolerTweak::IsApplied() const
{
    return service_utils::GetStartType(L"Spooler") == SERVICE_DISABLED;
}

// ─── DisableDiagServicesTweak ───────────────────────────────────────────────

bool DisableDiagServicesTweak::Apply()
{
    bool ok = service_utils::StopService(L"DiagSvc") || true;
    ok &= service_utils::SetStartType(L"DiagSvc", SERVICE_DISABLED);
    service_utils::StopService(L"dmwappushservice");
    ok &= service_utils::SetStartType(L"dmwappushservice", SERVICE_DISABLED);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableDiagServicesTweak::Revert()
{
    bool ok = service_utils::SetStartType(L"DiagSvc", SERVICE_DEMAND_START);
    ok &= service_utils::SetStartType(L"dmwappushservice", SERVICE_DEMAND_START);
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableDiagServicesTweak::IsApplied() const
{
    return service_utils::GetStartType(L"DiagSvc") == SERVICE_DISABLED;
}
