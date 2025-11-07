#pragma once

#include <imgui.h>
#include <filesystem>
#include <string>
#include <vector>
#include "../../Library/DebugSupporter/DebugSupporter.h"

namespace ImGui
{
	static const char* DDSTextureFilter = "Texture Files(*.dds)\0*.dds;\0All Files(*.*)\0*.*;\0\0";
	static const char* JsonFilter = "Json Files(*.json)\0*.json;\0All Files(*.*)\0*.*;\0\0";
	static const char* TextureFilter = "Texture Files(*.dds;*.png;*.tga;*.jpg;*.tif;*.jpeg)\0*.dds;*.png;*.tga;*.jpg;*.tif;*.jpeg;\0All Files(*.*)\0*.*;\0\0";
	static const char* ModelFilter = "Model Files(*.fbx;*.obj)\0*.fbx;*.obj;\0All Files(*.*)\0*.*;\0\0";

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
				// 相対パス取得
				try
				{
					// 相対パス取得
					std::filesystem::path path = std::filesystem::relative(filepath, currentDirectory);
					*resultPath = path.u8string();
				}
				catch (...)
				{
					// 相対パス取得に失敗した場合は絶対パスを使用
					*resultPath = filepath;
				}

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
	static bool SaveDialogBotton(const char* title, std::string* resultPath, const char* filter = "All Files(*.*)\0*.*;\0\0", LPCSTR dialogTitle = nullptr)
	{
		if (ImGui::Button(title))
		{
			// ダイアログを開く
			Debug::Dialog::DialogResult result = Debug::Dialog::SaveFileName(
				resultPath,
				filter,
				dialogTitle,
				"txt");
			if (result == Debug::Dialog::DialogResult::OK)
			{
				return true;
			}
		}
		return false;
	}
	/// <summary>
	/// 画像を表示して、クリックされたらダイアログを開いてパスを更新するボタン
	/// </summary>
	/// <param name="path"></param>
	/// <param name="srv"></param>
	/// <param name="size"></param>
	/// <param name="filter"></param>
	/// <returns></returns>
	static bool ImageEditButton(std::wstring* path, ID3D11ShaderResourceView* srv, const ImVec2& size = ImVec2(128.0f, 128.0f), const char* filter = TextureFilter)
	{
		if (ImGui::ImageButton(srv, ImVec2(128.0f, 128.0f)))
		{
			// ダイアログを開く
			std::string filepath;
			std::string currentDirectory;
			Debug::Dialog::DialogResult result = Debug::Dialog::OpenFileName(filepath, currentDirectory, filter);
			// ファイルを選択したら
			if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
			{
				// 相対パス取得
				std::filesystem::path relativePath = std::filesystem::relative(filepath, currentDirectory);
				// パスを更新
				*path = relativePath.wstring();
				return true; // 画像がクリックされた
			}
		}
		return false; // 画像がクリックされなかった
	}

	/// <summary>
	/// 指定された文字列リストから選択可能なコンボボックスを表示し、選択された項目を更新します。
	/// </summary>
	/// <param name="title">コンボボックスのタイトルとして表示する文字列。</param>
	/// <param name="item">現在選択されている項目の文字列。選択が変更された場合は更新されます。</param>
	/// <param name="items">選択肢として表示する文字列のリスト。</param>
	/// <returns>ユーザーが項目を選択して変更した場合は true、それ以外は false を返します。</returns>
	static bool ComboString(const char* title, std::string* item, const std::vector<const char*> items)
	{
		int currentIndex = -1;
		for (size_t i = 0; i < items.size(); ++i)
		{
			if (*item == items[i])
			{
				currentIndex = static_cast<int>(i);
				break;
			}
		}
		if (ImGui::Combo(title, &currentIndex, items.data(), static_cast<int>(items.size())))
		{
			if (currentIndex >= 0 && currentIndex < static_cast<int>(items.size()))
			{
				*item = items[currentIndex];
				return true;
			}
		}
        return false;
	}

	/// <summary>
	/// Comboのラッパー（std::vector<std::string>版）
	/// </summary>
	/// <param name="title"></param>
	/// <param name="itemIndex"></param>
	/// <param name="items"></param>
	/// <returns></returns>
	static bool Combo(const char* title, int* itemIndex, const std::vector<std::string>& items)
	{
		std::vector<const char*> itemPtrs;
		itemPtrs.reserve(items.size());
		for (const auto& str : items)
		{
			itemPtrs.push_back(str.c_str());
		}
		return ImGui::Combo(title, itemIndex, itemPtrs.data(), static_cast<int>(itemPtrs.size()));
	}
}