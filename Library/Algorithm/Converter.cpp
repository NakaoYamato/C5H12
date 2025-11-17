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

std::wstring ToUtf16(const std::string& str)
{
	if (str.empty()) {
		return std::wstring();
	}

	int requiredSize = MultiByteToWideChar(
		CP_UTF8,				// 変換元はUTF-8
		0,						// オプションフラグ
		str.c_str(),			// 変換元文字列
		(int)str.length(),		// 文字列の長さ (NULL終端を含めない)
		NULL,					// NULLで第一引数として呼び出すと、
		0						// 必要なバッファサイズ（ワイド文字数）が返る
	);

	if (requiredSize == 0) 
	{
		// 変換失敗
		return std::wstring();
	}

	std::wstring utf16Str(requiredSize, 0);

	int result = MultiByteToWideChar(
		CP_UTF8,
		0,
		str.c_str(),
		(int)str.length(),
		&utf16Str[0],       // 書き込み先バッファ
		requiredSize        // バッファサイズ
	);

	if (result == 0) 
	{
		// 変換失敗
		return std::wstring();
	}

	return utf16Str;
}
