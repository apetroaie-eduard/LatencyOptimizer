#pragma once
#include <string>
#include <memory>

// Risk level for a tweak
enum class TweakRisk {
    Safe,    // Very unlikely to cause issues; easily reversible
    Medium,  // May affect some features; reversible
    High     // Could cause instability on some hardware/configs
};

// Compatibility hint
enum class TweakCompat {
    All,         // Works on all Windows 10/11 builds
    Win10Only,   // Windows 10 only
    Win11Only,   // Windows 11 only
    NvidiaOnly,  // NVIDIA GPU required
    AdvancedHW   // Requires specific hardware support
};

// Applied/reverted status
enum class TweakStatus {
    Unknown,
    Applied,
    Reverted,
    Failed
};

class TweakBase {
public:
    virtual ~TweakBase() = default;

    // Human-readable name
    virtual const char* Name() const = 0;

    // One-sentence description shown in the list
    virtual const char* Description() const = 0;

    // Longer explanation shown in the detail popup
    virtual const char* Detail() const = 0;

    // Category label
    virtual const char* Category() const = 0;

    virtual TweakRisk   Risk()   const = 0;
    virtual TweakCompat Compat() const = 0;

    // Apply the tweak.  Returns true on success.
    virtual bool Apply() = 0;

    // Revert the tweak to the original / safe default.  Returns true on success.
    virtual bool Revert() = 0;

    // Query whether the tweak appears to be currently applied.
    virtual bool IsApplied() const = 0;

    // Whether this tweak uses BackupManager before applying
    virtual bool RequiresBackup() const { return false; }

    // Last operation result
    TweakStatus LastStatus() const { return m_lastStatus; }

protected:
    TweakStatus m_lastStatus = TweakStatus::Unknown;
};

using TweakPtr = std::shared_ptr<TweakBase>;
