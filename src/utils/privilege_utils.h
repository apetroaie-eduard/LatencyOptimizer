#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace privilege_utils {

// Returns true if the current process is running with administrator privileges.
bool IsRunningAsAdmin();

// Re-launches the current executable with "runas" (UAC elevation request).
// Returns false if ShellExecuteEx fails.
bool RelaunchAsAdmin();

} // namespace privilege_utils
