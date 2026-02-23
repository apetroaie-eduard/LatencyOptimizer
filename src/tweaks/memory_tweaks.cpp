#include "memory_tweaks.h"
#include "../utils/registry_utils.h"
#include "../utils/cmd_utils.h"

static const wchar_t* kMemMgmtKey =
    L"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Memory Management";

// ─── DisablePagingExecutiveTweak ──────────────────────────────────────────────

bool DisablePagingExecutiveTweak::Apply()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kMemMgmtKey,
                                          L"DisablePagingExecutive", 1);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisablePagingExecutiveTweak::Revert()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kMemMgmtKey,
                                          L"DisablePagingExecutive", 0);
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisablePagingExecutiveTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kMemMgmtKey,
                                          L"DisablePagingExecutive");
    return val.has_value() && *val == 1;
}

// ─── LargeSystemCacheTweak ────────────────────────────────────────────────────

bool LargeSystemCacheTweak::Apply()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kMemMgmtKey,
                                          L"LargeSystemCache", 1);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool LargeSystemCacheTweak::Revert()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kMemMgmtKey,
                                          L"LargeSystemCache", 0);
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool LargeSystemCacheTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kMemMgmtKey,
                                          L"LargeSystemCache");
    return val.has_value() && *val == 1;
}

// ─── DisableMemoryCompressionTweak ────────────────────────────────────────────
// Managed via PowerShell (Disable-MMAgent -mc)

bool DisableMemoryCompressionTweak::Apply()
{
    int rc = cmd_utils::RunCommand(L"powershell",
        L"-NoProfile -Command \"Disable-MMAgent -MemoryCompression\"", false, true);
    m_lastStatus = (rc == 0) ? TweakStatus::Applied : TweakStatus::Failed;
    return rc == 0;
}

bool DisableMemoryCompressionTweak::Revert()
{
    int rc = cmd_utils::RunCommand(L"powershell",
        L"-NoProfile -Command \"Enable-MMAgent -MemoryCompression\"", false, true);
    m_lastStatus = (rc == 0) ? TweakStatus::Reverted : TweakStatus::Failed;
    return rc == 0;
}

bool DisableMemoryCompressionTweak::IsApplied() const
{
    std::string out = cmd_utils::CaptureOutput(
        "powershell -NoProfile -Command \"(Get-MMAgent).MemoryCompression\"");
    return out.find("False") != std::string::npos
        || out.find("false") != std::string::npos;
}
