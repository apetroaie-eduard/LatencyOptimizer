#include "service_utils.h"
#include <vector>

namespace service_utils {

namespace {
    SC_HANDLE OpenSCM()
    {
        return OpenSCManagerW(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
    }
} // anonymous namespace

bool StartService(const std::wstring& serviceName)
{
    SC_HANDLE hSCM = OpenSCM();
    if (!hSCM) return false;

    SC_HANDLE hSvc = OpenServiceW(hSCM, serviceName.c_str(), SERVICE_START);
    bool ok = hSvc && ::StartServiceW(hSvc, 0, nullptr);
    if (hSvc) CloseServiceHandle(hSvc);
    CloseServiceHandle(hSCM);
    return ok;
}

bool StopService(const std::wstring& serviceName)
{
    SC_HANDLE hSCM = OpenSCM();
    if (!hSCM) return false;

    SC_HANDLE hSvc = OpenServiceW(hSCM, serviceName.c_str(), SERVICE_STOP);
    bool ok = false;
    if (hSvc)
    {
        SERVICE_STATUS ss{};
        ok = ControlService(hSvc, SERVICE_CONTROL_STOP, &ss) != 0;
        CloseServiceHandle(hSvc);
    }
    CloseServiceHandle(hSCM);
    return ok;
}

bool SetStartType(const std::wstring& serviceName, DWORD startType)
{
    SC_HANDLE hSCM = OpenSCM();
    if (!hSCM) return false;

    SC_HANDLE hSvc = OpenServiceW(hSCM, serviceName.c_str(), SERVICE_CHANGE_CONFIG);
    bool ok = false;
    if (hSvc)
    {
        ok = ChangeServiceConfigW(hSvc, SERVICE_NO_CHANGE, startType,
                                   SERVICE_NO_CHANGE, nullptr, nullptr, nullptr,
                                   nullptr, nullptr, nullptr, nullptr) != 0;
        CloseServiceHandle(hSvc);
    }
    CloseServiceHandle(hSCM);
    return ok;
}

DWORD GetStartType(const std::wstring& serviceName)
{
    SC_HANDLE hSCM = OpenSCM();
    if (!hSCM) return SERVICE_NO_CHANGE;

    SC_HANDLE hSvc = OpenServiceW(hSCM, serviceName.c_str(), SERVICE_QUERY_CONFIG);
    DWORD startType = SERVICE_NO_CHANGE;
    if (hSvc)
    {
        DWORD needed = 0;
        QueryServiceConfigW(hSvc, nullptr, 0, &needed);

        std::vector<BYTE> buf(needed);
        auto* cfg = reinterpret_cast<QUERY_SERVICE_CONFIGW*>(buf.data());
        if (QueryServiceConfigW(hSvc, cfg, needed, &needed))
            startType = cfg->dwStartType;

        CloseServiceHandle(hSvc);
    }
    CloseServiceHandle(hSCM);
    return startType;
}

DWORD GetServiceState(const std::wstring& serviceName)
{
    SC_HANDLE hSCM = OpenSCM();
    if (!hSCM) return 0;

    SC_HANDLE hSvc = OpenServiceW(hSCM, serviceName.c_str(), SERVICE_QUERY_STATUS);
    DWORD state = 0;
    if (hSvc)
    {
        SERVICE_STATUS ss{};
        if (QueryServiceStatus(hSvc, &ss))
            state = ss.dwCurrentState;
        CloseServiceHandle(hSvc);
    }
    CloseServiceHandle(hSCM);
    return state;
}

} // namespace service_utils
