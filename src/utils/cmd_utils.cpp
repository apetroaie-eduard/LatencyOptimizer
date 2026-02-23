#include "cmd_utils.h"
#include <shellapi.h>
#include <array>

namespace cmd_utils {

int RunCommand(const std::wstring& exe, const std::wstring& args, bool asAdmin, bool waitForExit)
{
    SHELLEXECUTEINFOW sei{};
    sei.cbSize       = sizeof(sei);
    sei.fMask        = SEE_MASK_NOCLOSEPROCESS;
    sei.lpVerb       = asAdmin ? L"runas" : L"open";
    sei.lpFile       = exe.c_str();
    sei.lpParameters = args.empty() ? nullptr : args.c_str();
    sei.nShow        = SW_HIDE;

    if (!ShellExecuteExW(&sei))
        return -1;

    int exitCode = 0;
    if (waitForExit && sei.hProcess)
    {
        WaitForSingleObject(sei.hProcess, INFINITE);
        DWORD code = 0;
        GetExitCodeProcess(sei.hProcess, &code);
        exitCode = static_cast<int>(code);
        CloseHandle(sei.hProcess);
    }
    else if (sei.hProcess)
    {
        CloseHandle(sei.hProcess);
    }
    return exitCode;
}

std::string CaptureOutput(const std::string& command)
{
    std::array<char, 256> buf{};
    std::string result;

    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) return result;

    while (fgets(buf.data(), static_cast<int>(buf.size()), pipe))
        result += buf.data();

    _pclose(pipe);
    return result;
}

} // namespace cmd_utils
