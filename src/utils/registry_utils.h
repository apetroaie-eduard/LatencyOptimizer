#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <optional>
#include <variant>

namespace registry_utils {

// Write a DWORD value to the registry (creates key if needed)
bool WriteDword(HKEY root, const std::wstring& subKey, const std::wstring& valueName, DWORD data);

// Write a string (REG_SZ) value to the registry
bool WriteString(HKEY root, const std::wstring& subKey, const std::wstring& valueName, const std::wstring& data);

// Read a DWORD value from the registry
std::optional<DWORD> ReadDword(HKEY root, const std::wstring& subKey, const std::wstring& valueName);

// Read a string value from the registry
std::optional<std::wstring> ReadString(HKEY root, const std::wstring& subKey, const std::wstring& valueName);

// Delete a value from the registry
bool DeleteValue(HKEY root, const std::wstring& subKey, const std::wstring& valueName);

// Check whether a registry value exists
bool ValueExists(HKEY root, const std::wstring& subKey, const std::wstring& valueName);

// Create a registry key (and all intermediate keys)
bool CreateKey(HKEY root, const std::wstring& subKey);

} // namespace registry_utils
