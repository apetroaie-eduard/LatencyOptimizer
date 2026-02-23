#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <vector>
#include <chrono>

// A single saved registry value
struct RegValueEntry {
    HKEY        root;
    std::wstring subKey;
    std::wstring valueName;
    DWORD        type;
    std::vector<BYTE> data;
};

// A single saved service state
struct ServiceEntry {
    std::wstring serviceName;
    DWORD        startType;
};

// One restore point – a named snapshot of all backed-up values
struct RestorePoint {
    std::string  name;            // e.g. tweak name or "Manual"
    std::string  timestamp;       // ISO-8601 string
    std::vector<RegValueEntry> regValues;
    std::vector<ServiceEntry>  services;
};

class BackupManager {
public:
    BackupManager() = default;

    // Save the current value of a registry DWORD before a tweak modifies it.
    void BackupRegDword(HKEY root, const std::wstring& subKey,
                        const std::wstring& valueName);

    // Save the current value of a registry string before a tweak modifies it.
    void BackupRegString(HKEY root, const std::wstring& subKey,
                         const std::wstring& valueName);

    // Save the current start type of a service.
    void BackupService(const std::wstring& serviceName);

    // Restore all backed-up values from all restore points.
    bool RestoreAll();

    // Restore only the most recent restore point.
    bool RestoreLatest();

    // Create a named restore point from the current backup state.
    void CreateRestorePoint(const std::string& name);

    // Export the restore-point log to a file (UTF-8 text).
    bool ExportLog(const std::wstring& filePath) const;

    // Get human-readable status of all restore points.
    std::vector<std::string> GetStatus() const;

    // Clear all stored backup state.
    void Clear();

    const std::vector<RestorePoint>& RestorePoints() const { return m_points; }

private:
    std::vector<RestorePoint> m_points;
    RestorePoint              m_current; // accumulates until CreateRestorePoint

    static std::string CurrentTimestamp();
    static bool ApplyRestorePoint(const RestorePoint& rp);
};
