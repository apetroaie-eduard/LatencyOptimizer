#include "registry_utils.h"

namespace registry_utils {

bool WriteDword(HKEY root, const std::wstring& subKey, const std::wstring& valueName, DWORD data)
{
    HKEY hKey = nullptr;
    if (RegCreateKeyExW(root, subKey.c_str(), 0, nullptr,
                        REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, nullptr,
                        &hKey, nullptr) != ERROR_SUCCESS)
        return false;

    bool ok = RegSetValueExW(hKey, valueName.c_str(), 0, REG_DWORD,
                              reinterpret_cast<const BYTE*>(&data), sizeof(data)) == ERROR_SUCCESS;
    RegCloseKey(hKey);
    return ok;
}

bool WriteString(HKEY root, const std::wstring& subKey, const std::wstring& valueName,
                 const std::wstring& data)
{
    HKEY hKey = nullptr;
    if (RegCreateKeyExW(root, subKey.c_str(), 0, nullptr,
                        REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, nullptr,
                        &hKey, nullptr) != ERROR_SUCCESS)
        return false;

    DWORD byteLen = static_cast<DWORD>((data.size() + 1) * sizeof(wchar_t));
    bool ok = RegSetValueExW(hKey, valueName.c_str(), 0, REG_SZ,
                              reinterpret_cast<const BYTE*>(data.c_str()), byteLen) == ERROR_SUCCESS;
    RegCloseKey(hKey);
    return ok;
}

std::optional<DWORD> ReadDword(HKEY root, const std::wstring& subKey, const std::wstring& valueName)
{
    HKEY hKey = nullptr;
    if (RegOpenKeyExW(root, subKey.c_str(), 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
        return std::nullopt;

    DWORD data = 0;
    DWORD dataSize = sizeof(data);
    DWORD type = 0;
    bool ok = RegQueryValueExW(hKey, valueName.c_str(), nullptr, &type,
                                reinterpret_cast<BYTE*>(&data), &dataSize) == ERROR_SUCCESS
              && type == REG_DWORD;
    RegCloseKey(hKey);
    return ok ? std::optional<DWORD>(data) : std::nullopt;
}

std::optional<std::wstring> ReadString(HKEY root, const std::wstring& subKey,
                                        const std::wstring& valueName)
{
    HKEY hKey = nullptr;
    if (RegOpenKeyExW(root, subKey.c_str(), 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
        return std::nullopt;

    DWORD dataSize = 0;
    DWORD type = 0;
    if (RegQueryValueExW(hKey, valueName.c_str(), nullptr, &type, nullptr, &dataSize) != ERROR_SUCCESS
        || (type != REG_SZ && type != REG_EXPAND_SZ))
    {
        RegCloseKey(hKey);
        return std::nullopt;
    }

    std::wstring result(dataSize / sizeof(wchar_t), L'\0');
    RegQueryValueExW(hKey, valueName.c_str(), nullptr, &type,
                     reinterpret_cast<BYTE*>(result.data()), &dataSize);
    RegCloseKey(hKey);

    // Strip null terminator if present
    while (!result.empty() && result.back() == L'\0')
        result.pop_back();

    return result;
}

bool DeleteValue(HKEY root, const std::wstring& subKey, const std::wstring& valueName)
{
    HKEY hKey = nullptr;
    if (RegOpenKeyExW(root, subKey.c_str(), 0, KEY_SET_VALUE, &hKey) != ERROR_SUCCESS)
        return false;

    bool ok = RegDeleteValueW(hKey, valueName.c_str()) == ERROR_SUCCESS;
    RegCloseKey(hKey);
    return ok;
}

bool ValueExists(HKEY root, const std::wstring& subKey, const std::wstring& valueName)
{
    return ReadDword(root, subKey, valueName).has_value()
        || ReadString(root, subKey, valueName).has_value();
}

bool CreateKey(HKEY root, const std::wstring& subKey)
{
    HKEY hKey = nullptr;
    bool ok = RegCreateKeyExW(root, subKey.c_str(), 0, nullptr,
                               REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr,
                               &hKey, nullptr) == ERROR_SUCCESS;
    if (ok) RegCloseKey(hKey);
    return ok;
}

} // namespace registry_utils
