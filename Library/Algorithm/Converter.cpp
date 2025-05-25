#include "Converter.h"

#define WIN32_LEAN_AND_MEAM
#include <Windows.h>

#include <filesystem>

/// ワイド文字列からマルチバイト文字へ変換
std::string ToString(const std::wstring& str)
{
    return std::filesystem::path(str).u8string();
}

/// マルチバイト文字列からワイド文字列へ変換
std::wstring ToWString(const std::string& str)
{
    return std::filesystem::path(str);
}
