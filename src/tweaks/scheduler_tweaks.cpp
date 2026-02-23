#include "scheduler_tweaks.h"
#include "../utils/registry_utils.h"

static const wchar_t* kPriSepKey =
    L"SYSTEM\\CurrentControlSet\\Control\\PriorityControl";
static const wchar_t* kMMCSSGamesKey =
    L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Multimedia\\SystemProfile\\Tasks\\Games";
static const wchar_t* kMMCSSProAudioKey =
    L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Multimedia\\SystemProfile\\Tasks\\Pro Audio";

// ─── Win32PrioritySeparationTweak ─────────────────────────────────────────────

bool Win32PrioritySeparationTweak::Apply()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kPriSepKey,
                                          L"Win32PrioritySeparation", 26);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool Win32PrioritySeparationTweak::Revert()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kPriSepKey,
                                          L"Win32PrioritySeparation", 2);
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool Win32PrioritySeparationTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kPriSepKey,
                                          L"Win32PrioritySeparation");
    return val.has_value() && *val == 26;
}

// ─── GameCPUPriorityTweak ────────────────────────────────────────────────────

bool GameCPUPriorityTweak::Apply()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kMMCSSGamesKey,
                                          L"GPU Priority", 8);
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kMMCSSGamesKey,
                                      L"Priority", 6);
    ok &= registry_utils::WriteString(HKEY_LOCAL_MACHINE, kMMCSSGamesKey,
                                       L"Scheduling Category", L"High");
    ok &= registry_utils::WriteString(HKEY_LOCAL_MACHINE, kMMCSSGamesKey,
                                       L"SFIO Priority", L"High");
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool GameCPUPriorityTweak::Revert()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kMMCSSGamesKey,
                                          L"GPU Priority", 2);
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kMMCSSGamesKey,
                                      L"Priority", 2);
    ok &= registry_utils::WriteString(HKEY_LOCAL_MACHINE, kMMCSSGamesKey,
                                       L"Scheduling Category", L"Medium");
    ok &= registry_utils::WriteString(HKEY_LOCAL_MACHINE, kMMCSSGamesKey,
                                       L"SFIO Priority", L"Normal");
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool GameCPUPriorityTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kMMCSSGamesKey, L"GPU Priority");
    return val.has_value() && *val == 8;
}

// ─── SFIOPriorityTweak ────────────────────────────────────────────────────────

bool SFIOPriorityTweak::Apply()
{
    bool ok = registry_utils::WriteString(HKEY_LOCAL_MACHINE, kMMCSSProAudioKey,
                                           L"SFIO Priority", L"High");
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kMMCSSProAudioKey,
                                      L"Priority", 6);
    ok &= registry_utils::WriteString(HKEY_LOCAL_MACHINE, kMMCSSProAudioKey,
                                       L"Scheduling Category", L"High");
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool SFIOPriorityTweak::Revert()
{
    bool ok = registry_utils::WriteString(HKEY_LOCAL_MACHINE, kMMCSSProAudioKey,
                                           L"SFIO Priority", L"Normal");
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kMMCSSProAudioKey,
                                      L"Priority", 3);
    ok &= registry_utils::WriteString(HKEY_LOCAL_MACHINE, kMMCSSProAudioKey,
                                       L"Scheduling Category", L"Medium");
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool SFIOPriorityTweak::IsApplied() const
{
    auto val = registry_utils::ReadString(HKEY_LOCAL_MACHINE, kMMCSSProAudioKey, L"SFIO Priority");
    return val.has_value() && *val == L"High";
}
