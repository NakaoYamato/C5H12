#include "ChestItemMenuController.h"

#include "../../Library/Actor/UI/UIActor.h"
#include "../../Library/Algorithm/Converter.h"

#include <imgui.h>

// 開始処理
void ChestItemMenuController::Start()
{
    _userDataManager = ResourceManager::Instance().GetResourceAs<UserDataManager>("UserDataManager");

    _spriteRenderer = this->GetActor()->GetComponent<SpriteRenderer>();
    if (auto spriteRenderer = _spriteRenderer.lock())
	{
		if (!spriteRenderer->IsLoaded())
		{
			spriteRenderer->LoadTexture(PourchBackSpr, L"");
			spriteRenderer->LoadTexture(StrageBackSpr, L"");
			spriteRenderer->LoadTexture(TextBoxSpr, L"");
		}
	}

	// アイテムの前面スプライト
	_itemFrontSprite = std::make_unique<Sprite>(L"Data/Texture/UI/Chest/ItemFront.png", Sprite::LeftUp);
	_itemFrontSprite->GetRectTransform().SetReflectParentScale(false);
	_itemFrontSprite->GetRectTransform().SetLocalScale(Vector2(0.4f, 0.4f));
	// アイテムの背景スプライト
	_itemBackSprite = std::make_unique<Sprite>(L"Data/Texture/UI/Chest/ItemBack.png", Sprite::LeftUp);
	if (!LoadFromFile())
	{
		// ポーチ内アイテム用トランスフォーム初期化
		_pourchItemTransform.SetLocalPosition(Vector2(0.0f, 7.5f));
		_pourchItemTransform.SetLocalScale(Vector2(0.4f, 0.4f));
		_pourchItemTransform.SetReflectParentScale(false);
	}
}

// 更新処理
void ChestItemMenuController::Update(float elapsedTime)
{
	auto spriteRenderer = _spriteRenderer.lock();
	if (!spriteRenderer)
		return;
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;

	// 選択中のタブに合わせて背景トランスフォーム、説明テキスト更新
	_itemName.text = L"";
	_itemDescription.text = L"";
	Vector2 localPosition = Vector2::Zero;
	switch (_tab)
	{
	case ChestItemMenuController::Tab::Pourch:
		// 選択している背景を明るくする
		spriteRenderer->SetColorAlpha(PourchBackSpr, 0.5f);
		spriteRenderer->SetColorAlpha(StrageBackSpr, 0.25f);

		localPosition = _pourchItemTransform.GetLocalPosition() + _pourchItemOffset * static_cast<float>(_currentIndex);
		_itemFrontSprite->GetRectTransform().SetLocalPosition(localPosition);
		_itemFrontSprite->UpdateTransform(&spriteRenderer->GetRectTransform(PourchBackSpr));

		{
			auto pouchItem = &userDataManager->GetPouchItems()[_currentIndex];
			if (!pouchItem)
				break;
			auto itemData = userDataManager->GetAcquiredItemData(pouchItem->itemIndex);
			if (!itemData)
				break;
			auto baseData = itemData->GetBaseData();
			if (!baseData)
				break;

			_itemName.text = ToUtf16(baseData->name);
			_itemDescription.text = ToUtf16(baseData->description);
		}
		break;
	case ChestItemMenuController::Tab::Strage:
		// 選択している背景を明るくする
		spriteRenderer->SetColorAlpha(PourchBackSpr, 0.25f);
		spriteRenderer->SetColorAlpha(StrageBackSpr, 0.5f);

		localPosition = _strageItemTransform.GetLocalPosition();
		localPosition.x += (_currentIndex % _strageItemColumnIndex) * _strageItemOffset.x;
		localPosition.y += (_currentIndex / _strageItemColumnIndex) * _strageItemOffset.y;
		_itemFrontSprite->GetRectTransform().SetLocalPosition(localPosition);
		_itemFrontSprite->UpdateTransform(&spriteRenderer->GetRectTransform(StrageBackSpr));

		{
			auto iter = userDataManager->GetAcquiredItemDataMap().find(_currentIndex);
			if (iter == userDataManager->GetAcquiredItemDataMap().end())
				break;
			auto baseData = iter->second.GetBaseData();
			if (!baseData)
				break;

			_itemName.text = ToUtf16(baseData->name);
			_itemDescription.text = ToUtf16(baseData->description);
		}
		break;
	}

	// ポーチ内アイテム用トランスフォーム更新
	_pourchItemTransform.UpdateTransform(&spriteRenderer->GetRectTransform(PourchBackSpr));
	// ストレージ内アイテム用トランスフォーム更新
	_strageItemTransform.UpdateTransform(&spriteRenderer->GetRectTransform(StrageBackSpr));
}

// 3D描画後の描画処理
void ChestItemMenuController::DelayedRender(const RenderContext& rc)
{
	auto& textureRenderer = GetActor()->GetScene()->GetTextureRenderer();

	// ポーチメニュー描画
	RenderPourch(rc);

	// ストレージメニュー描画
	RenderStrage(rc);

	// アイテムの前面描画
	_itemFrontSprite->Render(rc, textureRenderer);

	// テキスト描画
	GetActor()->GetScene()->GetTextRenderer().Draw(_itemName);
	GetActor()->GetScene()->GetTextRenderer().Draw(_itemDescription);
}

// GUI描画
void ChestItemMenuController::DrawGui()
{
	if (ImGui::TreeNode(u8"ポーチ内アイテムトランスフォーム"))
	{
		_pourchItemTransform.DrawGui();
		ImGui::DragFloat2(u8"アイテムオフセット", &_pourchItemOffset.x, 1.0f);
		ImGui::DragFloat2(u8"アイテムスケール", &_pourchItemScale.x, 0.1f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode(u8"ストレージ内アイテムトランスフォーム"))
	{
		_strageItemTransform.DrawGui();
		ImGui::DragFloat2(u8"アイテムオフセット", &_strageItemOffset.x, 1.0f);
		ImGui::DragFloat2(u8"アイテムスケール", &_strageItemScale.x, 0.1f);
		ImGui::DragInt(u8"列数", &_strageItemColumnIndex);
		ImGui::DragInt(u8"行数", &_strageItemRowIndex);
		ImGui::DragInt(u8"ページ数", &_strageItemMaxPage);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode(u8"アイテム前面スプライト"))
	{
		_itemFrontSprite->DrawGui();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode(u8"アイテム背景スプライト"))
	{
		_itemBackSprite->DrawGui();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode(u8"個数表示用パラメータ"))
	{
		ImGui::DragFloat2(u8"オフセット", &_itemQuantityOffset.x, 1.0f);
		ImGui::DragFloat2(u8"スケール", &_itemQuantityScale.x, 0.1f);
		ImGui::TreePop();
	}
	_itemName.DrawGui(u8"名前");
	_itemDescription.DrawGui(u8"説明");
}

// インデックス追加
void ChestItemMenuController::AddIndex(int val)
{
	switch (_tab)
	{
	case ChestItemMenuController::Tab::Pourch:
		// ストレージへ移動
		if (val >= +_strageItemColumnIndex)
		{
			_tab = Tab::Strage;
		}
		else if (_currentIndex + val >= UserDataManager::MaxPouchItemCount && val == +1)
		{
			_currentIndex = 0;
		}
		// 左端から右端へ移動
		else if (_currentIndex + val < 0)
		{
			_currentIndex = (UserDataManager::MaxPouchItemCount - 1);
		}
		// 右端から左端へ移動
		else if (_currentIndex + val >= UserDataManager::MaxPouchItemCount)
		{
			_currentIndex = 0;
		}
		else
		{
			_currentIndex += val;
		}
		break;
	case ChestItemMenuController::Tab::Strage:
		// 左端から右端へ移動
		if (_currentIndex % _strageItemColumnIndex == 0 &&
			val == -1)
		{
			_currentIndex += _strageItemColumnIndex - 1;
		}
		// 右端から左端へ移動
		else if (_currentIndex % _strageItemColumnIndex == _strageItemColumnIndex - 1 && val == +1)
		{
			_currentIndex -= _strageItemColumnIndex - 1;
		}
		// 下端から上端へ移動
		else if (_currentIndex / _strageItemColumnIndex == (_strageItemRowIndex - 1) && val >= +_strageItemColumnIndex)
		{
			_currentIndex = (_currentIndex + val) % (_strageItemColumnIndex);
		}
		// 上端から下端へ移動
		else if (_currentIndex / _strageItemColumnIndex == 0 && val <= -_strageItemColumnIndex)
		{
			_currentIndex = (_strageItemRowIndex - 1) * _strageItemColumnIndex + (_currentIndex % _strageItemColumnIndex);
		}
		else
		{
			_currentIndex += val;
		}
		break;
	}
}

// 次へ進む
void ChestItemMenuController::NextState()
{
	//switch (_state)
	//{
	//case ChestItemMenuController::State::Pourch:

	//	break;
	//case ChestItemMenuController::State::Strage:
	//	break;
	//}
}

// 前の状態へ戻る
// メニューを閉じる場合はtrueを返す
bool ChestItemMenuController::PreviousState()
{
	//switch (_state)
	//{
	//case ChestItemMenuController::State::Pourch:
	//	return true;
	//case ChestItemMenuController::State::Strage:
	//	_state = State::Pourch;
	//	_currentIndex = std::clamp(
	//		_currentIndex,
	//		0,
	//		UserDataManager::MaxPouchItemCount - 1);
	//	break;
	//}

	return false;
}

// リセット
void ChestItemMenuController::Reset()
{
	_tab = Tab::Pourch;
	_state = State::ItmeSelect;
	_currentIndex = 0;
}

// タブ切り替え
void ChestItemMenuController::ChangeTab()
{
	switch (_tab)
	{
	case ChestItemMenuController::Tab::Pourch:
		_tab = Tab::Strage;
		break;
	case ChestItemMenuController::Tab::Strage:
		_tab = Tab::Pourch;
		break;
	}
}

#pragma region 入出力
// ファイル読み込み
bool ChestItemMenuController::LoadFromFile()
{
	std::string filePath = this->GetDirectory();
	filePath += this->GetName();
	filePath += ".json";

	nlohmann::json json;
	if (Exporter::LoadJsonFile(filePath, &json))
	{
		_pourchItemTransform.LoadFromFile(json["PourchItemTransform"]);
		_pourchItemOffset.x = json.value("PourchItemOffsetX", 0.0f);
		_pourchItemOffset.y = json.value("PourchItemOffsetY", 0.0f);
		_pourchItemScale.x = json.value("PourchItemScaleX", 1.0f);
		_pourchItemScale.y = json.value("PourchItemScaleY", 1.0f);
		_strageItemTransform.LoadFromFile(json["StrageItemTransform"]);
		_strageItemOffset.x = json.value("StrageItemOffsetX", 0.0f);
		_strageItemOffset.y = json.value("StrageItemOffsetY", 0.0f);
		_strageItemScale.x = json.value("StrageItemScaleX", 1.0f);
		_strageItemScale.y = json.value("StrageItemScaleY", 1.0f);
		_strageItemColumnIndex = json.value("StrageItemColumnIndex", 5);
		_strageItemRowIndex = json.value("StrageItemRowIndex", 10);
		_strageItemMaxPage = json.value("StrageItemMaxPage", 10);

        _itemQuantityOffset.x = json.value("ItemQuantityOffsetX", 0.0f);
        _itemQuantityOffset.y = json.value("ItemQuantityOffsetY", 0.0f);
        _itemQuantityScale.x = json.value("ItemQuantityScaleX", 1.0f);
        _itemQuantityScale.y = json.value("ItemQuantityScaleY", 1.0f);

		if (json.contains("Text"))
		{
			auto& sub = json["Text"];
			_itemName.position.x = sub.value("itemNamePositionX", 0.0f);
			_itemName.position.y = sub.value("itemNamePositionY", 0.0f);
			_itemName.scale.x = sub.value("itemNameScaleX", 1.0f);
			_itemName.scale.y = sub.value("itemNameScaleY", 1.0f);
			_itemName.color.x = sub.value("itemNameColorR", 1.0f);
			_itemName.color.y = sub.value("itemNameColorG", 1.0f);
			_itemName.color.z = sub.value("itemNameColorB", 1.0f);
			_itemName.color.w = sub.value("itemNameColorA", 1.0f);
			_itemName.origin.x = sub.value("itemNameOriginX", 0.0f);
			_itemName.origin.y = sub.value("itemNameOriginY", 0.0f);

			_itemDescription.position.x = sub.value("itemDescriptionPositionX", 0.0f);
			_itemDescription.position.y = sub.value("itemDescriptionPositionY", 0.0f);
			_itemDescription.scale.x = sub.value("itemDescriptionScaleX", 1.0f);
			_itemDescription.scale.y = sub.value("itemDescriptionScaleY", 1.0f);
			_itemDescription.color.x = sub.value("itemDescriptionColorR", 1.0f);
			_itemDescription.color.y = sub.value("itemDescriptionColorG", 1.0f);
			_itemDescription.color.z = sub.value("itemDescriptionColorB", 1.0f);
			_itemDescription.color.w = sub.value("itemDescriptionColorA", 1.0f);
			_itemDescription.origin.x = sub.value("itemDescriptionOriginX", 0.0f);
			_itemDescription.origin.y = sub.value("itemDescriptionOriginY", 0.0f);
		}
	}
	return false;
}

// ファイル保存
bool ChestItemMenuController::SaveToFile()
{
	std::string filePath = this->GetDirectory();
	filePath += this->GetName();
	filePath += ".json";

	nlohmann::json json;
	_pourchItemTransform.SaveToFile(json["PourchItemTransform"]);
	json["PourchItemOffsetX"] = _pourchItemOffset.x;
	json["PourchItemOffsetY"] = _pourchItemOffset.y;
	json["PourchItemScaleX"] = _pourchItemScale.x;
	json["PourchItemScaleY"] = _pourchItemScale.y;
	_strageItemTransform.SaveToFile(json["StrageItemTransform"]);
	json["StrageItemOffsetX"] = _strageItemOffset.x;
	json["StrageItemOffsetY"] = _strageItemOffset.y;
	json["StrageItemScaleX"] = _strageItemScale.x;
	json["StrageItemScaleY"] = _strageItemScale.y;
	json["StrageItemColumnIndex"] = _strageItemColumnIndex;
	json["StrageItemRowIndex"] = _strageItemRowIndex;
	json["StrageItemMaxPage"] = _strageItemMaxPage;

	json["ItemQuantityOffsetX"] = _itemQuantityOffset.x;
	json["ItemQuantityOffsetY"] = _itemQuantityOffset.y;
    json["ItemQuantityScaleX"] = _itemQuantityScale.x;
    json["ItemQuantityScaleY"] = _itemQuantityScale.y;

	{
		auto& sub = json["Text"];
		sub["itemNamePositionX"] = _itemName.position.x;
		sub["itemNamePositionY"] = _itemName.position.y;
		sub["itemNameScaleX"] = _itemName.scale.x;
		sub["itemNameScaleY"] = _itemName.scale.y;
		sub["itemNameColorR"] = _itemName.color.x;
		sub["itemNameColorG"] = _itemName.color.y;
		sub["itemNameColorB"] = _itemName.color.z;
		sub["itemNameColorA"] = _itemName.color.w;
		sub["itemNameOriginX"] = _itemName.origin.x;
		sub["itemNameOriginY"] = _itemName.origin.y;

		sub["itemDescriptionPositionX"] = _itemDescription.position.x;
		sub["itemDescriptionPositionY"] = _itemDescription.position.y;
		sub["itemDescriptionScaleX"] = _itemDescription.scale.x;
		sub["itemDescriptionScaleY"] = _itemDescription.scale.y;
		sub["itemDescriptionColorR"] = _itemDescription.color.x;
		sub["itemDescriptionColorG"] = _itemDescription.color.y;
		sub["itemDescriptionColorB"] = _itemDescription.color.z;
		sub["itemDescriptionColorA"] = _itemDescription.color.w;
		sub["itemDescriptionOriginX"] = _itemDescription.origin.x;
		sub["itemDescriptionOriginY"] = _itemDescription.origin.y;
	}

	return Exporter::SaveJsonFile(filePath, json);
}
#pragma endregion

// ポーチメニュー描画
void ChestItemMenuController::RenderPourch(const RenderContext& rc)
{
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;
	auto spriteRenderer = _spriteRenderer.lock();
	if (!spriteRenderer)
		return;
	auto& textureRenderer = GetActor()->GetScene()->GetTextureRenderer();
	auto itemManager = ResourceManager::Instance().GetResourceAs<ItemManager>("ItemManager");
	if (!itemManager)
		return;
	auto iconCanvas = itemManager->GetItemIconCanvas();
	if (!iconCanvas)
		return;
	Vector2 canvasScale = {
		static_cast<float>(itemManager->GetItemIconCanvas()->GetCanvasScale().x),
		static_cast<float>(itemManager->GetItemIconCanvas()->GetCanvasScale().y)
	};

	float alpha = spriteRenderer->GetOverallAlpha();
	// 開いていないなら半透明
	if (_tab != Tab::Pourch)
	{
		alpha *= 0.5f;
	}

	// ポーチ内のアイテム描画
	auto PourchItemDraw = [&](int indexOffset)
		{
			int pouchItemIndex = indexOffset;
			Vector2 position = _pourchItemTransform.GetWorldPosition() +
				_pourchItemOffset * static_cast<float>(indexOffset);
			Vector2 scale = _pourchItemTransform.GetWorldScale();

			// 背景描画
			_itemBackSprite->Render(rc, textureRenderer, position, scale);

			auto pouchItem = &userDataManager->GetPouchItems()[pouchItemIndex];
			if (!pouchItem)
				return;

			auto itemData = userDataManager->GetAcquiredItemData(pouchItem->itemIndex);
			if (!itemData)
				return;
			auto baseData = itemData->GetBaseData();
			if (!baseData)
				return;

			auto texturedata = itemManager->GetItemIconTextureData(baseData->iconIndex);
			Vector4 color = baseData->color;
			color.w *= alpha;
			textureRenderer.Render(
				rc,
				iconCanvas->GetColorSRV().GetAddressOf(),
				canvasScale,
				position,
				_pourchItemScale,
				texturedata->texPosition,
				texturedata->texSize,
				Vector2::Zero,
				_pourchItemTransform.GetWorldAngle(),
				color);
			// オーバーレイアイコンの描画
			auto overlayIconTexture = itemManager->GetOverlayIconTexture();
			if (overlayIconTexture && baseData->overlayIconIndex >= 0)
			{
				auto texturedata = itemManager->GetItemIconTextureData(baseData->overlayIconIndex);
				color = Vector4::White;
				color.w *= alpha;
				GetActor()->GetScene()->GetTextureRenderer().Render(
					rc,
					overlayIconTexture,
					position,
					_pourchItemScale,
					texturedata->texPosition,
					texturedata->texSize,
					Vector2::Zero,
					_pourchItemTransform.GetWorldAngle(),
					color);
			}
			// 個数表示
			bool isMax = baseData->maxCountInpouch == -1 ||
				pouchItem->quantity == baseData->maxCountInpouch;
			GetActor()->GetScene()->GetTextRenderer().Draw(
				FontType::MSGothic,
				pouchItem->quantity != -1 ? std::to_string(pouchItem->quantity).c_str() : "inf",
				position + _itemQuantityOffset,
				isMax ? Vector4::Red : Vector4::White,
				_pourchItemTransform.GetWorldAngle(),
				Vector2::Zero,
				_itemQuantityScale);
		};
	// ポーチ内のアイテム描画
	for (int i = 0; i < UserDataManager::MaxPouchItemCount; ++i)
	{
		PourchItemDraw(i);
	}
}

// ストレージメニュー描画
void ChestItemMenuController::RenderStrage(const RenderContext& rc)
{
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;
	auto spriteRenderer = _spriteRenderer.lock();
	if (!spriteRenderer)
		return;
	auto& textureRenderer = GetActor()->GetScene()->GetTextureRenderer();
	auto itemManager = ResourceManager::Instance().GetResourceAs<ItemManager>("ItemManager");
	if (!itemManager)
		return;
	auto iconCanvas = itemManager->GetItemIconCanvas();
	if (!iconCanvas)
		return;
	Vector2 canvasScale = {
		static_cast<float>(itemManager->GetItemIconCanvas()->GetCanvasScale().x),
		static_cast<float>(itemManager->GetItemIconCanvas()->GetCanvasScale().y)
	};

	float alpha = spriteRenderer->GetOverallAlpha();
	// 開いていないなら半透明
	if (_tab != Tab::Strage)
	{
		alpha *= 0.5f;
	}

	int strageItemCount = _strageItemColumnIndex * _strageItemRowIndex;
	// ストレージの背景描画
	for (int index = 0; index < strageItemCount; ++index)
	{
		Vector2 position = _strageItemTransform.GetWorldPosition();
		position.x += (index % _strageItemColumnIndex) * _strageItemOffset.x;
		position.y += (index / _strageItemColumnIndex) * _strageItemOffset.y;
		Vector2 scale = _strageItemTransform.GetWorldScale();

		// 背景描画
		_itemBackSprite->Render(rc, textureRenderer, position, scale);
	}

	// ストレージ内のアイテム描画
	int drawIndex = 0;
	for (auto& [index, itemData] : userDataManager->GetAcquiredItemDataMap())
	{
		Vector2 position = _strageItemTransform.GetWorldPosition();
		position.x += (drawIndex % _strageItemColumnIndex) * _strageItemOffset.x;
		position.y += (drawIndex / _strageItemColumnIndex) * _strageItemOffset.y;

		auto baseData = itemData.GetBaseData();
		if (!baseData)
			continue;

		auto texturedata = itemManager->GetItemIconTextureData(baseData->iconIndex);
		Vector4 color = baseData->color;
		color.w *= alpha;
		textureRenderer.Render(
			rc,
			iconCanvas->GetColorSRV().GetAddressOf(),
			canvasScale,
			position,
			_strageItemScale,
			texturedata->texPosition,
			texturedata->texSize,
			Vector2::Zero,
			_pourchItemTransform.GetWorldAngle(),
			color);
		// オーバーレイアイコンの描画
		auto overlayIconTexture = itemManager->GetOverlayIconTexture();
		if (overlayIconTexture && baseData->overlayIconIndex >= 0)
		{
			auto texturedata = itemManager->GetItemIconTextureData(baseData->overlayIconIndex);
			color = Vector4::White;
			color.w *= alpha;
			GetActor()->GetScene()->GetTextureRenderer().Render(
				rc,
				overlayIconTexture,
				position,
				_pourchItemScale,
				texturedata->texPosition,
				texturedata->texSize,
				Vector2::Zero,
				_pourchItemTransform.GetWorldAngle(),
				color);
		}
		// 個数表示
		bool isMax = baseData->maxCountInpouch == -1 ||
			itemData.quantity == baseData->maxCountInpouch;
		GetActor()->GetScene()->GetTextRenderer().Draw(
			FontType::MSGothic,
			itemData.quantity != -1 ? std::to_string(itemData.quantity).c_str() : "inf",
			position + _itemQuantityOffset,
			isMax ? Vector4::Red : Vector4::White,
			_pourchItemTransform.GetWorldAngle(),
			Vector2::Zero,
			_itemQuantityScale);

		drawIndex++;
	}
}
