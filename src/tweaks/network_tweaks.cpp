#include "network_tweaks.h"
#include "../utils/registry_utils.h"
#include "../utils/cmd_utils.h"

// ─── DisableNagleTweak ────────────────────────────────────────────────────────
// Applied to every interface key under TCPIP\Parameters\Interfaces
static const wchar_t* kTcpParamKey =
    L"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters";
static const wchar_t* kTcpInterfacesKey =
    L"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces";

bool DisableNagleTweak::Apply()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kTcpParamKey,
                                          L"TCPNoDelay", 1);
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kTcpParamKey,
                                      L"TcpAckFrequency", 1);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableNagleTweak::Revert()
{
    bool ok = registry_utils::DeleteValue(HKEY_LOCAL_MACHINE, kTcpParamKey, L"TCPNoDelay");
    ok &= registry_utils::DeleteValue(HKEY_LOCAL_MACHINE, kTcpParamKey, L"TcpAckFrequency");
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableNagleTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kTcpParamKey, L"TCPNoDelay");
    return val.has_value() && *val == 1;
}

// ─── DisableNetworkThrottlingTweak ────────────────────────────────────────────
static const wchar_t* kMMCSKey =
    L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Multimedia\\SystemProfile";

bool DisableNetworkThrottlingTweak::Apply()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kMMCSKey,
                                          L"NetworkThrottlingIndex", 0xFFFFFFFF);
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kMMCSKey,
                                      L"SystemResponsiveness", 0);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableNetworkThrottlingTweak::Revert()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kMMCSKey,
                                          L"NetworkThrottlingIndex", 10);
    ok &= registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kMMCSKey,
                                      L"SystemResponsiveness", 20);
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableNetworkThrottlingTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kMMCSKey,
                                          L"NetworkThrottlingIndex");
    return val.has_value() && *val == 0xFFFFFFFF;
}

// ─── DisableTCPAutoTuningTweak ────────────────────────────────────────────────

bool DisableTCPAutoTuningTweak::Apply()
{
    int rc = cmd_utils::RunCommand(L"netsh",
        L"int tcp set global autotuninglevel=disabled", false, true);
    m_lastStatus = (rc == 0) ? TweakStatus::Applied : TweakStatus::Failed;
    return rc == 0;
}

bool DisableTCPAutoTuningTweak::Revert()
{
    int rc = cmd_utils::RunCommand(L"netsh",
        L"int tcp set global autotuninglevel=normal", false, true);
    m_lastStatus = (rc == 0) ? TweakStatus::Reverted : TweakStatus::Failed;
    return rc == 0;
}

bool DisableTCPAutoTuningTweak::IsApplied() const
{
    std::string out = cmd_utils::CaptureOutput("netsh int tcp show global");
    return out.find("disabled") != std::string::npos
        || out.find("Disabled") != std::string::npos;
}

// ─── DisableECNTweak ─────────────────────────────────────────────────────────

bool DisableECNTweak::Apply()
{
    int rc = cmd_utils::RunCommand(L"netsh",
        L"int tcp set global ecncapability=disabled", false, true);
    m_lastStatus = (rc == 0) ? TweakStatus::Applied : TweakStatus::Failed;
    return rc == 0;
}

bool DisableECNTweak::Revert()
{
    int rc = cmd_utils::RunCommand(L"netsh",
        L"int tcp set global ecncapability=default", false, true);
    m_lastStatus = (rc == 0) ? TweakStatus::Reverted : TweakStatus::Failed;
    return rc == 0;
}

bool DisableECNTweak::IsApplied() const
{
    std::string out = cmd_utils::CaptureOutput("netsh int tcp show global");
    auto pos = out.find("ECN");
    if (pos == std::string::npos) return false;
    auto line = out.substr(pos, out.find('\n', pos) - pos);
    return line.find("disabled") != std::string::npos
        || line.find("Disabled") != std::string::npos;
}

// ─── DisableTCPTimestampsTweak ───────────────────────────────────────────────

bool DisableTCPTimestampsTweak::Apply()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kTcpParamKey,
                                          L"Tcp1323Opts", 1);
    m_lastStatus = ok ? TweakStatus::Applied : TweakStatus::Failed;
    return ok;
}

bool DisableTCPTimestampsTweak::Revert()
{
    bool ok = registry_utils::WriteDword(HKEY_LOCAL_MACHINE, kTcpParamKey,
                                          L"Tcp1323Opts", 3);
    m_lastStatus = ok ? TweakStatus::Reverted : TweakStatus::Failed;
    return ok;
}

bool DisableTCPTimestampsTweak::IsApplied() const
{
    auto val = registry_utils::ReadDword(HKEY_LOCAL_MACHINE, kTcpParamKey, L"Tcp1323Opts");
    return val.has_value() && (*val == 0 || *val == 1);
}
