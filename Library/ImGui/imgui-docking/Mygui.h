#pragma once

#include <imgui.h>
#include "../../Library/DebugSupporter/DebugSupporter.h"

namespace ImGui
{
	static const char* DDSTextureFilter = "Texture Files(*.dds)\0*.dds;\0All Files(*.*)\0*.*;\0\0";
	static const char* JsonFilter = "Texture Files(*.json)\0*.json;\0All Files(*.*)\0*.*;\0\0";

	/// <summary>
	/// ファイルを開いてパスを取得するボタン（ファイルを開く用）
	/// </summary>
	/// <param name="title"></param>
	/// <param name="resultPath"></param>
	/// <param name="filter">例 "Texture Files(*.dds)\0*.dds;\0All Files(*.*)\0*.*;\0\0" </param>
	/// <returns></returns>
	static bool OpenDialogBotton(const char* title, std::string* resultPath, const char* filter = "All Files(*.*)\0*.*;\0\0")
	{
		if (ImGui::Button(title))
		{
			// ダイアログを開く
			std::string filepath;
			std::string currentDirectory;
			Debug::Dialog::DialogResult result = Debug::Dialog::OpenFileName(filepath, currentDirectory, filter);
			// ファイルを選択したら
			if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
			{
				*resultPath = filepath;
				return true;
			}
		}
		return false;
	}
	/// <summary>
	/// ファイルを開いてパスを取得するボタン（ファイルを保存する用）
	/// </summary>
	/// <param name="title"></param>
	/// <param name="resultPath"></param>
	/// <param name="filter"></param>
	/// <returns></returns>
	static bool SaveDialogBotton(const char* title, std::string* resultPath, const char* filter = "All Files(*.*)\0*.*;\0\0")
	{
		if (ImGui::Button(title))
		{
			// ダイアログを開く
			Debug::Dialog::DialogResult result = Debug::Dialog::SaveFileName(
				resultPath,
				filter,
				title,
				"txt");
			if (result == Debug::Dialog::DialogResult::OK)
			{
				return true;
			}
		}
		return false;
	}
}