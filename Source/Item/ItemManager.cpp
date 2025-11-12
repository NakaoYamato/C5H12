#include "ItemManager.h"

#include "../../Library/Exporter/Exporter.h"
#include "../../Library/Algorithm/Converter.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Graphics/GpuResourceManager.h"
#include "../../Library/2D/SpriteResource.h"

#include <Mygui.h>

// 初期化処理
bool ItemManager::Initialize()
{
	_itemIconCanvas = std::make_unique<Canvas>(
		Graphics::Instance().GetDevice(),
		DirectX::XMUINT2(1280, 1280),
		Vector2(128.0f, 128.0f));
	_itemIconCanvas->SetFilePath("./Data/Resource/ItemIconCanvas.png");

	return true;
}

// ファイル読み込み
bool ItemManager::LoadFromFile()
{
	nlohmann::json jsonData;
	if (Exporter::LoadJsonFile(_filePath, &jsonData))
	{
		_itemDataList.clear();
		if (!jsonData.contains("ItemDataListSize"))
			return false;
		size_t size = jsonData["ItemDataListSize"].get<std::size_t>();
		for (size_t i = 0; i < size; ++i)
		{
			ItemData data;
			data.name = jsonData["ItemDataList" + std::to_string(i) + "name"].get<std::string>();
			data.type = static_cast<ItemType>(jsonData["ItemDataList" + std::to_string(i) + "type"].get<int>());
			data.rarity = jsonData["ItemDataList" + std::to_string(i) + "rarity"].get<int>();
			_itemDataList.push_back(data);
		}
		return true;
	}
	return false;
}

// ファイル保存
bool ItemManager::SaveToFile()
{
	nlohmann::json jsonData;
	jsonData["ItemDataListSize"] = _itemDataList.size();
	for (size_t i = 0; i < _itemDataList.size(); ++i)
	{
		jsonData["ItemDataList" + std::to_string(i) + "name"] = _itemDataList[i].name;
		jsonData["ItemDataList" + std::to_string(i) + "type"] = static_cast<int>(_itemDataList[i].type);
		jsonData["ItemDataList" + std::to_string(i) + "rarity"] = _itemDataList[i].rarity;
	}
	return Exporter::SaveJsonFile(_filePath, jsonData);
}

// Gui描画
void ItemManager::DrawGui()
{
	static std::vector<char> ItemTypeNames;
	static std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> TextureCache;
	if (ItemTypeNames.empty())
	{
		for (size_t i = 0; i < static_cast<size_t>(ItemType::ItemTypeMax); ++i)
		{
			for (char c : ToString<ItemType>(i))
			{
				ItemTypeNames.push_back(c);
			}
			ItemTypeNames.push_back('\0');
		}
	}

	if (ImGui::Button("Add"))
	{
		ItemData data;
		_itemDataList.push_back(data);
	}
	ImGui::Separator();

	size_t index = 0;
	for (auto& data : _itemDataList)
	{
		if (ImGui::TreeNode(std::to_string(index).c_str()))
		{
			ImGui::InputText(u8"名前", &data.name);
			ImGui::Text(u8"テクスチャファイルパス: %ls", data.textureFilePath.c_str());
			ImGui::SameLine();
			if (ImGui::Button("..."))
			{
				// ダイアログを開く
				std::string filepath;
				std::string currentDirectory;
				Debug::Dialog::DialogResult result = Debug::Dialog::OpenFileName(filepath, currentDirectory, ImGui::TextureFilter);
				// ファイルを選択したら
				if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
				{
					try
					{
						// 相対パス取得
						std::filesystem::path path =
							std::filesystem::relative(filepath, currentDirectory);
						data.textureFilePath = path.wstring();
					}
					catch (...)
					{
						data.textureFilePath = ToWString(filepath);
					}
				}
			}
			if (!data.textureFilePath.empty())
			{
				Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv = TextureCache[data.textureFilePath];
				if (srv.Get() == nullptr)
				{
					// テクスチャ読み込み
					GpuResourceManager::LoadTextureFromFile(
						Graphics::Instance().GetDevice(),
						data.textureFilePath.c_str(),
						&srv, nullptr);
				}
				// テクスチャ表示
				if (srv)
				{
					ImGui::Image(
						srv.Get(),
						ImVec2(64.0f, 64.0f),
						ImVec2(0.0f, 0.0f),
						ImVec2(1.0f, 1.0f), 
						ImVec4(data.color.x, data.color.y, data.color.z, data.color.w));
				}
			}
			ImGui::ColorEdit4(u8"表示色", &data.color.x);
			ImGui::Combo(u8"種類", reinterpret_cast<int*>(&data.type), ItemTypeNames.data(), static_cast<int>(ItemType::ItemTypeMax));
			ImGui::InputInt(u8"レア度", &data.rarity);
			if (ImGui::Button(u8"削除"))
			{
				_itemDataList.erase(_itemDataList.begin() + index);
				ImGui::TreePop();
				break;
			}
			ImGui::TreePop();
		}
		index++;
	}

	if (ImGui::TreeNode(u8"アイコンキャンバス"))
	{
		{
			std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());

			if (ImGui::Button(u8"アイコン追加"))
			{
				// ダイアログを開く
				std::string filepath;
				std::string currentDirectory;
				Debug::Dialog::DialogResult result = Debug::Dialog::OpenFileName(filepath, currentDirectory, ImGui::TextureFilter);
				// ファイルを選択したら
				if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
				{
					ToWString(filepath);
					_itemIconCanvas->Load(
						Graphics::Instance().GetDevice(),
						Graphics::Instance().GetDeviceContext(),
						ToWString(filepath).c_str(),
						{ 1,1 });
				}
			}

			_itemIconCanvas->DrawGui(
				Graphics::Instance().GetDevice(),
				Graphics::Instance().GetDeviceContext());
		}
		ImGui::TreePop();
	}
}

// アイテムデータ取得
ItemData* ItemManager::GetItemData(size_t index)
{
	if (index < _itemDataList.size())
	{
		return &_itemDataList[index];
	}
	return nullptr;
}
