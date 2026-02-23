#include "backup_manager.h"
#include "utils/registry_utils.h"
#include "utils/service_utils.h"

#include <sstream>
#include <fstream>
#include <ctime>
#include <algorithm>

// ─── helpers ─────────────────────────────────────────────────────────────────

std::string BackupManager::CurrentTimestamp()
{
    std::time_t t = std::time(nullptr);
    char buf[32]{};
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", std::localtime(&t));
    return buf;
}

static std::string HkeyName(HKEY root)
{
    if (root == HKEY_LOCAL_MACHINE)  return "HKLM";
    if (root == HKEY_CURRENT_USER)   return "HKCU";
    if (root == HKEY_CLASSES_ROOT)   return "HKCR";
    if (root == HKEY_USERS)          return "HKU";
    return "HKEY_UNKNOWN";
}

// ─── BackupManager ───────────────────────────────────────────────────────────

void BackupManager::BackupRegDword(HKEY root, const std::wstring& subKey,
                                    const std::wstring& valueName)
{
    auto existing = registry_utils::ReadDword(root, subKey, valueName);
    if (!existing.has_value()) return;

    RegValueEntry entry;
    entry.root      = root;
    entry.subKey    = subKey;
    entry.valueName = valueName;
    entry.type      = REG_DWORD;
    entry.data.resize(sizeof(DWORD));
    *reinterpret_cast<DWORD*>(entry.data.data()) = *existing;
    m_current.regValues.push_back(std::move(entry));
}

void BackupManager::BackupRegString(HKEY root, const std::wstring& subKey,
                                     const std::wstring& valueName)
{
    auto existing = registry_utils::ReadString(root, subKey, valueName);
    if (!existing.has_value()) return;

    RegValueEntry entry;
    entry.root      = root;
    entry.subKey    = subKey;
    entry.valueName = valueName;
    entry.type      = REG_SZ;
    // Store as bytes (including null terminator)
    const wchar_t* str = existing->c_str();
    const BYTE* begin = reinterpret_cast<const BYTE*>(str);
    entry.data = std::vector<BYTE>(begin, begin + (existing->size() + 1) * sizeof(wchar_t));
    m_current.regValues.push_back(std::move(entry));
}

void BackupManager::BackupService(const std::wstring& serviceName)
{
    DWORD startType = service_utils::GetStartType(serviceName);
    if (startType == SERVICE_NO_CHANGE) return;

    ServiceEntry entry;
    entry.serviceName = serviceName;
    entry.startType   = startType;
    m_current.services.push_back(std::move(entry));
}

void BackupManager::CreateRestorePoint(const std::string& name)
{
    m_current.name      = name;
    m_current.timestamp = CurrentTimestamp();
    m_points.push_back(std::move(m_current));
    m_current = RestorePoint{};
}

bool BackupManager::ApplyRestorePoint(const RestorePoint& rp)
{
    bool ok = true;
    for (const auto& rv : rp.regValues)
    {
        if (rv.type == REG_DWORD)
        {
            DWORD val = *reinterpret_cast<const DWORD*>(rv.data.data());
            if (!registry_utils::WriteDword(rv.root, rv.subKey, rv.valueName, val))
                ok = false;
        }
        else if (rv.type == REG_SZ)
        {
            const wchar_t* str = reinterpret_cast<const wchar_t*>(rv.data.data());
            if (!registry_utils::WriteString(rv.root, rv.subKey, rv.valueName, str))
                ok = false;
        }
    }
    for (const auto& svc : rp.services)
    {
        if (!service_utils::SetStartType(svc.serviceName, svc.startType))
            ok = false;
    }
    return ok;
}

bool BackupManager::RestoreAll()
{
    bool ok = true;
    for (const auto& rp : m_points)
        ok &= ApplyRestorePoint(rp);
    return ok;
}

bool BackupManager::RestoreLatest()
{
    if (m_points.empty()) return false;
    return ApplyRestorePoint(m_points.back());
}

bool BackupManager::ExportLog(const std::wstring& filePath) const
{
    std::ofstream f(filePath);
    if (!f.is_open()) return false;

    f << "LatencyOptimizer Backup Log\n";
    f << "============================\n\n";

    for (const auto& rp : m_points)
    {
        f << "[" << rp.timestamp << "] Restore Point: " << rp.name << "\n";
        f << "  Registry values: " << rp.regValues.size() << "\n";
        for (const auto& rv : rp.regValues)
        {
            std::string key(rv.subKey.begin(), rv.subKey.end());
            std::string vname(rv.valueName.begin(), rv.valueName.end());
            f << "    " << HkeyName(rv.root) << "\\" << key
              << " -> " << vname << "\n";
        }
        f << "  Services: " << rp.services.size() << "\n";
        for (const auto& sv : rp.services)
        {
            std::string name(sv.serviceName.begin(), sv.serviceName.end());
            f << "    " << name << " (start type " << sv.startType << ")\n";
        }
        f << "\n";
    }
    return true;
}

std::vector<std::string> BackupManager::GetStatus() const
{
    std::vector<std::string> lines;
    if (m_points.empty())
    {
        lines.push_back("No restore points.");
        return lines;
    }
    for (std::size_t i = 0; i < m_points.size(); ++i)
    {
        const auto& rp = m_points[i];
        std::ostringstream oss;
        oss << "[" << (i + 1) << "] " << rp.timestamp << " - " << rp.name
            << "  (" << rp.regValues.size() << " reg, "
            << rp.services.size() << " svc)";
        lines.push_back(oss.str());
    }
    return lines;
}

void BackupManager::Clear()
{
    m_points.clear();
    m_current = RestorePoint{};
}
