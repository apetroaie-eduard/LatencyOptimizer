#include "timers_tweaks.h"
#include "../utils/cmd_utils.h"

// ─── HighResTimerTweak ────────────────────────────────────────────────────────

bool HighResTimerTweak::Apply()
{
    // Enable HPET / platform clock
    int rc = cmd_utils::RunCommand(L"bcdedit", L"/set useplatformclock true", false, true);
    // Also set the TSCSYNC flag for invariant TSC
    cmd_utils::RunCommand(L"bcdedit", L"/set tscsyncpolicy enhanced", false, true);
    m_lastStatus = (rc == 0) ? TweakStatus::Applied : TweakStatus::Failed;
    return rc == 0;
}

bool HighResTimerTweak::Revert()
{
    int rc = cmd_utils::RunCommand(L"bcdedit", L"/deletevalue useplatformclock", false, true);
    cmd_utils::RunCommand(L"bcdedit", L"/deletevalue tscsyncpolicy", false, true);
    m_lastStatus = (rc == 0) ? TweakStatus::Reverted : TweakStatus::Failed;
    return rc == 0;
}

bool HighResTimerTweak::IsApplied() const
{
    std::string out = cmd_utils::CaptureOutput("bcdedit /enum current");
    return out.find("useplatformclock") != std::string::npos
        && out.find("Yes") != std::string::npos;
}

// ─── DisableDynamicTickTweak ──────────────────────────────────────────────────

bool DisableDynamicTickTweak::Apply()
{
    int rc = cmd_utils::RunCommand(L"bcdedit",
        L"/set disabledynamictick yes", false, true);
    m_lastStatus = (rc == 0) ? TweakStatus::Applied : TweakStatus::Failed;
    return rc == 0;
}

bool DisableDynamicTickTweak::Revert()
{
    int rc = cmd_utils::RunCommand(L"bcdedit",
        L"/deletevalue disabledynamictick", false, true);
    m_lastStatus = (rc == 0) ? TweakStatus::Reverted : TweakStatus::Failed;
    return rc == 0;
}

bool DisableDynamicTickTweak::IsApplied() const
{
    std::string out = cmd_utils::CaptureOutput("bcdedit /enum current");
    return out.find("disabledynamictick") != std::string::npos
        && (out.find("Yes") != std::string::npos
            || out.find("yes") != std::string::npos);
}
