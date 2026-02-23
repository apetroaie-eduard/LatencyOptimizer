#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

namespace service_utils {

// Start a Windows service by name
bool StartService(const std::wstring& serviceName);

// Stop a Windows service by name
bool StopService(const std::wstring& serviceName);

// Set the start type of a service (e.g. SERVICE_DISABLED, SERVICE_DEMAND_START, SERVICE_AUTO_START)
bool SetStartType(const std::wstring& serviceName, DWORD startType);

// Query current start type of a service
DWORD GetStartType(const std::wstring& serviceName);

// Query current service state (SERVICE_RUNNING, SERVICE_STOPPED, etc.)
DWORD GetServiceState(const std::wstring& serviceName);

} // namespace service_utils
