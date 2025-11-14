#include "ItemManager.h"

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
	_itemIconCanvas->Deserialize(
		Graphics::Instance().GetDevice(),
		Graphics::Instance().GetDeviceContext());

	_overlayIconTexture.Load(
		Graphics::Instance().GetDevice(),
		ToWString("./Data/Texture/UI/Item/OverlayIcons.png").c_str());

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
			auto& sub = jsonData["ItemDataList" + std::to_string(i)]; 
			ItemData data;
			data.Load(sub);
			_itemDataList.push_back(data);
		}
		if (jsonData.contains("ItemIconTextureIndex"))
			_itemIconTextureIndex = jsonData["ItemIconTextureIndex"].get<int>();
		if (jsonData.contains("ItemIconTextureMapSize"))
		{
			size_t mapSize = jsonData["ItemIconTextureMapSize"].get<std::size_t>();
			for (size_t i = 0; i < mapSize; ++i)
			{
				Canvas::TextureData textureData;
				auto& sub = jsonData["ItemIconTextureMapKey" + std::to_string(i)];
				textureData.texPosition.x = sub["texPositionX"].get<float>();
				textureData.texPosition.y = sub["texPositionY"].get<float>();
				textureData.texSize.x = sub["texSizeX"].get<float>();
				textureData.texSize.y = sub["texSizeY"].get<float>();
				_itemIconTextureMap[static_cast<int>(i)] = textureData;
			}
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
		auto& sub = jsonData["ItemDataList" + std::to_string(i)];
		_itemDataList[i].Save(sub);
	}
	jsonData["ItemIconTextureIndex"]	= _itemIconTextureIndex;
	jsonData["ItemIconTextureMapSize"]	= _itemIconTextureMap.size();
	for (const auto& [key, value] : _itemIconTextureMap)
	{
		auto& sub = jsonData["ItemIconTextureMapKey" + std::to_string(key)];
		sub["texPositionX"] = value.texPosition.x;
		sub["texPositionY"] = value.texPosition.y;
		sub["texSizeX"]		= value.texSize.x;
		sub["texSizeY"]		= value.texSize.y;
	}
	return Exporter::SaveJsonFile(_filePath, jsonData);
}

// Gui描画
void ItemManager::DrawGui()
{
	static std::vector<char> ItemTypeNames;
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
			// アイテムアイコンGui描画
			DrawItemIconGui(data.iconIndex, data.overlayIconIndex, data.color);
			data.DrawGui(_itemIconTextureIndex);

			ImGui::TreePop();
		}
		index++;
	}

	ImGui::Separator();
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
				_itemIconTextureMap[_itemIconTextureIndex] = _itemIconCanvas->Load(
					Graphics::Instance().GetDevice(),
					Graphics::Instance().GetDeviceContext(),
					ToWString(filepath).c_str(),
					{ 1,1 });
				_itemIconTextureIndex++;
			}
		}

		_itemIconCanvas->DrawGui(
			Graphics::Instance().GetDevice(),
			Graphics::Instance().GetDeviceContext());

		ImGui::Text(u8"オーバーレイアイコン");
		ImGui::Image(_overlayIconTexture.Get(),
			ImVec2(
				64.0f * _overlayIconTexture.GetTextureSize().x / _overlayIconTexture.GetTextureSize().y,
				64.0f));
	}
}

// アイテムアイコンGui描画
void ItemManager::DrawItemIconGui(int selectedIconIndex, int overlayIconIndex, const Vector4& color)
{
	if (_itemIconTextureMap.find(selectedIconIndex) != _itemIconTextureMap.end())
	{
		// 描画開始位置取得
		ImVec2 startPos = ImGui::GetCursorScreenPos();

		auto& textureData = _itemIconTextureMap[selectedIconIndex];
		const float canvasScaleX = static_cast<float>(_itemIconCanvas->GetCanvasScale().x);
		const float canvasScaleY = static_cast<float>(_itemIconCanvas->GetCanvasScale().y);
		ImVec2 uv0, uv1;
		uv0.x = textureData.texPosition.x / canvasScaleX;
		uv0.y = textureData.texPosition.y / canvasScaleY;
		uv1.x = (textureData.texPosition.x + textureData.texSize.x) / canvasScaleX;
		uv1.y = (textureData.texPosition.y + textureData.texSize.y) / canvasScaleY;
		// テクスチャ表示
		ImGui::Image(
			_itemIconCanvas->GetColorSRV().Get(),
			ImVec2(64.0f, 64.0f),
			uv0,
			uv1,
			ImVec4(color.x, color.y, color.z, color.w));
		// 次のウィジェット位置取得
		ImVec2 nextWidgetPos = ImGui::GetCursorScreenPos();

		// オーバーレイ描画
		if (overlayIconIndex >= 0)
		{
			float overlayUvX0 = static_cast<float>(overlayIconIndex % 8) * (1.0f / 8.0f);
			float overlayUvY0 = 0.0f;
			float overlayUvX1 = overlayUvX0 + (1.0f / 8.0f);
			float overlayUvY1 = 1.0f;
			ImGui::SetCursorScreenPos(startPos);
			ImGui::Image(
				_overlayIconTexture.Get(),
				ImVec2(64.0f, 64.0f),
				ImVec2(overlayUvX0, overlayUvY0),
				ImVec2(overlayUvX1, overlayUvY1),
				ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

			// ウィジェット位置を戻す
			ImGui::SetCursorScreenPos(nextWidgetPos);
		}
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
