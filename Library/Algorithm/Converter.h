#pragma once

#include <string>

#include "../../External/nameof/include/nameof.hpp"
#include "../../External/magic_enum/include/magic_enum/magic_enum.hpp"

// 変数名の文字列化
#define _TO_STRING_U8(str) std::string(#str)

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
std::wstring ToWString(const std::string& str);

/// <summary>
/// UTF-8文字列からUTF-16文字列へ変換
/// </summary>
/// <param name="str"></param>
/// <returns></returns>
std::wstring ToUtf16(const std::string& str);

/// <summary>
/// enumをstringに変換
/// </summary>
/// <param name="index"></param>
/// <returns>string</returns>
template<class E>
std::string ToString(size_t index) {
	return nameof::nameof_enum(magic_enum::enum_value<E>(index)).data();
}

/// <summary>
/// クラス名を文字列に変換
/// </summary>
/// <returns>string</returns>
template<class T>
std::string ClassToString()
{
	std::string className = typeid(T).name();
	// 不要な部分を削除
	className = className.substr(6); // "class "分を削除
	return className;
}
