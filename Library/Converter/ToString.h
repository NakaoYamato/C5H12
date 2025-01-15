#pragma once

#include <string>

// 変数の文字列化
#define TO_STRING_U8(str) std::string(#str)

/// <summary>
/// ワイド文字列からマルチバイト文字へ変換
/// </summary>
/// <param name="str">ワイド文字</param>
/// <returns>マルチバイト文字</returns>
std::string ToString(const std::wstring& str);

/// <summary> 
/// マルチバイト文字列からワイド文字列へ変換
/// </summary>
/// <param name="str">マルチバイト文字</param>
/// <returns>ワイド文字</returns>
std::wstring ToWideString(const std::string& str);