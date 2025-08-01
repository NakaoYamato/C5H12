#pragma once

#include <imgui.h>
#include <filesystem>
#include "../../Library/DebugSupporter/DebugSupporter.h"

namespace ImGui
{
	static const char* DDSTextureFilter = "Texture Files(*.dds)\0*.dds;\0All Files(*.*)\0*.*;\0\0";
	static const char* JsonFilter = "Json Files(*.json)\0*.json;\0All Files(*.*)\0*.*;\0\0";
	static const char* TextureFilter = "Texture Files(*.dds;*.png;*.tga;*.jpg;*.tif)\0*.dds;*.png;*.tga;*.jpg;*.tif;\0All Files(*.*)\0*.*;\0\0";
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
}