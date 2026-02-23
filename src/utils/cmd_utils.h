#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

namespace cmd_utils {

// Run a command with ShellExecuteEx (can request elevation)
// Returns the process exit code, or -1 on failure.
int RunCommand(const std::wstring& exe, const std::wstring& args,
               bool asAdmin = false, bool waitForExit = true);

// Run a command via cmd.exe /c and capture stdout as a narrow string
std::string CaptureOutput(const std::string& command);

} // namespace cmd_utils
