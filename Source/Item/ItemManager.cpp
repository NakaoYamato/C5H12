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

	// アイテム効果処理リスト初期化
	_itemFunctionList.push_back(std::make_unique<HealingPotionFunc>());
	_itemFunctionList.push_back(std::make_unique<StrengthPotionFunc>());
	_itemFunctionList.push_back(std::make_unique<ElixirPotionFunc>());

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
	// 各種類リスト
	static std::vector<char> ItemTypeNames;
	// ソート用リスト
	static std::vector<char> SortedItemTypeNames;
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

		SortedItemTypeNames = ItemTypeNames;
		SortedItemTypeNames.push_back('A');
		SortedItemTypeNames.push_back('l');
		SortedItemTypeNames.push_back('l');
		SortedItemTypeNames.push_back('\0');

		ItemTypeNames.push_back('\0');
		SortedItemTypeNames.push_back('\0');
	}

	// 各名前リスト
	static std::vector<char> ItemFunctionNames;
	if (ItemFunctionNames.empty())
	{
		for (size_t i = 0; i < _itemFunctionList.size(); ++i)
		{
			for (char c : _itemFunctionList[i]->GetName())
			{
				ItemFunctionNames.push_back(c);
			}
			ItemFunctionNames.push_back('\0');
		}
	}

	ImGui::Combo(u8"ソート", reinterpret_cast<int*>(&_selectedSortType), SortedItemTypeNames.data(), static_cast<int>(ItemType::ItemTypeMax));
	ImGui::Separator();
	if (ImGui::Button("Add"))
	{
		ItemData data;
		if (_selectedSortType != ItemType::ItemTypeMax)
		{
			data.type = _selectedSortType;
		}
		_itemDataList.push_back(data);
	}
	ImGui::Separator();

	size_t index = 0;
	for (auto& data : _itemDataList)
	{
		// ソート処理
		if (_selectedSortType != ItemType::ItemTypeMax &&
			data.type != _selectedSortType)
		{
			index++;
			continue;
		}

		if (ImGui::TreeNode(std::to_string(index).c_str()))
		{
			ImGui::MyColumns(3, "user_item_colums", true, { 300.0f, 460.0f });

			ImGui::SetNextItemWidth(100.0f);
			if (ImGui::InputInt(u8"アイコン番号", &data.iconIndex))
				data.iconIndex = std::clamp<int>(data.iconIndex, -1, _itemIconTextureIndex - 1);
			ImGui::SetNextItemWidth(100.0f);
			if (ImGui::InputInt(u8"オーバーレイアイコン番号", &data.overlayIconIndex))
				data.overlayIconIndex = std::clamp<int>(data.overlayIconIndex, -1, 7);
			// アイテムアイコンGui描画
			DrawItemIconGui(data.iconIndex, data.overlayIconIndex, data.color);

			// ItemDataGui描画
			ImGui::NextColumn();
			{
				ImGui::SetNextItemWidth(200.0f);
				ImGui::InputText(u8"名前", &data.name);

				ImGui::InputTextMultiline(u8"説明", &data.description, ImVec2(200.0f, 100.0f));

				ImGui::ColorEdit4(u8"表示色", &data.color.x, ImGuiColorEditFlags_NoInputs);

				ImGui::Combo(u8"種類", reinterpret_cast<int*>(&data.type), ItemTypeNames.data(), static_cast<int>(ItemType::ItemTypeMax));

				ImGui::SetNextItemWidth(100.0f);
				ImGui::Checkbox(u8"ポーチ内に入るかどうか", &data.isInPouch);
				ImGui::SetNextItemWidth(100.0f);
				ImGui::SameLine();
				ImGui::InputInt(u8"ポーチ内最大所持数", &data.maxCountInpouch);
				data.maxCountInpouch = std::clamp<int>(data.maxCountInpouch, -1, 99);

				ImGui::SetNextItemWidth(100.0f);
				ImGui::InputInt(u8"レア度", &data.rarity);
			}

			ImGui::NextColumn();
			if (ImGui::TreeNode(u8"アイテム実行処理"))
			{
				if (ImGui::TreeNode(u8"パラメータ"))
				{
					for (auto& [name, parm] : data.parameters)
					{
						if (auto p = std::get_if<int>(&parm))
						{
							ImGui::DragInt(name.c_str(), p);
						}
						else if (auto p = std::get_if<float>(&parm))
						{
							ImGui::DragFloat(name.c_str(), p);
						}
						else if (auto p = std::get_if<Vector2>(&parm))
						{
							ImGui::DragFloat2(name.c_str(), &p->x);
						}
						else if (auto p = std::get_if<Vector3>(&parm))
						{
							ImGui::DragFloat3(name.c_str(), &p->x);
						}
						else if (auto p = std::get_if<Vector4>(&parm))
						{
							ImGui::DragFloat4(name.c_str(), &p->x);
						}
					}
					ImGui::TreePop();
				}

				for (int index = -1; index < static_cast<int>(_itemFunctionList.size()); ++index)
				{
					if (index == -1)
					{
						if (ImGui::RadioButton(u8"なし", data.executeProcessIndex == index))
						{
							data.executeProcessIndex = index;
						}
						continue;
					}

					auto& func = _itemFunctionList[index];
					if (ImGui::RadioButton(func->GetName().c_str(), data.executeProcessIndex == index))
					{
						data.executeProcessIndex = index;
						// パラメータ初期化
						data.parameters = func->GetParameterKeys();
					}
				}
				ImGui::TreePop();
			}

			ImGui::NextColumn();
			ImGui::Columns(1);

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
			ImVec2(128.0f, 128.0f),
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
				ImVec2(128.0f, 128.0f),
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

// アイコン番号からUV取得
Canvas::TextureData* ItemManager::GetItemIconTextureData(int iconIndex)
{
	if (_itemIconTextureMap.find(iconIndex) == _itemIconTextureMap.end())
		return nullptr;
	return &_itemIconTextureMap[iconIndex];
}

// オーバーレイアイコンテクスチャのUV取得
Canvas::TextureData ItemManager::GetOverlayIconTextureData(int overlayIconIndex)
{
	Canvas::TextureData textureData;
	if (overlayIconIndex < 0 || overlayIconIndex >= 8)
		return textureData;
	const float uvWidth = 1.0f / 8.0f;
	textureData.texPosition.x = static_cast<float>(overlayIconIndex) * uvWidth;
	textureData.texPosition.y = 0.0f;
	textureData.texSize.x = uvWidth;
	textureData.texSize.y = 1.0f;
	return textureData;
}
